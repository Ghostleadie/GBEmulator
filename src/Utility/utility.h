//
// Created by Jack_ on 05/09/2025.
//

#ifndef GAMEBOYEMULATOR_UTILITY_H
#define GAMEBOYEMULATOR_UTILITY_H
#include <cstdint>
#include <string>


namespace utility {
	//general

	//cpu
	// Returns true if 'address' is within the inclusive range [low, high].
	inline bool inRange(uint16_t address, uint16_t low, uint16_t high)
	{
		return low <= address && address <= high;
	};

	// Sets the bit at position 'bit' in 'value' and returns the new value.
	inline uint8_t setBit(const uint8_t value, const uint8_t bit)
	{
		auto value_set = value | (1 << bit);
		return static_cast<uint8_t>(value_set);
	};

	// Clears the bit at position 'bit' in 'value' and returns the new value.
	inline uint8_t clearBit(const uint8_t value, const uint8_t bit)
	{
		auto value_cleared = value & ~(1 << bit);
		return static_cast<uint8_t>(value_cleared);
	};

	// Sets or clears the bit at position 'bit' in 'value' based on 'bit_on'.
	inline void setBitTo(uint8_t& value, const uint8_t& bit, bool bit_on)
	{
		if (bit_on) {
			value |= (1 << bit);
		} else {
			value &= ~(1 << bit);
		}
	};

	// Returns true if the bit at position 'bit' in 'value' is set.
	inline bool checkBit(const uint8_t value, const uint8_t bit)
	{
		return (value & (1 << bit)) != 0;
	};
}


#endif //GAMEBOYEMULATOR_UTILITY_H