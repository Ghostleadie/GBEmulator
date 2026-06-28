//
// Created by Jack_ on 27/06/2026.
//

#ifndef GAMEBOYEMULATOR_INTERRUPTSINK_H
#define GAMEBOYEMULATOR_INTERRUPTSINK_H

enum interruptTypes
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
	virtual void raise(interruptTypes t) = 0;
};

#endif //GAMEBOYEMULATOR_INTERRUPTSINK_H