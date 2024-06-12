#pragma once
#include "cartridge.h"

class bus
{
public:
	bus(std::shared_ptr <cartridgeLoader> loader);
	uint8_t busRead(uint16_t address);
	void busWrite(uint16_t address, uint8_t value);
private:
	std::shared_ptr <cartridgeLoader> m_loader;
};