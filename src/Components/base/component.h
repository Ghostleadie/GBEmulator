//
// Created by Jack_ on 30/08/2025.
//

#ifndef GAMEBOYEMULATOR_COMPONENT_H
#define GAMEBOYEMULATOR_COMPONENT_H
#include "componentMessanger.h"


class memoryComponent
{
public:
    memoryComponent(memorycomponentMessanger* compMessanger = nullptr) : messanger(compMessanger) {};
    void setComponentMessanger(memorycomponentMessanger* messanger);

	// Read a byte from a device-local address (offset from base).
	virtual uint8_t read(uint16_t address) = 0;

	// Write a byte to a device-local address (offset from base).
	virtual void write(uint16_t address, uint8_t value) = 0;
protected:
    memorycomponentMessanger* messanger;
};


#endif //GAMEBOYEMULATOR_COMPONENT_H