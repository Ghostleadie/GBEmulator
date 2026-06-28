//
// Created by Jack_ on 31/08/2025.
//

#ifndef GAMEBOYEMULATOR_JOYPAD_H
#define GAMEBOYEMULATOR_JOYPAD_H
#include "../interfaces/IComponentMessanger.h"

class joypad : public memoryComponentMessanger
{
public:
	// Read a byte from a device-local address (offset from base).
	uint8_t read(uint16_t address) override;

	// Write a byte to a device-local address (offset from base).
	void write(uint16_t address, uint8_t value) override;
};


#endif //GAMEBOYEMULATOR_JOYPAD_H