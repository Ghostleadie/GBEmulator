#pragma once

#include "cpu.h"

enum interruptType
{
	IT_VBLANK = 1,
	IT_LCD_STAT = 2,
	IT_TIMER = 4,
	IT_SERIAL = 8,
	IT_JOYPAD = 16
};

class interrupts
{
public:
	interrupts(std::shared_ptr <cpu> cpu);
	
	void requestInterrupts(interruptType type);
	void handleInterrupts(std::weak_ptr<cpuContext> ctx, uint16_t address);
private:
	std::shared_ptr <cpu> m_cpu;
};