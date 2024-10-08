#include "interrupts.h"

interrupts::interrupts(std::shared_ptr <cpu> cpu)
{
	m_cpu = cpu;
}

void interrupts::ConnectCPU(std::shared_ptr<cpu> cpu)
{
	m_cpu = cpu;
}

void interrupts::requestInterrupts(interruptType type)
{

}

void interrupts::handleInterrupts(std::weak_ptr<cpuContext> ctx)
{
	if (checkInterrupt(ctx, m_cpu, 0x40, IT_VBLANK)) {

	}
	else if (checkInterrupt(ctx, m_cpu, 0x48, IT_LCD_STAT)) {

	}
	else if (checkInterrupt(ctx, m_cpu, 0x50, IT_TIMER)) {

	}
	else if (checkInterrupt(ctx, m_cpu, 0x58, IT_SERIAL)) {

	}
	else if (checkInterrupt(ctx, m_cpu, 0x60, IT_JOYPAD)) {

	}
}

bool interrupts::checkInterrupt(std::weak_ptr<cpuContext> ctx, std::weak_ptr<cpu> cpu, uint16_t address, interruptType type)
{
	if (auto context = ctx.lock())
	{
		if (context->interruptFlags & type && context->interruptEnableRegister & type) {
			handleInterrupt(ctx, cpu, address);
			context->interruptFlags &= ~type;
			context->halted = false;
			context->masterInterruptEnabled = false;

			return true;
		}

		return false;
	}
	return false;
}

void interrupts::handleInterrupt(std::weak_ptr<cpuContext> ctx, std::weak_ptr<cpu> cpu, uint16_t address)
{
	if (auto context = ctx.lock())
	{
		if (auto m_cpu = cpu.lock())
		{
			m_cpu->pushStack(context->registers.programCounter);
			context->registers.programCounter = address;
		}
	}
}
