#pragma once
#include "../emulator/instructions.h"

#include <sstream>
#include <iostream>
#include <iomanip>

class utility
{
public:
	//general
	static std::string uint8ToHex(const uint8_t i);
	//cpu
	static bool inRange(uint16_t address, uint16_t low, uint16_t high);
	static bool isReg16Bit(registryType rt);
	static uint8_t setBit(const uint8_t value, const uint8_t bit);
	static uint8_t clearBit(const uint8_t value, const uint8_t bit);
	static uint8_t setBitTo(const uint8_t value, const uint8_t bit, bool bit_on);
	static bool checkBit(const uint8_t value, const uint8_t bit);
};