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
};


#endif //GAMEBOYEMULATOR_COMPONENTMESSANGER_H
