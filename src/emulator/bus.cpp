#include "bus.h"
#include <type_traits>

bus::bus(std::shared_ptr <cartridgeLoader> loader)
{
	m_loader = loader;
}

uint8_t bus::read8bit(uint16_t address)
{
	if (address < 0x8000)
	{
		//ROM Data
		return m_loader->readCartridge(address);
	}
	return 0;
}

uint16_t bus::read16bit(uint16_t address)
{
	uint16_t lo = read8bit(address);
	uint16_t hi = read8bit(address + 1);

	return lo | (hi << 8);

}

void bus::write(uint16_t address, uint8_t value)
{
	if (address < 0x8000)
	{
		//ROM Data
		m_loader->writeToCartridge(address, value);
		return;
	}
}

void bus::write(uint16_t address, uint16_t value)
{
	write(uint16_t(address + 1), uint8_t((value >> 8) & 0xFF));
	write(uint16_t(address), uint8_t(value & 0xFF));
}
