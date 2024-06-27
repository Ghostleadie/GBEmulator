#pragma once

#include <sstream>
#include <iostream>
#include <iomanip>

class utility
{
public:
	static std::string uint8ToHex(const uint8_t i);
	static bool inRange(uint16_t address, uint16_t low, uint16_t high);
};