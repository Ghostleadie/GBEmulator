#pragma once


class cartridgeLoader;
class memory;
class cpu;

class bus
{
public:
	bus(std::shared_ptr <cartridgeLoader> loader, std::shared_ptr <memory> memory);
	bus(std::shared_ptr <cartridgeLoader> loader, std::shared_ptr <memory> memory, std::shared_ptr <cpu> cpu);
	void connectCPU(std::shared_ptr <cpu> cpu);
	uint8_t read(uint16_t address, int bits = 8);
	void write(uint16_t address, uint8_t value);
	void write(uint16_t address, uint16_t value);
private:
	std::shared_ptr <cartridgeLoader> m_loader;
	std::shared_ptr <memory> m_memory;
	std::shared_ptr <cpu> m_cpu;
};


