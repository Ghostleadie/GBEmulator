//
// Created by Jack_ on 31/08/2025.
//

#include "cartridgeLoader.h"
#include "mbc.h"

#include <fstream>
#include <cstdio>
#include <iterator>
#include <vector>

cartridgeLoader::cartridgeLoader() = default;

// Flush the battery one last time so a quit mid-session is not lost. Safe when
// no battery cart is loaded (saveBattery() no-ops).
cartridgeLoader::~cartridgeLoader()
{
	saveBattery();
}

bool cartridgeLoader::loadCartridge(const std::string cartridgePath)
{
	CARTRIDGE_INFO("rom path: {}",cartridgePath);
	std::ifstream cartridgeFile;
	cartridgeFile.open(cartridgePath, std::ifstream::binary);

	if (!cartridgeFile) {
		LOG_WARN("Failed to open: {}", cartridgePath);
		return false;
	}

	std::snprintf(ctx.filename, sizeof(ctx.filename), "%s", cartridgePath.c_str());

	cartridgeFile.seekg(0, std::ifstream::end);
	ctx.romSize = static_cast<unsigned long>(cartridgeFile.tellg());
	cartridgeFile.seekg(0, std::ifstream::beg);

	// Reloading: persist the outgoing cart's battery (still using its .sav path),
	// then drop the old mapper (it points at the previous buffer) and free that
	// buffer before allocating the new one.
	saveBattery();
	m_mbc.reset();
	delete[] ctx.romData;

	ctx.romData = new char[ctx.romSize];
	cartridgeFile.read(ctx.romData, ctx.romSize);
	cartridgeFile.close();

	ctx.header = reinterpret_cast<cartirdge*>(ctx.romData + 0x100);
	ctx.header->title[15] = 0;

	CARTRIDGE_INFO("Cartridge Loaded:\n");
	CARTRIDGE_INFO("\t Title    : " + std::string(ctx.header->title));
	CARTRIDGE_INFO("\t Type     : " + std::to_string(ctx.header->type) + " " + ROM_TYPES[ctx.header->type]);
	CARTRIDGE_INFO("\t ROM Size : " + std::to_string(32 << ctx.header->romSize) + "KB");
	CARTRIDGE_INFO("\t RAM Size : " + std::to_string(ctx.header->ramSize));
	CARTRIDGE_INFO("\t LIC Code : " + std::to_string(ctx.header->licCode) + "(" + getLicenseeName(ctx.header->licCode) + ")");
	CARTRIDGE_INFO("\t ROM Vers : " + std::to_string(ctx.header->version));

	uint16_t x = 0;
	for (uint16_t i = 0x0134; i <= 0x014C; i++) {
		x = x - ctx.romData[i] - 1;
	}

	CARTRIDGE_INFO("\t Checksum : " + std::to_string(ctx.header->checksum) + "(" + ((x & 0xFF) ? "PASSED" : "FAILED") + ")");

	// Pick the mapper the header asks for; all bus ROM/RAM traffic runs through it.
	m_mbc = makeMbc(ctx.header->type,
	                reinterpret_cast<const uint8_t*>(ctx.romData),
	                ctx.romSize,
	                ctx.header->ramSize);

	// Restore this cart's battery-backed RAM/RTC from its sibling .sav, if any.
	m_savePath = deriveSavePath(cartridgePath);
	m_lastSave = std::chrono::steady_clock::now();
	loadBattery();

	return true;
}

uint8_t cartridgeLoader::read(uint16_t address)
{
	if (m_mbc)
		return m_mbc->read(address);
	return 0xFF;
}

void cartridgeLoader::write(uint16_t address, uint8_t value)
{
	if (m_mbc)
		m_mbc->write(address, value);
}

std::string cartridgeLoader::deriveSavePath(const std::string& romPath)
{
	// Swap the ROM's extension for ".sav", keeping it a sibling file. Only treat
	// a dot as an extension when it sits after the last path separator.
	const std::size_t slash = romPath.find_last_of("/\\");
	const std::size_t dot = romPath.find_last_of('.');
	if (dot != std::string::npos && (slash == std::string::npos || dot > slash))
		return romPath.substr(0, dot) + ".sav";
	return romPath + ".sav";
}

void cartridgeLoader::loadBattery()
{
	if (!m_mbc || !m_mbc->hasBattery() || m_savePath.empty())
		return;

	std::ifstream f(m_savePath, std::ifstream::binary);
	if (!f)
		return;   // no save yet: fresh cartridge, nothing to restore

	const std::vector<uint8_t> data((std::istreambuf_iterator<char>(f)),
	                                 std::istreambuf_iterator<char>());
	if (!data.empty())
	{
		m_mbc->deserializeSave(data);
		CARTRIDGE_INFO("Battery save loaded: " + m_savePath + " (" + std::to_string(data.size()) + " bytes)");
	}
}

void cartridgeLoader::saveBattery()
{
	if (!m_mbc || !m_mbc->hasBattery() || m_savePath.empty())
		return;

	const std::vector<uint8_t> data = m_mbc->serializeSave();
	if (data.empty())
		return;

	std::ofstream f(m_savePath, std::ofstream::binary | std::ofstream::trunc);
	if (!f)
	{
		LOG_WARN("Failed to write battery save: {}", m_savePath);
		return;
	}
	f.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));

	m_mbc->clearSaveDirty();
	m_lastSave = std::chrono::steady_clock::now();
}

void cartridgeLoader::saveBatteryIfDirty()
{
	if (!m_mbc || !m_mbc->hasBattery() || !m_mbc->saveDirty())
		return;

	// Throttle: at most one flush every couple of seconds, so a game that writes
	// save RAM constantly does not thrash the disk.
	using namespace std::chrono;
	if (steady_clock::now() - m_lastSave < seconds(2))
		return;

	saveBattery();
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