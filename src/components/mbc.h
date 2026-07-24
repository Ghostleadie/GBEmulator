//
// Created by Jack_ on 24/07/2026.
//
// Memory Bank Controllers.
//
// The cartridge exposes two windows to the bus: ROM at 0x0000-0x7FFF and
// external RAM at 0xA000-0xBFFF. A plain 32 KiB cart maps straight through, but
// larger carts carry an MBC chip whose control registers -- written *into* the
// ROM address space -- swap which physical bank each window points at. Each
// class below models one chip's banking rules; the bus stays unaware and just
// forwards reads/writes for those two regions to the active mbc.
//

#ifndef GAMEBOYEMULATOR_MBC_H
#define GAMEBOYEMULATOR_MBC_H

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

class mbc
{
public:
	/**
	 * Constructs a mapper over a ROM image and allocates its external RAM.
	 * @param rom Pointer to the full ROM image (owned by the caller).
	 * @param romSize ROM image length in bytes.
	 * @param ramSize External RAM size in bytes (0 if the cart has none).
	 */
	mbc(const uint8_t* rom, uint32_t romSize, uint32_t ramSize)
		: m_rom(rom), m_romSize(romSize)
	{
		m_ram.assign(ramSize, 0);
		// Bank count is derived from the actual file length (a power-of-two
		// multiple of 16 KiB) so bank masking below is a cheap AND.
		m_romBanks = romSize >= 0x4000 ? romSize / 0x4000u : 1;
	}
	virtual ~mbc() = default;

	/**
	 * Reads a byte from the mapped ROM or external RAM window.
	 * Only ROM (0x0000-0x7FFF) and external RAM (0xA000-0xBFFF) addresses reach
	 * here; the bus routes everything else itself.
	 * @param address Raw CPU address within the ROM or RAM window.
	 * @return The mapped byte, or 0xFF when the window is disabled or unmapped.
	 */
	virtual uint8_t read(uint16_t address) = 0;

	/**
	 * Writes a byte to external RAM or a mapper control register (both live in
	 * the ROM/RAM address windows the bus forwards here).
	 * @param address Raw CPU address within the ROM or RAM window.
	 * @param value Byte to write.
	 */
	virtual void write(uint16_t address, uint8_t value) = 0;

	/**
	 * Battery-backed external RAM buffer; empty when the cart carries no RAM.
	 * @return Mutable reference to the cartridge RAM.
	 */
	std::vector<uint8_t>& ram() { return m_ram; }

	/**
	 * Whether this cartridge has a battery, i.e. its RAM (and RTC, if any) should
	 * persist to a .sav file between sessions.
	 * @return True if the cartridge type includes a battery.
	 */
	bool hasBattery() const { return m_battery; }

	/**
	 * Marks the mapper as battery-backed. Set by the factory from the cart type.
	 * @param battery True if the cartridge type includes a battery.
	 */
	void setBattery(bool battery) { m_battery = battery; }

	/**
	 * Whether persistable state changed since the last clearSaveDirty(); used to
	 * throttle .sav writes so a quiet game is not flushed to disk repeatedly.
	 * @return True if RAM (or RTC) has been written since the flag was last cleared.
	 */
	bool saveDirty() const { return m_dirty; }

	/** Clears the save-dirty flag; call right after a successful .sav write. */
	void clearSaveDirty() { m_dirty = false; }

	/**
	 * Serialises battery-backed state to a flat byte blob for a .sav file. The
	 * base implementation emits just the RAM; mappers with extra persistent state
	 * (MBC3's RTC) append it. Non-const because it may first advance the RTC.
	 * @return The bytes to write to disk (empty when the cart has nothing to save).
	 */
	virtual std::vector<uint8_t> serializeSave() { return m_ram; }

	/**
	 * Restores battery-backed state from a blob previously produced by
	 * serializeSave(). Extra trailing bytes (RTC) are ignored by the base class.
	 * @param data Raw .sav bytes; only the leading RAM-sized span is consumed here.
	 */
	virtual void deserializeSave(const std::vector<uint8_t>& data)
	{
		const std::size_t n = std::min(data.size(), m_ram.size());
		std::copy_n(data.begin(), n, m_ram.begin());
	}

protected:
	/**
	 * Fetches a byte from a logical ROM bank, guarding the file bound so an
	 * out-of-range bank (undersized ROM) reads back as open-bus 0xFF.
	 * @param bank Logical 16 KiB ROM bank index.
	 * @param offsetInBank Byte offset within the bank (0-0x3FFF).
	 * @return The ROM byte, or 0xFF if the computed address is past the image.
	 */
	uint8_t romByte(uint32_t bank, uint16_t offsetInBank) const
	{
		const uint32_t addr = bank * 0x4000u + offsetInBank;
		return addr < m_romSize ? m_rom[addr] : 0xFF;
	}

	/** Flags persistable state as changed so the loader will flush it to .sav. */
	void markDirty() { m_dirty = true; }

	const uint8_t* m_rom = nullptr;
	uint32_t m_romSize = 0;
	uint32_t m_romBanks = 1;
	std::vector<uint8_t> m_ram;
	bool m_battery = false;
	bool m_dirty = false;
};

// -----------------------------------------------------------------------------
// No mapper: flat 32 KiB ROM, optional 8 KiB RAM (cart types 0x00/0x08/0x09).
// -----------------------------------------------------------------------------
class mbcNone : public mbc
{
public:
	using mbc::mbc;

	/**
	 * Reads flat 32 KiB ROM directly, or optional external RAM when present.
	 * @param address Raw CPU address (ROM below 0x8000, RAM 0xA000-0xBFFF).
	 * @return The mapped byte, or 0xFF if unmapped or out of range.
	 */
	uint8_t read(uint16_t address) override
	{
		if (address < 0x8000)
			return address < m_romSize ? m_rom[address] : 0xFF;
		if (address >= 0xA000 && address < 0xC000 && !m_ram.empty())
			return m_ram[(address - 0xA000u) & (m_ram.size() - 1)];
		return 0xFF;
	}

	/**
	 * Writes only to external RAM when present; the mapper has no control
	 * registers, so ROM-region writes are dropped.
	 * @param address Raw CPU address (RAM 0xA000-0xBFFF; ROM writes ignored).
	 * @param value Byte to write.
	 */
	void write(uint16_t address, uint8_t value) override
	{
		// No control registers; ROM writes are dropped.
		if (address >= 0xA000 && address < 0xC000 && !m_ram.empty())
		{
			m_ram[(address - 0xA000u) & (m_ram.size() - 1)] = value;
			markDirty();
		}
	}
};

// -----------------------------------------------------------------------------
// MBC1: up to 2 MiB ROM / 32 KiB RAM (cart types 0x01-0x03).
// -----------------------------------------------------------------------------
class mbc1 : public mbc
{
public:
	using mbc::mbc;

	/**
	 * Reads the MBC1 ROM/RAM windows. The low ROM window (0x0000-0x3FFF) is
	 * bank 0, or the upper-bank bits in advanced mode on large carts; the high
	 * window (0x4000-0x7FFF) selects bank2:bank1; RAM (0xA000-0xBFFF) reads the
	 * enabled RAM bank.
	 * @param address Raw CPU address in the ROM or RAM window.
	 * @return The mapped byte, or 0xFF when RAM is disabled/absent or unmapped.
	 */
	uint8_t read(uint16_t address) override
	{
		if (address < 0x4000)
		{
			// Fixed at bank 0 in simple mode; in advanced (mode 1) banking the
			// two upper-bank bits also select this low window on >=1 MiB carts.
			const uint32_t bank = (m_mode ? (m_bank2 << 5) : 0) & (m_romBanks - 1);
			return romByte(bank, address);
		}
		if (address < 0x8000)
		{
			const uint32_t bank = ((m_bank2 << 5) | m_bank1) & (m_romBanks - 1);
			return romByte(bank, address - 0x4000);
		}
		if (address >= 0xA000 && address < 0xC000)
		{
			if (!m_ramEnabled || m_ram.empty()) return 0xFF;
			return m_ram[ramOffset(address)];
		}
		return 0xFF;
	}

	/**
	 * Writes an MBC1 control register or external RAM. 0x0000-0x1FFF enables RAM
	 * (value 0x0A in the low nibble), 0x2000-0x3FFF sets the low 5 ROM-bank bits
	 * (0 becomes 1), 0x4000-0x5FFF sets the 2 upper bank bits, 0x6000-0x7FFF
	 * selects simple/advanced banking mode, and 0xA000-0xBFFF writes RAM when
	 * enabled.
	 * @param address Raw CPU address selecting the register or RAM byte.
	 * @param value Byte written.
	 */
	void write(uint16_t address, uint8_t value) override
	{
		if (address < 0x2000)
			m_ramEnabled = (value & 0x0F) == 0x0A;
		else if (address < 0x4000)
		{
			m_bank1 = value & 0x1F;
			if (m_bank1 == 0) m_bank1 = 1;   // bank 0 is not selectable here
		}
		else if (address < 0x6000)
			m_bank2 = value & 0x03;
		else if (address < 0x8000)
			m_mode = value & 0x01;
		else if (address >= 0xA000 && address < 0xC000)
		{
			if (m_ramEnabled && !m_ram.empty())
			{
				m_ram[ramOffset(address)] = value;
				markDirty();
			}
		}
	}

private:
	/**
	 * Maps a RAM-window address to a byte offset in the RAM buffer. The RAM bank
	 * comes from the upper-bank register, but only in advanced (mode 1) banking.
	 * @param address Raw CPU address in 0xA000-0xBFFF.
	 * @return Wrapped offset into the RAM buffer.
	 */
	uint32_t ramOffset(uint16_t address) const
	{
		const uint32_t bank = m_mode ? m_bank2 : 0;
		return (bank * 0x2000u + (address - 0xA000u)) & (m_ram.size() - 1);
	}

	bool m_ramEnabled = false;
	uint8_t m_bank1 = 1;   // 0x2000-0x3FFF: low 5 ROM bits
	uint8_t m_bank2 = 0;   // 0x4000-0x5FFF: 2 bits (RAM bank or ROM bits 5-6)
	uint8_t m_mode = 0;    // 0x6000-0x7FFF: 0 = simple, 1 = advanced
};

// -----------------------------------------------------------------------------
// MBC2: 256 KiB ROM max, built-in 512 x 4-bit RAM (cart types 0x05/0x06).
// -----------------------------------------------------------------------------
class mbc2 : public mbc
{
public:
	/**
	 * Constructs an MBC2 mapper with its fixed built-in 512 x 4-bit RAM; the
	 * header RAM-size code is ignored.
	 * @param rom Pointer to the full ROM image.
	 * @param romSize ROM image length in bytes.
	 * @param ramSize Header RAM size (unused; built-in RAM is always 512 nibbles).
	 */
	mbc2(const uint8_t* rom, uint32_t romSize, uint32_t /*ramSize*/)
		: mbc(rom, romSize, 512)   // fixed built-in RAM; header RAM size is unused
	{}

	/**
	 * Reads the MBC2 ROM/RAM windows. Low ROM is bank 0, the high window selects
	 * m_romBank; the built-in RAM is 512 4-bit cells mirrored across
	 * 0xA000-0xBFFF, with the upper nibble reading back as ones.
	 * @param address Raw CPU address in the ROM or RAM window.
	 * @return The mapped byte, or 0xFF when RAM is disabled or the address is unmapped.
	 */
	uint8_t read(uint16_t address) override
	{
		if (address < 0x4000)
			return romByte(0, address);
		if (address < 0x8000)
			return romByte(m_romBank & (m_romBanks - 1), address - 0x4000);
		if (address >= 0xA000 && address < 0xC000)
		{
			if (!m_ramEnabled) return 0xFF;
			// 512 half-bytes, mirrored across the whole window; the upper nibble
			// is unmapped and reads back as ones.
			return 0xF0 | (m_ram[(address - 0xA000u) & 0x1FF] & 0x0F);
		}
		return 0xFF;
	}

	/**
	 * Writes an MBC2 control register or built-in RAM. Below 0x4000, address
	 * bit 8 picks the register: clear enables RAM (value 0x0A), set sets the
	 * 4-bit ROM bank (0 becomes 1). RAM writes store the low nibble when enabled.
	 * @param address Raw CPU address selecting the register or RAM cell.
	 * @param value Byte written (low nibble used for RAM).
	 */
	void write(uint16_t address, uint8_t value) override
	{
		if (address < 0x4000)
		{
			// Address bit 8 picks the register: clear = RAM enable, set = ROM bank.
			if (address & 0x0100)
			{
				m_romBank = value & 0x0F;
				if (m_romBank == 0) m_romBank = 1;
			}
			else
				m_ramEnabled = (value & 0x0F) == 0x0A;
		}
		else if (address >= 0xA000 && address < 0xC000)
		{
			if (m_ramEnabled)
			{
				m_ram[(address - 0xA000u) & 0x1FF] = value & 0x0F;
				markDirty();
			}
		}
	}

private:
	bool m_ramEnabled = false;
	uint8_t m_romBank = 1;
};

// -----------------------------------------------------------------------------
// MBC3: up to 2 MiB ROM / 32 KiB RAM + real-time clock (cart types 0x0F-0x13).
//
// The RTC counts real seconds off the host wall clock: it is advanced lazily
// whenever the game latches or the save is serialised, folding the elapsed time
// into the seconds/minutes/hours/day registers (honouring the halt and
// day-carry bits). Because time is measured from a stored base timestamp, the
// clock also keeps advancing while the game is closed once the base is restored
// from a .sav. RTC only ticks on cart types that carry the timer (0x0F/0x10);
// plain MBC3 exposes RAM only.
// -----------------------------------------------------------------------------
class mbc3 : public mbc
{
public:
	/**
	 * Constructs an MBC3 mapper and seeds the RTC base to the current wall-clock
	 * time so a fresh clock starts at zero elapsed.
	 * @param rom Pointer to the full ROM image.
	 * @param romSize ROM image length in bytes.
	 * @param ramSize External RAM size in bytes.
	 */
	mbc3(const uint8_t* rom, uint32_t romSize, uint32_t ramSize)
		: mbc(rom, romSize, ramSize), m_baseTime(nowSeconds())
	{}

	/**
	 * Declares whether this cartridge carries the RTC (TIMER in the cart type).
	 * @param timer True for MBC3+TIMER carts (0x0F/0x10); false for plain MBC3.
	 */
	void setTimer(bool timer) { m_hasTimer = timer; }

	/**
	 * Reads the MBC3 ROM/RAM windows. Low ROM is bank 0, the high window selects
	 * m_romBank; in the RAM window a RAM-bank select of 0x00-0x03 reads external
	 * RAM, while 0x08-0x0C reads the latched RTC register (timer carts only).
	 * @param address Raw CPU address in the ROM or RAM window.
	 * @return The mapped byte, or 0xFF when RAM/RTC access is disabled or absent.
	 */
	uint8_t read(uint16_t address) override
	{
		if (address < 0x4000)
			return romByte(0, address);
		if (address < 0x8000)
			return romByte(m_romBank & (m_romBanks - 1), address - 0x4000);
		if (address >= 0xA000 && address < 0xC000)
		{
			if (!m_ramEnabled) return 0xFF;
			if (m_ramBank <= 0x03)
			{
				if (m_ram.empty()) return 0xFF;
				return m_ram[ramOffset(address)];
			}
			if (m_hasTimer) return readRtc();   // 0x08-0x0C selects an RTC register
			return 0xFF;
		}
		return 0xFF;
	}

	/**
	 * Writes an MBC3 control register, external RAM or an RTC register.
	 * 0x0000-0x1FFF enables RAM/RTC, 0x2000-0x3FFF sets the 7-bit ROM bank
	 * (0 becomes 1), 0x4000-0x5FFF selects a RAM bank (0x00-0x03) or RTC register
	 * (0x08-0x0C), and a 0-then-1 write to 0x6000-0x7FFF latches the clock.
	 * RAM/RTC writes in 0xA000-0xBFFF apply when enabled.
	 * @param address Raw CPU address selecting the register, RAM or RTC byte.
	 * @param value Byte written.
	 */
	void write(uint16_t address, uint8_t value) override
	{
		if (address < 0x2000)
			m_ramEnabled = (value & 0x0F) == 0x0A;
		else if (address < 0x4000)
		{
			m_romBank = value & 0x7F;
			if (m_romBank == 0) m_romBank = 1;
		}
		else if (address < 0x6000)
			m_ramBank = value;   // 0x00-0x03 = RAM bank, 0x08-0x0C = RTC register
		else if (address < 0x8000)
		{
			// A 0-then-1 write latches the live clock into the readable copy.
			if (m_latch == 0 && value == 1)
			{
				syncRtc();               // fold in elapsed real time first
				m_rtcLatched = m_rtc;
			}
			m_latch = value;
		}
		else if (address >= 0xA000 && address < 0xC000)
		{
			if (!m_ramEnabled) return;
			if (m_ramBank <= 0x03)
			{
				if (!m_ram.empty())
				{
					m_ram[ramOffset(address)] = value;
					markDirty();
				}
			}
			else if (m_hasTimer)
				writeRtc(value);
		}
	}

	/**
	 * Serialises RAM followed by the RTC block (timer carts only) so the clock
	 * survives across sessions. Advances the clock first so the saved base
	 * timestamp and registers are current.
	 * @return RAM bytes, optionally followed by the 48-byte RTC block.
	 */
	std::vector<uint8_t> serializeSave() override
	{
		std::vector<uint8_t> out = m_ram;
		if (m_hasTimer)
		{
			syncRtc();
			appendU32(out, m_rtc.s);        appendU32(out, m_rtc.m);
			appendU32(out, m_rtc.h);        appendU32(out, m_rtc.dl);
			appendU32(out, m_rtc.dh);
			appendU32(out, m_rtcLatched.s); appendU32(out, m_rtcLatched.m);
			appendU32(out, m_rtcLatched.h); appendU32(out, m_rtcLatched.dl);
			appendU32(out, m_rtcLatched.dh);
			appendU64(out, static_cast<uint64_t>(m_baseTime));
		}
		return out;
	}

	/**
	 * Restores RAM and, when present, the RTC block, then advances the clock by
	 * the real time that elapsed while the game was closed.
	 * @param data Raw .sav bytes: RAM, optionally followed by the 48-byte RTC block.
	 */
	void deserializeSave(const std::vector<uint8_t>& data) override
	{
		const std::size_t ramN = std::min(data.size(), m_ram.size());
		std::copy_n(data.begin(), ramN, m_ram.begin());

		// The RTC block (10 x uint32 + uint64 = 48 bytes) follows the full RAM
		// image; anything shorter is a RAM-only save from a non-timer build.
		const std::size_t rtcAt = m_ram.size();
		if (m_hasTimer && data.size() >= rtcAt + 48)
		{
			std::size_t p = rtcAt;
			m_rtc.s  = readU32(data, p); m_rtc.m  = readU32(data, p);
			m_rtc.h  = readU32(data, p); m_rtc.dl = readU32(data, p);
			m_rtc.dh = readU32(data, p);
			m_rtcLatched.s  = readU32(data, p); m_rtcLatched.m  = readU32(data, p);
			m_rtcLatched.h  = readU32(data, p); m_rtcLatched.dl = readU32(data, p);
			m_rtcLatched.dh = readU32(data, p);
			m_baseTime = static_cast<int64_t>(readU64(data, p));
			syncRtc();   // account for time passed since the save was written
		}
	}

private:
	struct rtc { uint8_t s = 0, m = 0, h = 0, dl = 0, dh = 0; };

	/** @return Current wall-clock time in whole seconds since the Unix epoch. */
	static int64_t nowSeconds()
	{
		using namespace std::chrono;
		return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
	}

	/**
	 * Advances the live RTC by the real seconds elapsed since the last sync,
	 * unless the halt bit is set (in which case the clock is frozen). Rebases the
	 * timestamp so each call only folds in newly elapsed time.
	 */
	void syncRtc()
	{
		if (!m_hasTimer) return;
		const int64_t now = nowSeconds();
		// Halt bit (day-high bit 6) stops the clock; keep the base current so it
		// resumes cleanly when cleared, and never let a backwards clock rewind.
		if ((m_rtc.dh & 0x40) || now <= m_baseTime)
		{
			m_baseTime = now;
			return;
		}
		advance(now - m_baseTime);
		m_baseTime = now;
	}

	/**
	 * Folds a span of elapsed seconds into the live seconds/minutes/hours/day
	 * registers, setting the day-carry bit (day-high bit 7) on overflow past 511.
	 * @param secs Seconds to add to the running clock.
	 */
	void advance(int64_t secs)
	{
		int64_t s = m_rtc.s + secs;
		m_rtc.s = static_cast<uint8_t>(s % 60);
		int64_t mins = m_rtc.m + s / 60;
		m_rtc.m = static_cast<uint8_t>(mins % 60);
		int64_t hrs = m_rtc.h + mins / 60;
		m_rtc.h = static_cast<uint8_t>(hrs % 24);
		int64_t days = (((m_rtc.dh & 0x01) << 8) | m_rtc.dl) + hrs / 24;
		m_rtc.dl = static_cast<uint8_t>(days & 0xFF);
		m_rtc.dh = static_cast<uint8_t>((m_rtc.dh & 0xFE) | ((days >> 8) & 0x01));
		if (days > 0x1FF)
			m_rtc.dh |= 0x80;   // day-counter carry, latched until the game clears it
		markDirty();
	}

	/**
	 * Maps a RAM-window address to a byte offset in the selected RAM bank.
	 * @param address Raw CPU address in 0xA000-0xBFFF.
	 * @return Wrapped offset into the RAM buffer.
	 */
	uint32_t ramOffset(uint16_t address) const
	{
		return (m_ramBank * 0x2000u + (address - 0xA000u)) & (m_ram.size() - 1);
	}

	/**
	 * Reads the latched RTC register selected by the current RAM-bank value.
	 * @return The latched seconds/minutes/hours/day-low/day-high register, or 0xFF if none is selected.
	 */
	uint8_t readRtc() const
	{
		switch (m_ramBank)
		{
			case 0x08: return m_rtcLatched.s;
			case 0x09: return m_rtcLatched.m;
			case 0x0A: return m_rtcLatched.h;
			case 0x0B: return m_rtcLatched.dl;
			case 0x0C: return m_rtcLatched.dh;
			default:   return 0xFF;
		}
	}

	/**
	 * Writes the live RTC register selected by the current RAM-bank value and
	 * rebases the clock, so setting the time does not immediately jump.
	 * @param value Byte stored into the selected seconds/minutes/hours/day-low/day-high register.
	 */
	void writeRtc(uint8_t value)
	{
		switch (m_ramBank)
		{
			case 0x08: m_rtc.s = value; break;
			case 0x09: m_rtc.m = value; break;
			case 0x0A: m_rtc.h = value; break;
			case 0x0B: m_rtc.dl = value; break;
			case 0x0C: m_rtc.dh = value; break;
			default: return;
		}
		m_baseTime = nowSeconds();
		markDirty();
	}

	/** Appends a little-endian uint32 to a byte buffer. */
	static void appendU32(std::vector<uint8_t>& v, uint32_t x)
	{
		v.push_back(static_cast<uint8_t>(x));
		v.push_back(static_cast<uint8_t>(x >> 8));
		v.push_back(static_cast<uint8_t>(x >> 16));
		v.push_back(static_cast<uint8_t>(x >> 24));
	}

	/** Appends a little-endian uint64 to a byte buffer. */
	static void appendU64(std::vector<uint8_t>& v, uint64_t x)
	{
		for (int i = 0; i < 8; ++i)
			v.push_back(static_cast<uint8_t>(x >> (8 * i)));
	}

	/** Reads a little-endian uint32 at offset p, advancing p by 4. */
	static uint32_t readU32(const std::vector<uint8_t>& v, std::size_t& p)
	{
		const uint32_t x = static_cast<uint32_t>(v[p]) |
		                   (static_cast<uint32_t>(v[p + 1]) << 8) |
		                   (static_cast<uint32_t>(v[p + 2]) << 16) |
		                   (static_cast<uint32_t>(v[p + 3]) << 24);
		p += 4;
		return x;
	}

	/** Reads a little-endian uint64 at offset p, advancing p by 8. */
	static uint64_t readU64(const std::vector<uint8_t>& v, std::size_t& p)
	{
		uint64_t x = 0;
		for (int i = 0; i < 8; ++i)
			x |= static_cast<uint64_t>(v[p + i]) << (8 * i);
		p += 8;
		return x;
	}

	bool m_ramEnabled = false;
	bool m_hasTimer = false;
	uint8_t m_romBank = 1;
	uint8_t m_ramBank = 0;
	uint8_t m_latch = 0xFF;
	int64_t m_baseTime = 0;   // wall-clock seconds at the last RTC sync
	rtc m_rtc;
	rtc m_rtcLatched;
};

// -----------------------------------------------------------------------------
// MBC5: up to 8 MiB ROM / 128 KiB RAM (cart types 0x19-0x1E, incl. rumble).
// Unlike MBC1/3, bank 0 *is* selectable in the high ROM window.
// -----------------------------------------------------------------------------
class mbc5 : public mbc
{
public:
	using mbc::mbc;

	/**
	 * Reads the MBC5 ROM/RAM windows. Low ROM is bank 0; the high window selects
	 * the 9-bit m_romBank (bank 0 is selectable here, unlike MBC1/3); RAM
	 * (0xA000-0xBFFF) reads the enabled RAM bank.
	 * @param address Raw CPU address in the ROM or RAM window.
	 * @return The mapped byte, or 0xFF when RAM is disabled/absent or unmapped.
	 */
	uint8_t read(uint16_t address) override
	{
		if (address < 0x4000)
			return romByte(0, address);
		if (address < 0x8000)
			return romByte(m_romBank & (m_romBanks - 1), address - 0x4000);
		if (address >= 0xA000 && address < 0xC000)
		{
			if (!m_ramEnabled || m_ram.empty()) return 0xFF;
			return m_ram[ramOffset(address)];
		}
		return 0xFF;
	}

	/**
	 * Writes an MBC5 control register or external RAM. 0x0000-0x1FFF enables RAM,
	 * 0x2000-0x2FFF sets the low 8 ROM-bank bits, 0x3000-0x3FFF sets ROM-bank
	 * bit 8, 0x4000-0x5FFF selects the 4-bit RAM bank (bit 3 also drives rumble),
	 * and 0xA000-0xBFFF writes RAM when enabled.
	 * @param address Raw CPU address selecting the register or RAM byte.
	 * @param value Byte written.
	 */
	void write(uint16_t address, uint8_t value) override
	{
		if (address < 0x2000)
			m_ramEnabled = (value & 0x0F) == 0x0A;
		else if (address < 0x3000)
			m_romBank = (m_romBank & 0x100) | value;                  // low 8 bits
		else if (address < 0x4000)
			m_romBank = (m_romBank & 0x0FF) | ((value & 0x01) << 8);  // bit 8
		else if (address < 0x6000)
			m_ramBank = value & 0x0F;   // bit 3 doubles as rumble on rumble carts
		else if (address >= 0xA000 && address < 0xC000)
		{
			if (m_ramEnabled && !m_ram.empty())
			{
				m_ram[ramOffset(address)] = value;
				markDirty();
			}
		}
	}

private:
	/**
	 * Maps a RAM-window address to a byte offset in the selected RAM bank.
	 * @param address Raw CPU address in 0xA000-0xBFFF.
	 * @return Wrapped offset into the RAM buffer.
	 */
	uint32_t ramOffset(uint16_t address) const
	{
		return (m_ramBank * 0x2000u + (address - 0xA000u)) & (m_ram.size() - 1);
	}

	bool m_ramEnabled = false;
	uint16_t m_romBank = 1;   // 9 bits wide
	uint8_t m_ramBank = 0;
};

/**
 * Whether a cartridge type carries a battery, i.e. its save RAM (and RTC, if
 * any) should persist to a .sav file between sessions.
 * @param cartType Cartridge type from header byte 0x0147.
 * @return True for the battery-backed cartridge types.
 */
inline bool cartHasBattery(uint8_t cartType)
{
	switch (cartType)
	{
		case 0x03:   // MBC1+RAM+BATTERY
		case 0x06:   // MBC2+BATTERY
		case 0x09:   // ROM+RAM+BATTERY
		case 0x0D:   // MMM01+RAM+BATTERY
		case 0x0F:   // MBC3+TIMER+BATTERY
		case 0x10:   // MBC3+TIMER+RAM+BATTERY
		case 0x13:   // MBC3+RAM+BATTERY
		case 0x1B:   // MBC5+RAM+BATTERY
		case 0x1E:   // MBC5+RUMBLE+RAM+BATTERY
		case 0x22:   // MBC7+SENSOR+RUMBLE+RAM+BATTERY
			return true;
		default:
			return false;
	}
}

/**
 * Builds the MBC mapper named by the cartridge header, translating the RAM-size
 * code to a byte count and dispatching on the cartridge type. The battery flag
 * (and MBC3 timer flag) are set from the type so the loader knows what to
 * persist. Unknown or not-yet-modelled types fall back to the flat no-mapper so
 * the header and bank 0 still read correctly.
 * @param cartType Cartridge type from header byte 0x0147.
 * @param rom Pointer to the full ROM image.
 * @param romSize ROM image length in bytes.
 * @param ramSizeCode RAM-size code from header byte 0x0149.
 * @return A mapper instance owning the banking logic for this cartridge.
 */
inline std::unique_ptr<mbc> makeMbc(uint8_t cartType, const uint8_t* rom,
                                    uint32_t romSize, uint8_t ramSizeCode)
{
	static const uint32_t RAM_BYTES[] =
	{
		0,            // 0x00: none
		2u * 1024,    // 0x01: 2 KiB   (unofficial)
		8u * 1024,    // 0x02: 8 KiB   (1 bank)
		32u * 1024,   // 0x03: 32 KiB  (4 banks)
		128u * 1024,  // 0x04: 128 KiB (16 banks)
		64u * 1024,   // 0x05: 64 KiB  (8 banks)
	};
	const uint32_t ramBytes = (ramSizeCode < 6) ? RAM_BYTES[ramSizeCode] : 0;

	std::unique_ptr<mbc> m;
	switch (cartType)
	{
		case 0x01: case 0x02: case 0x03:
			m = std::make_unique<mbc1>(rom, romSize, ramBytes);
			break;

		case 0x05: case 0x06:
			m = std::make_unique<mbc2>(rom, romSize, 0);
			break;

		case 0x0F: case 0x10: case 0x11: case 0x12: case 0x13:
		{
			auto p = std::make_unique<mbc3>(rom, romSize, ramBytes);
			p->setTimer(cartType == 0x0F || cartType == 0x10);   // TIMER variants only
			m = std::move(p);
			break;
		}

		case 0x19: case 0x1A: case 0x1B:
		case 0x1C: case 0x1D: case 0x1E:
			m = std::make_unique<mbc5>(rom, romSize, ramBytes);
			break;

		case 0x00: case 0x08: case 0x09:
		default:
			// ROM-only, plus a safe fallback for mappers not yet modelled so the
			// header and bank 0 still read correctly.
			m = std::make_unique<mbcNone>(rom, romSize, ramBytes);
			break;
	}

	m->setBattery(cartHasBattery(cartType));
	return m;
}

#endif //GAMEBOYEMULATOR_MBC_H
