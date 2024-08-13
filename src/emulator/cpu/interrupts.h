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
	interrupts() {};
	interrupts(std::shared_ptr <cpu> cpu);
	
	void ConnectCPU(std::shared_ptr <cpu> cpu);

	void requestInterrupts(interruptType type);
	void handleInterrupts(std::weak_ptr<cpuContext> ctx);
	bool checkInterrupt(std::weak_ptr<cpuContext> ctx, std::weak_ptr<cpu> cpu, uint16_t address, interruptType type);
private:
	void handleInterrupt(std::weak_ptr<cpuContext> ctx, std::weak_ptr<cpu> cpu, uint16_t address);
	std::shared_ptr <cpu> m_cpu;
};