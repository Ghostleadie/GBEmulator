//
// Created by Jack_ on 31/08/2025.
//

#ifndef GAMEBOYEMULATOR_TIMER_H
#define GAMEBOYEMULATOR_TIMER_H
#include "../interfaces/IComponentMessanger.h"
#include "../interfaces/IInterruptSink.h"
#include "../interfaces/IClocked.h"

class timer : public IComponentMessanger, public IClocked
{
public:
	timer() = default;
	~timer() override = default;

	void init(std::shared_ptr<IInterruptSink> interruptSink);
	void tick() override;

	// Read a byte from a device-local address (offset from base).
	uint8_t read(uint16_t address);

	// Write a byte to a device-local address (offset from base).
	void write(uint16_t address, uint8_t value);

private:
	uint16_t div;
	uint8_t tima;
	uint8_t tma;
	uint8_t tac;

	std::weak_ptr<IInterruptSink> m_interruptSink;
};


#endif //GAMEBOYEMULATOR_TIMER_H