//
// Created by Jack_ on 31/08/2025.
//

#include "ppu.h"

uint8_t ppu::read(uint16_t address)
{
	if (address >= 0x8000 && address <= 0x9FFF)
	{
		return vram[address - 0x8000];
	}
	if (address >= 0xFE00 && address <= 0xFE9F)
	{
		return reinterpret_cast<const uint8_t*>(oamRAM.data())[address - 0xFE00];
	}
	return 0xFF;
}

void ppu::write(uint16_t address, uint8_t value)
{
	if (address >= 0x8000 && address <= 0x9FFF)
	{
		vram[address - 0x8000] = value;
		return;
	}
	if (address >= 0xFE00 && address <= 0xFE9F)
	{
		reinterpret_cast<uint8_t*>(oamRAM.data())[address - 0xFE00] = value;
		return;
	}
}
