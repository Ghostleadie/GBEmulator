//
// Created by Jack_ on 05/09/2025.
//

#ifndef GAMEBOYEMULATOR_UTILITY_H
#define GAMEBOYEMULATOR_UTILITY_H
#include <cstdint>
#include <string>
#include <queue>
#include <bitset>
#include <chrono>
#include <thread>


namespace utility {
	//general

	//cpu
	/**
	 * Returns true if 'address' is within the inclusive range [low, high].
	 * @param address Value to test.
	 * @param low Lower bound of the range, inclusive.
	 * @param high Upper bound of the range, inclusive.
	 * @return True if low <= address <= high.
	 */
	inline bool inRange(uint16_t address, uint16_t low, uint16_t high)
	{
		return low <= address && address <= high;
	};

	/**
	 * Sets the bit at position 'bit' in 'value' and returns the new value.
	 * @param value Source byte, left unmodified.
	 * @param bit Bit position to set (0-7).
	 * @return The value with the given bit set.
	 */
	inline uint8_t setBit(const uint8_t value, const uint8_t bit)
	{
		std::bitset<8> bits(value);
		bits.set(bit);
		return static_cast<uint8_t>(bits.to_ulong());
	};

	/**
	 * Clears the bit at position 'bit' in 'value' and returns the new value.
	 * @param value Source byte, left unmodified.
	 * @param bit Bit position to clear (0-7).
	 * @return The value with the given bit cleared.
	 */
	inline uint8_t clearBit(const uint8_t value, const uint8_t bit)
	{
		std::bitset<8> bits(value);
		bits.reset(bit);
		return static_cast<uint8_t>(bits.to_ulong());
	};

	/**
	 * Sets or clears the bit at position 'bit' in 'value' based on 'bit_on'.
	 * @param value Byte modified in place.
	 * @param bit Bit position to change (0-7).
	 * @param bit_on True to set the bit, false to clear it.
	 */
	inline void setBitTo(uint8_t& value, const uint8_t& bit, bool bit_on)
	{
		std::bitset<8> bits(value);
		bits.set(bit, bit_on);
		value = static_cast<uint8_t>(bits.to_ulong());
	};

	/**
	 * Returns true if the bit at position 'bit' in 'value' is set.
	 * Templated on the value width so 8- and 16-bit registers both work.
	 * @tparam T Unsigned integer type of the value.
	 * @param value Value to test.
	 * @param bit Bit position to test.
	 * @return True if the given bit is set.
	 */
	template <typename T>
	inline bool checkBit(const T value, const uint8_t bit)
	{
		return std::bitset<sizeof(T) * 8>(value).test(bit);
	};

	/**
	 * Milliseconds from a monotonic clock.
	 * Uses std::chrono rather than SDL so the emulator core stays free of SDL (the
	 * headless romrunner links it too).
	 * @return Milliseconds elapsed since the steady clock's epoch.
	 */
	inline uint64_t GetTicks()
	{
		using namespace std::chrono;
		return static_cast<uint64_t>(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
	}

	/**
	 * Blocks the calling thread for the given duration.
	 * @param ms Number of milliseconds to sleep.
	 */
	inline void Delay(const uint32_t ms) {std::this_thread::sleep_for(std::chrono::milliseconds(ms));}

	/**
	 * Removes all elements from the queue, leaving it empty.
	 * @tparam T Element type of the queue.
	 * @param queue Queue to clear in place.
	 */
	template <typename T>
	inline void ClearQueue(std::queue<T>& queue)
	{
		std::queue<T> empty;
		std::swap(queue, empty);
	}
}


#endif //GAMEBOYEMULATOR_UTILITY_H