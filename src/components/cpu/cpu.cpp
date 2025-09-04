//
// Created by Jack_ on 31/08/2025.
//

#include "cpu.h"
#include "../bus.h"
#include <cstdint>
void cpu::init()
{
	registers.pc = 0x100;
	registers.a = 0x01;
}

uint8_t cpu::fetch()
{
	return m_bus->read(registers.pc);
}

void cpu::execute(uint8_t opcode)
{

}

void cpu::emulateCycle()
{
	currentOpcode = fetch();
	execute(currentOpcode);
}
