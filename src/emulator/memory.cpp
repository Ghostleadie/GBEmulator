#include "memory.h"

uint8_t memory::read(uint16_t address)
{
	if (utility::inRange(address, 0xC000, 0xCFFF))
	{
		return wRAM.at(address - 0xC000);
	}
	else if (utility::inRange(address, 0xFF80, 0xFFFE))
	{
		return hRAM.at(address - 0xFF80);
	}
	else
	{
		GBE_ERROR("invalid memory read");
		return 0;
	}
	
}

void memory::write(uint16_t address, uint8_t value)
{
	if (utility::inRange(address, 0xC000, 0xCFFF))
	{
		wRAM.at(address - 0xC000) = value;
		return;
	}
	else if (utility::inRange(address, 0xFF80, 0xFFFE))
	{
		hRAM.at(address - 0xFF80) = value;
		return;
	}
	else
	{
		GBE_ERROR("invalid memory read");
		return;
	}
}


