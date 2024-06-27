#pragma once
#include "cartridge.h"
#include "memory.h"

class bus
{
public:
	bus(std::shared_ptr <cartridgeLoader> loader, std::shared_ptr <memory> memory);
	uint8_t read8bit(uint16_t address);
	uint16_t read16bit(uint16_t address);
	void write(uint16_t address, uint8_t value);
	void write(uint16_t address, uint16_t value);
private:
	std::shared_ptr <cartridgeLoader> m_loader;
	std::shared_ptr <memory> m_memory;
};


