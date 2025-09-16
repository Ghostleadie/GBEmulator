//
// Created by Jack_ on 30/08/2025.
//

#ifndef GAMEBOYEMULATOR_COMPONENTMESSANGER_H
#define GAMEBOYEMULATOR_COMPONENTMESSANGER_H

class memoryComponent;

class memorycomponentMessanger
{
public:
    virtual void notify(const std::string& event, std::shared_ptr<memoryComponent> sender) = 0;
	virtual ~memorycomponentMessanger() = default;

	// Read a byte from a device-local address (offset from base).
	virtual uint8_t read(uint16_t address) = 0;

	// Write a byte to a device-local address (offset from base).
	virtual void write(uint16_t address, uint8_t value) = 0;

	inline uint16_t read16(uint16_t address) {
		const uint8_t lo = read(address);
		const uint8_t hi = read(static_cast<uint16_t>(address + 1)); // wrap to 16-bit
		return static_cast<uint16_t>((static_cast<uint16_t>(hi) << 8) | lo);
	}

	inline void write16(uint16_t address, uint16_t value) {
		write(address, static_cast<uint8_t>(value & 0xFF));                 // low first
		write(static_cast<uint16_t>(address + 1), static_cast<uint8_t>(value >> 8)); // high next
	}
};


#endif //GAMEBOYEMULATOR_COMPONENTMESSANGER_H
