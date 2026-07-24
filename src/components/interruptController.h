//
// Created by Jack_ on 27/06/2026.
//

#ifndef GAMEBOYEMULATOR_INTERRUPTCONTROLLER_H
#define GAMEBOYEMULATOR_INTERRUPTCONTROLLER_H
#include "../interfaces/IComponentMessanger.h"
#include "../interfaces/IInterruptSink.h"


// Owns the two interrupt registers and is the single place peripherals raise
// interrupts into. The bus maps it like any other device:
//   IF (interrupt flags)  -> 0xFF0F
//   IE (interrupt enable) -> 0xFFFF
// Peripherals depend only on IInterruptSink::raise(); the CPU consumes IF/IE
// through the bus (0xFF0F / 0xFFFF) when it services interrupts.
class interruptController : public IComponentMessanger, public IInterruptSink
{
public:
	interruptController() = default;
	~interruptController() override = default;

	/**
	 * Sets the pending bit for an interrupt source in IF (0xFF0F).
	 * @param t Interrupt source whose flag bit is OR'd into the interrupt-flag register.
	 */
	void raise(interruptTypes t) override;

	/**
	 * Reads an interrupt register by its global address: 0xFF0F returns IF, 0xFFFF returns IE.
	 * @param address Global address, either 0xFF0F (IF) or 0xFFFF (IE).
	 * @return The selected register, or 0xFF for any other address.
	 */
	uint8_t read(uint16_t address) override;
	/**
	 * Writes an interrupt register by its global address: 0xFF0F sets IF, 0xFFFF sets IE.
	 * @param address Global address, either 0xFF0F (IF) or 0xFFFF (IE).
	 * @param value Byte to store; any other address is ignored.
	 */
	void write(uint16_t address, uint8_t value) override;

private:
	uint8_t m_interruptFlags = 0;   // IF  (0xFF0F)
	uint8_t m_interruptEnable = 0;  // IE  (0xFFFF)
};



#endif //GAMEBOYEMULATOR_INTERRUPTCONTROLLER_H
