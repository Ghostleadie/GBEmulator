//
// Created by Jack_ on 31/08/2025.
//

#include "joypad.h"

uint8_t joypad::read(uint16_t address)
{
	// No buttons pressed / nothing selected — open lines read high.
	return 0xFF;
}

void joypad::write(uint16_t address, uint8_t value)
{

}