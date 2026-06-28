//
// Created by Jack_ on 27/06/2026.
//

#ifndef GAMEBOYEMULATOR_INTERRUPTCONTROLLER_H
#define GAMEBOYEMULATOR_INTERRUPTCONTROLLER_H
#include "../interfaces/IComponentMessanger.h"
#include "../interfaces/InterruptSink.h"


// Owns the two interrupt registers and is the single place peripherals raise
// interrupts into. The bus maps it like any other device:
//   IF (interrupt flags)  -> 0xFF0F
//   IE (interrupt enable) -> 0xFFFF
// Peripherals depend only on IInterruptSink::raise(); the CPU consumes IF/IE
// through the bus (0xFF0F / 0xFFFF) when it services interrupts.
class interruptController : public memoryComponentMessanger, public IInterruptSink
{
public:
	interruptController() = default;
	~interruptController() override = default;

	// IInterruptSink: set the pending bit for an interrupt source.
	void raise(interruptTypes t) override;

	// device interface: 0xFF0F = IF, 0xFFFF = IE (global address).
	uint8_t read(uint16_t address) override;
	void write(uint16_t address, uint8_t value) override;

private:
	uint8_t m_interruptFlags = 0;   // IF  (0xFF0F)
	uint8_t m_interruptEnable = 0;  // IE  (0xFFFF)
};



#endif //GAMEBOYEMULATOR_INTERRUPTCONTROLLER_H
