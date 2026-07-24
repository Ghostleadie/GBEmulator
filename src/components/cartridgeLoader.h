//
// Created by Jack_ on 31/08/2025.
//

#ifndef GAMEBOYEMULATOR_CARTRIDGELOADER_H
#define GAMEBOYEMULATOR_CARTRIDGELOADER_H
#include "../interfaces/ICartridgeDebug.h"
#include "../interfaces/IComponentMessanger.h"
#include <array>
#include <chrono>
#include <memory>
#include <unordered_map>

class mbc;


struct cartirdge
{
	uint8_t entry[4];
	uint8_t logo[0x30];

	char title[16];
	uint16_t newLicCode = 0;
	uint8_t sgbFlag = 0;
	uint8_t type = 0;
	uint8_t romSize = 0;
	uint8_t ramSize = 0;
	uint8_t destCode = 0;
	uint8_t licCode = 0;
	uint8_t version = 0;
	uint8_t checksum = 0;
	uint16_t globalChecksum = 0;
};

struct cartridgeContext
{
	char filename[1024] = {0};
	uint32_t romSize = 0;
	char* romData = nullptr;
	cartirdge* header = nullptr;
};

class cartridgeLoader : public IComponentMessanger, public ICartridgeDebug
{
public:
	// Both defined out-of-line: m_mbc is a unique_ptr to an incomplete type in
	// this header, so any special member that might destroy it (ctor cleanup
	// path included) must be compiled where mbc is complete.
	cartridgeLoader();
	~cartridgeLoader() override;
	/**
	 * Loads a ROM file: reads it into memory, maps the cartridge header at 0x0100,
	 * logs the title/type/size/licensee, verifies the header checksum
	 * (0x0134-0x014C) and builds the MBC mapper named by the header type. Any
	 * previously loaded ROM and mapper are released first.
	 * @param cartridge Filesystem path to the ROM file.
	 * @return True if the file was opened and loaded; false if it could not be opened.
	 */
	bool loadCartridge(const std::string cartridge);

	/**
	 * Forwards a ROM/RAM read to the active MBC mapper.
	 * @param address Raw CPU address (ROM 0x0000-0x7FFF or external RAM 0xA000-0xBFFF).
	 * @return The mapped byte, or 0xFF when no cartridge is loaded.
	 */
	uint8_t read(uint16_t address) override;

	/**
	 * Forwards a ROM/RAM write to the active MBC mapper, driving bank switching
	 * and RAM writes. Does nothing when no cartridge is loaded.
	 * @param address Raw CPU address (ROM control register or external RAM).
	 * @param value Byte to write.
	 */
	void write(uint16_t address, uint8_t value) override;

	/**
	 * Writes the cartridge's battery-backed RAM (and RTC, for MBC3+timer) to its
	 * .sav file, unconditionally. No-op when the cart has no battery or nothing is
	 * loaded. Call on shutdown / unload; safe to call from the thread that owns
	 * the mapper.
	 */
	void saveBattery();

	/**
	 * Flushes the .sav only when save data has changed and a throttle interval has
	 * elapsed, so a busy game is not written to disk every frame. Intended to be
	 * called once per frame from the emulation thread.
	 */
	void saveBatteryIfDirty();

	/**
	 * Resolves an old-style licensee code to its publisher name.
	 * @param code Licensee code from header byte 0x014B.
	 * @return The publisher name, or "UNKNOWN" if the code is not recognised.
	 */
	std::string getLicenseeName(int code) const;

	/** @return The parsed cartridge context: header pointer, ROM data, filename and size. */
	const cartridgeContext& peekCartridgeContext() const override {return ctx;}
private:

	cartridgeContext ctx;

	// Active mapper. Built from the cartridge header on load; every ROM/RAM
	// access the bus forwards is dispatched through it.
	std::unique_ptr<mbc> m_mbc;

	/**
	 * Derives the .sav path for the given ROM path (same name, ".sav" extension).
	 * @param romPath Filesystem path to the ROM file.
	 * @return The sibling .sav path.
	 */
	static std::string deriveSavePath(const std::string& romPath);

	/** Loads the .sav file (if any) into the mapper's battery-backed state. */
	void loadBattery();

	// .sav file for the current ROM, and the last time it was flushed (for the
	// dirty-write throttle).
	std::string m_savePath;
	std::chrono::steady_clock::time_point m_lastSave{};

	const std::array<std::string, 35> ROM_TYPES =
		{
		"ROM ONLY",
		"MBC1",
		"MBC1+RAM",
		"MBC1+RAM+BATTERY",
		"0x04 ???",
		"MBC2",
		"MBC2+BATTERY",
		"0x07 ???",
		"ROM+RAM 1",
		"ROM+RAM+BATTERY 1",
		"0x0A ???",
		"MMM01",
		"MMM01+RAM",
		"MMM01+RAM+BATTERY",
		"0x0E ???",
		"MBC3+TIMER+BATTERY",
		"MBC3+TIMER+RAM+BATTERY 2",
		"MBC3",
		"MBC3+RAM 2",
		"MBC3+RAM+BATTERY 2",
		"0x14 ???",
		"0x15 ???",
		"0x16 ???",
		"0x17 ???",
		"0x18 ???",
		"MBC5",
		"MBC5+RAM",
		"MBC5+RAM+BATTERY",
		"MBC5+RUMBLE",
		"MBC5+RUMBLE+RAM",
		"MBC5+RUMBLE+RAM+BATTERY",
		"0x1F ???",
		"MBC6",
		"0x21 ???",
		"MBC7+SENSOR+RUMBLE+RAM+BATTERY",
		};

	const std::unordered_map<int, std::string> licenseeNames =
		{
		{0, "None"},
		{1, "Nintendo R&D1"},
		{8, "Capcom"},
		{13, "Electronic Arts"},
		{18, "Hudson Soft"},
		{19, "b-ai"},
		{20, "kss"},
		{22, "pow"},
		{24, "PCM Complete"},
		{25, "san-x"},
		{28, "Kemco Japan"},
		};
};


#endif //GAMEBOYEMULATOR_CARTRIDGELOADER_H