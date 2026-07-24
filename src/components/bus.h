//
// Created by Jack_ on 31/08/2025.
//

#ifndef GAMEBOYEMULATOR_BUS_H
#define GAMEBOYEMULATOR_BUS_H
#include "../interfaces/IComponentMessanger.h"


class cpu;
class timer;
class ppu;
class joypad;
class cartridgeLoader;
class interruptController;

class bus :   public IComponentMessanger
{
public:
	bus()= default;
	/**
	 * Wires up the peripherals the bus dispatches to and hands the PPU a back-pointer to the bus.
	 * @param loader Cartridge/MBC backing ROM, external RAM and the 0x0000-0x00FF region.
	 * @param joypad P1/JOYP input device (0xFF00).
	 * @param ppu Pixel unit owning VRAM, OAM and the 0xFF40-0xFF4B LCD registers.
	 * @param timer DIV/TIMA/TMA/TAC timer (0xFF04-0xFF07).
	 * @param cpu CPU, held weakly and used only by the off-by-default serial debug trap.
	 * @param interruptController IF/IE owner (0xFF0F / 0xFFFF).
	 * All stored pointers are non-owning: the emulator owns every component and outlives the bus.
	 */
	void connectComponents(const std::shared_ptr<cartridgeLoader>& loader, const std::shared_ptr<joypad>& joypad, const std::shared_ptr<ppu>& ppu, const std::shared_ptr<timer>& timer, const std::shared_ptr<cpu>& cpu, const std::shared_ptr<interruptController>& interruptController);

	/**
	 * Attaches the APU that backs the sound registers (0xFF10-0xFF3F).
	 * Kept separate from connectComponents so the SDL-free headless targets
	 * (romrunner/tests), which build no APU, leave it null and get the old
	 * "unmapped audio reads 0xFF" behaviour. Typed as the device interface so
	 * bus.cpp never needs the SDL-carrying apu header.
	 * @param audio Non-owning APU pointer, or null for no audio; the emulator owns it and outlives the bus.
	 */
	void connectAudio(IComponentMessanger* audio) { m_apu = audio; }

	/**
	 * Reads a byte by decoding the 16-bit address to its memory-map region and dispatching to the owning device.
	 * @param address 16-bit CPU address.
	 * @return Byte at the address, or 0xFF when no device backs the region.
	 * Routes ROM/cartridge RAM to the cartridge, 0x8000-0x9FFF VRAM and 0xFE00-0xFE9F OAM to the PPU, 0xC000-0xDFFF WRAM
	 * (mirrored by 0xE000-0xFDFF echo RAM), 0xFF00-0xFF7F I/O, 0xFF80-0xFFFE HRAM, and 0xFFFF to the interrupt-enable register.
	 */
	uint8_t read(uint16_t address) override;
	/**
	 * Writes a byte by decoding the 16-bit address to its memory-map region and dispatching to the owning device.
	 * @param address 16-bit CPU address.
	 * @param value Byte to store.
	 * Passively captures every 0xFF01 serial byte for the debug panel; OAM writes are dropped while a DMA transfer is in progress.
	 */
	void write(uint16_t address, uint8_t value) override;

	/**
	 * Reads a byte from the 8 KiB work RAM (0xC000-0xDFFF), indexing from the 0xC000 base.
	 * @param address Global address in the WRAM range.
	 * @return Byte stored in WRAM.
	 * Logs and aborts the process if the offset falls outside the 0x2000-byte region.
	 */
	uint8_t readWRam(uint16_t address);
	/**
	 * Writes a byte to the 8 KiB work RAM (0xC000-0xDFFF), indexing from the 0xC000 base.
	 * @param address Global address in the WRAM range.
	 * @param value Byte to store.
	 * Logs and aborts the process if the offset falls outside the 0x2000-byte region.
	 */
	void writeWRam(uint16_t address, uint8_t value);

	/**
	 * Reads a byte from high RAM (0xFF80-0xFFFE), indexing from the 0xFF80 base.
	 * @param address Global address in the HRAM range.
	 * @return Byte stored in HRAM.
	 */
	uint8_t readHRam(uint16_t address);
	/**
	 * Writes a byte to high RAM (0xFF80-0xFFFE), indexing from the 0xFF80 base.
	 * @param address Global address in the HRAM range.
	 * @param value Byte to store.
	 */
	void writeHRam(uint16_t address, uint8_t value);

	/**
	 * Reads an I/O register (0xFF00-0xFF7F) by dispatching to the device that backs it.
	 * @param address Global I/O address.
	 * @return Register value, or 0xFF for unmapped I/O.
	 * Maps 0xFF00 joypad, 0xFF01/0xFF02 serial, 0xFF04-0xFF07 timer, 0xFF0F IF, and 0xFF40-0xFF4B PPU/LCD.
	 */
	uint8_t readIO(uint16_t address);
	/**
	 * Writes an I/O register (0xFF00-0xFF7F) by dispatching to the device that backs it.
	 * @param address Global I/O address.
	 * @param value Byte to store.
	 * Maps 0xFF00 joypad, 0xFF01/0xFF02 serial, 0xFF04-0xFF07 timer, 0xFF0F IF, and 0xFF40-0xFF4B PPU/LCD; unmapped writes are ignored.
	 */
	void writeIO(uint16_t address, uint8_t value);

	/**
	 * Enables or disables bus-side serial logging ("Serial:" lines plus the Failed-detection trap).
	 * @param enabled True to turn per-line serial logging on.
	 * Off by default; the CPU's SerialPortDebugger handles the SERIAL:/RESULT: logs.
	 */
	void setSerialLogging(bool enabled) { m_serialLogging = enabled; }
	/** Returns whether bus-side serial logging is enabled. */
	bool getSerialLogging() const { return m_serialLogging; }

	/** Returns the complete serial output, captured passively for the debug panel. */
	const char* getSerialOutput() const { return m_serialOutput.data(); }
	/** Clears the captured serial-output buffer. */
	void clearSerialOutput() { m_serialOutput[0] = '\0'; m_serialOutputLen = 0; }

private:
	// Non-owning: the emulator is the sole owner of every component and
	// outlives the bus. The bus only uses these, it does not keep them alive.
	cartridgeLoader* m_cartridge = nullptr;
	joypad* m_joypad = nullptr;
	ppu* m_ppu = nullptr;
	timer* m_timer = nullptr;
	interruptController* m_interruptController = nullptr;
	IComponentMessanger* m_apu = nullptr;  // sound registers 0xFF10-0xFF3F; null in headless builds
	std::weak_ptr<cpu> m_cpu;   // weak: only used by the off-by-default serial debug trap

	std::array<uint8_t, 0x2000> wRam{};
	std::array<uint8_t, 0x80> hRam{};

	std::array<char,2> serialData = {};
	std::string m_serialBuffer;
	bool m_serialLogging = false;

	std::array<char, 8192> m_serialOutput = {0};
	int  m_serialOutputLen = 0;
};


#endif //GAMEBOYEMULATOR_BUS_H