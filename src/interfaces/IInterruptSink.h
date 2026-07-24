//
// Created by Jack_ on 27/06/2026.
//

#ifndef GAMEBOYEMULATOR_INTERRUPTSINK_H
#define GAMEBOYEMULATOR_INTERRUPTSINK_H

#include <cstdint>

// Interrupt-request/enable bits (as laid out in IF/IF at 0xFF0F/0xFFFF). Scoped
// for type safety; combine with the register byte via static_cast<uint8_t>.
enum class interruptTypes : uint8_t
{
	INT_VBLANK = 1,
	INT_LCD_STAT = 2,
	INT_TIMER = 4,
	INT_SERIAL = 8,
	INT_JOYPAD = 16
};

class IInterruptSink
{
public:
	virtual ~IInterruptSink() = default;
	/** Raises (requests) the given interrupt so it will be serviced when enabled.
	 * @param t the interrupt line to request; implementations set the matching IF bit (0xFF0F).
	 */
	virtual void raise(interruptTypes t) = 0;
};

#endif //GAMEBOYEMULATOR_INTERRUPTSINK_H