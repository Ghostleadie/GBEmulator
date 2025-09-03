//
// Created by Jack_ on 31/08/2025.
//

#include "cartridgeLoader.h"

#include <fstream>

bool cartridgeLoader::loadCartridge(std::string cartridgePath)
{
	LOG_INFO("rom path: {}",cartridgePath);
	std::ifstream cartridgeFile;
	cartridgeFile.open(cartridgePath, std::ifstream::binary);

	if (!cartridgeFile) {
		LOG_WARN("Failed to open: {}", cartridgePath);
		return false;
	}

	cartridgeFile.seekg(0, std::ifstream::end);
	ctx.romSize = (unsigned long)cartridgeFile.tellg();
	cartridgeFile.seekg(0, std::ifstream::beg);

	ctx.romData = new char[ctx.romSize];
	cartridgeFile.read(ctx.romData, ctx.romSize);
	cartridgeFile.close();

	ctx.header = (cartirdge*)(ctx.romData + 0x100);
	ctx.header->title[15] = 0;

	LOG_INFO("Cartridge Loaded:\n");
	LOG_INFO("\t Title    : " + std::string(ctx.header->title));
	LOG_INFO("\t Type     : " + std::to_string(ctx.header->type) + " " + ROM_TYPES[ctx.header->type]);
	LOG_INFO("\t ROM Size : " + std::to_string(32 << ctx.header->romSize) + "KB");
	LOG_INFO("\t RAM Size : " + std::to_string(ctx.header->ramSize));
	LOG_INFO("\t LIC Code : " + std::to_string(ctx.header->licCode) + "(" + getLicenseeName(ctx.header->licCode) + ")");
	LOG_INFO("\t ROM Vers : " + std::to_string(ctx.header->version));

	uint16_t x = 0;
	for (uint16_t i = 0x0134; i <= 0x014C; i++) {
		x = x - ctx.romData[i] - 1;
	}

	LOG_INFO("\t Checksum : " + std::to_string(ctx.header->checksum) + "(" + ((x & 0xFF) ? "PASSED" : "FAILED") + ")");

	return true;
}

uint8_t cartridgeLoader::read(uint16_t address)
{
	return ctx.romData[address];
}

void cartridgeLoader::write(uint16_t address, uint8_t value)
{

}


std::string cartridgeLoader::getLicenseeName(const int code) const
{
	if (auto it = licenseeNames.find(code); it != licenseeNames.end()) {
		return it->second;
	}
	else {
		return "UNKNOWN";
	}
}