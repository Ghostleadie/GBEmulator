#include "interrupts.h"

interrupts::interrupts(std::shared_ptr <cpu> cpu)
{
	m_cpu = cpu;
}

void interrupts::requestInterrupts(interruptType type)
{

}

void interrupts::handleInterrupts(std::weak_ptr<cpuContext> ctx, uint16_t address)
{
	if (auto context = ctx.lock())
	{
		m_cpu->pushStack(context->registers.programCounter);
		context->registers.programCounter = address;
	}
}
