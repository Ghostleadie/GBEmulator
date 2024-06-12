#include "bus.h"
#include "cpu.h"
#include "emulator.h"
#include "spdlog/fmt/bin_to_hex.h"
#include <format>

cpu::cpu(std::shared_ptr <bus> bus, std::shared_ptr <instructions> instructions, std::shared_ptr <cartridgeLoader> cartridgeLoader)
{
	m_bus = bus;
	m_instructions = instructions;
	m_loader = cartridgeLoader;
}

void cpu::init()
{
	ctx.registers.programCounter = 0x100;
	ctx.registers.a = 0x01;
}

bool cpu::step()
{
	if (!ctx.halted)
	{
		fetchInstruction();
		fetchData();
		char hex_string[20];
		sprintf(hex_string, "%X", ctx.currentOpcode); //convert number to hex
		GBE_INFO("current instruction type: {}", hex_string);

		execute();
	}
	//GBE_WARN("Cpu not yet implemented.\n");
	return true;
}

void cpu::fetchInstruction()
{
	ctx.currentOpcode = m_bus->busRead(ctx.registers.programCounter++);
	ctx.currentInstruction = m_instructions->instructionByOpcode(ctx.currentOpcode);

	
}

void cpu::fetchData()
{
	ctx.memoryDestination = 0;
	ctx.destinationIsMemory = false;

	switch (ctx.currentInstruction.mode)
	{
	case AM_IMP:
		GBE_INFO("AM_IMP instruction	Program Count: {}", ctx.registers.programCounter);
		return;
		break;
	case AM_R:
		GBE_INFO("AM_R instruction	Program Count : {}", ctx.registers.programCounter);
		ctx.fetchedData = cpuReadRegistry(ctx.currentInstruction.reg1);
		return;
		break;
	case AM_R_D8:
		GBE_INFO("AM_R_D8 instruction	Program Count : {}", ctx.registers.programCounter);
		ctx.fetchedData = m_bus->busRead(ctx.registers.programCounter);
		emulation::cycles(1);
		ctx.registers.programCounter++;
		break;
	case AM_D16:
	//brackets need to create variables in the case statement
	{
		GBE_INFO("AM_D16 instruction	Program Count : {}", ctx.registers.programCounter);
		//can only read 8 bytes so we have to do it in 2 parts for 16 bytes
		uint16_t lowValue = m_bus->busRead(ctx.registers.programCounter);
		emulation::cycles(1);

		uint16_t highValue = m_bus->busRead(ctx.registers.programCounter + 1);
		emulation::cycles(1);

		//fetched data equal to low or high shifted into the high position
		ctx.fetchedData = lowValue | (highValue << 8);

		ctx.registers.programCounter += 2;

		return;
	}
		break;

	default:
	{
		char hex_string[20];
		sprintf(hex_string, "%X", ctx.currentOpcode);
		GBE_WARN("Unknown Addressing Mode! instruction mode:{} opcode:{}", (int)ctx.currentInstruction.mode, hex_string);
		exit(-7);
		return;
	}
	}
}

void cpu::execute()
{
}

uint16_t cpu::cpuReadRegistry(registryType rt)
{
	switch (rt) {
	case RT_A: 
		return ctx.registers.a;
		break;
	case RT_F: 
		return ctx.registers.f;
		break;
	case RT_B: 
		return ctx.registers.b;
		break;
	case RT_C: 
		return ctx.registers.c;
		break;
	case RT_D: 
		return ctx.registers.d;
		break;
	case RT_E: 
		return ctx.registers.e;
		break;
	case RT_H: 
		return ctx.registers.h;
		break;
	case RT_L: 
		return ctx.registers.l;
		break;

	case RT_AF: 
		return reverse(*((uint16_t*)&ctx.registers.a));
		break;
	case RT_BC: 
		return reverse(*((uint16_t*)&ctx.registers.b));
		break;
	case RT_DE: 
		return reverse(*((uint16_t*)&ctx.registers.d));
		break;
	case RT_HL: 
		return reverse(*((uint16_t*)&ctx.registers.h));
		break;

	case RT_PC:
		return ctx.registers.programCounter;
		break;
	case RT_SP:
		return ctx.registers.stackCounter;
		break;
	default: return 0;
	}
}

uint16_t cpu::reverse(uint16_t n)
{
	return ((n & 0xFF00) >> 8) | ((n & 0x00FF) << 8);
}
