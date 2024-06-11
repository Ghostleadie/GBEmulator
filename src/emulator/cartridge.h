#pragma once
#include <array>
#include <iostream>
#include <fstream>

struct cartirdge
{
	uint8_t entry[4];
	uint8_t logo[0x30];

	char title[16];
	uint16_t newLicCode;
	uint8_t sgb_flag;
	uint8_t type;
	uint8_t romSize;
	uint8_t ramSize;
	uint8_t destCode;
	uint8_t licCode;
	uint8_t version;
	uint8_t checksum;
	uint16_t globalChecksum;
};

struct cartridgeContext
{
    char filename[1024];
    uint32_t romSize;
    char* romData;
    cartirdge* header;
};

class cartridgeLoader
{
public:
    /*
    /
    */
	bool loadCartridge(char* cartridge);

    /*
    /
    */
	std::string getLicenseeName(int code);
private:

    cartridgeContext ctx;

    const std::array<std::string, 35> ROM_TYPES = {
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
};