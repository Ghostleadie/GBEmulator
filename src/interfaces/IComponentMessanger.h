//
// Created by Jack_ on 30/08/2025.
//

#ifndef GAMEBOYEMULATOR_COMPONENTMESSANGER_H
#define GAMEBOYEMULATOR_COMPONENTMESSANGER_H

#include <memory>
#include <string>
#include <cstdint>

// Single device interface: anything the bus can read from / write to
// (peripherals, RAM regions, the interrupt controller, the bus itself)
// implements this.
class IComponentMessanger
{
public:
	virtual ~IComponentMessanger() = default;

	/** Reads a byte from a device-local address (offset from the device's base).
	 * @param address device-local address to read.
	 * @return the byte stored at that address.
	 */
	virtual uint8_t read(uint16_t address) = 0;

	/** Writes a byte to a device-local address (offset from the device's base).
	 * @param address device-local address to write.
	 * @param value byte to store at that address.
	 */
	virtual void write(uint16_t address, uint8_t value) = 0;

	/** Reads a little-endian 16-bit value: low byte at address, high byte at address+1.
	 * @param address device-local address of the low byte (address+1 wraps within 16 bits).
	 * @return the 16-bit value assembled from the two bytes.
	 */
	inline uint16_t read16(uint16_t address) {
		const uint8_t lo = read(address);
		const uint8_t hi = read(static_cast<uint16_t>(address + 1)); // wrap to 16-bit
		return static_cast<uint16_t>((static_cast<uint16_t>(hi) << 8) | lo);
	}

	/** Writes a little-endian 16-bit value: low byte at address, high byte at address+1.
	 * @param address device-local address of the low byte.
	 * @param value 16-bit value to store (low byte written first, then high byte).
	 */
	inline void write16(uint16_t address, uint16_t value) {
		write(address, static_cast<uint8_t>(value & 0xFF));                 // low first
		write(static_cast<uint16_t>(address + 1), static_cast<uint8_t>(value >> 8)); // high next
	}
};


#endif //GAMEBOYEMULATOR_COMPONENTMESSANGER_H
