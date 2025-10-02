//
// Created by Jack_ on 31/08/2025.
//

#ifndef GAMEBOYEMULATOR_TIMER_H
#define GAMEBOYEMULATOR_TIMER_H
#include "base/component.h"

class cpu;

class timer : public memoryComponent
{
public:
	timer() = default;
	~timer() override = default;

	void init(std::shared_ptr<cpu> cpuPt);
	void tick();

	// Read a byte from a device-local address (offset from base).
	uint8_t read(uint16_t address);

	// Write a byte to a device-local address (offset from base).
	void write(uint16_t address, uint8_t value);

private:
	uint16_t div;
	uint8_t tima;
	uint8_t tma;
	uint8_t tac;

	std::weak_ptr<cpu> m_cpu;
};


#endif //GAMEBOYEMULATOR_TIMER_H