//
// Created by Jack_ on 31/08/2025.
//

#ifndef GAMEBOYEMULATOR_PPU_H
#define GAMEBOYEMULATOR_PPU_H
#include "base/component.h"

class ppu : public memoryComponent
{
public:
	// Read a byte from a device-local address (offset from base).
	uint8_t read(uint16_t address);

	// Write a byte to a device-local address (offset from base).
	void write(uint16_t address, uint8_t value);
};


#endif //GAMEBOYEMULATOR_PPU_H