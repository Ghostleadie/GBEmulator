#pragma once

#include "../cpu/instructions.h"
namespace utility {
	//general
	inline std::string uint8ToHex(const uint8_t i) 
	{
		return std::format("{:02X}", static_cast<int>(i));
	};

	//cpu
	inline bool inRange(uint16_t address, uint16_t low, uint16_t high)
	{
		return low <= address && address <= high;
	};

	inline bool isReg16Bit(registryType rt)
	{
		return rt >= RT_AF;
	};
	inline uint8_t setBit(const uint8_t value, const uint8_t bit)
	{
		auto value_set = value | (1 << bit);
		return static_cast<uint8_t>(value_set);
	};
	inline uint8_t clearBit(const uint8_t value, const uint8_t bit)
	{
		auto value_cleared = value & ~(1 << bit);
		return static_cast<uint8_t>(value_cleared);
	};
	inline uint8_t setBitTo(const uint8_t value, const uint8_t bit, bool bit_on)
	{
		return bit_on ? utility::setBit(value, bit) : utility::clearBit(value, bit);
	};
	inline bool checkBit(const uint8_t value, const uint8_t bit)
	{
		return (value & (1 << bit)) != 0;
	};
}