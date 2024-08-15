#pragma once
#include "../cpu/cpu.h"

class io
{
public:
	uint8_t read(std::weak_ptr<cpu> cpu, uint16_t address);
	void write(std::weak_ptr<cpu> cpu, uint16_t address, uint8_t value);
	static inline char serialData[2];
private:
	
};