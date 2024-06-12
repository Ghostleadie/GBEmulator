#include "bus.h"
#include "emulator.h"

bus::bus(std::shared_ptr <cartridgeLoader> loader)
{
	m_loader = loader;
}

uint8_t bus::busRead(uint16_t address)
{
	if (address < 0x8000)
	{
		//ROM Data
		return m_loader->readCartridge(address);
	}
	return 0;
}

void bus::busWrite(uint16_t address, uint8_t value)
{
	if (address < 0x8000)
	{
		//ROM Data
		m_loader->writeToCartridge(address, value);
		return;
	}
}
