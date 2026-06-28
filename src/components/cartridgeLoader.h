//
// Created by Jack_ on 31/08/2025.
//

#ifndef GAMEBOYEMULATOR_CARTRIDGELOADER_H
#define GAMEBOYEMULATOR_CARTRIDGELOADER_H
#include "../interfaces/ICartridgeDebug.h"
#include "../interfaces/IComponentMessanger.h"
#include <array>
#include <unordered_map>


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

class cartridgeLoader : public memoryComponentMessanger, public ICartridgeDebug
{
public:
	cartridgeLoader() = default;
	~cartridgeLoader() override = default;
	/*
	/
	*/
	bool loadCartridge(const std::string cartridge);

	// Read a byte from a device-local address (offset from base).
	uint8_t read(uint16_t address) override;

	// Write a byte to a device-local address (offset from base).
	void write(uint16_t address, uint8_t value) override;

	/*
	/
	*/
	std::string getLicenseeName(int code) const;

	const cartridgeContext& peekCartridgeContext() const override {return ctx;}
private:

	cartridgeContext ctx;

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