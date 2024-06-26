#include "bus.h"
#include "cpu.h"
#include "emulator.h"
#include "spdlog/fmt/bin_to_hex.h"
#include <format>
#include "../Utility/utility.h"

cpu::cpu(std::shared_ptr <bus> bus, std::shared_ptr <instructions> instructions, std::shared_ptr <cartridgeLoader> cartridgeLoader)
{
	m_bus = bus;
	m_instructions = instructions;
	m_loader = cartridgeLoader;
	ctx = std::shared_ptr <cpuContext>(new cpuContext());
}

void cpu::init()
{
	ctx->registers.programCounter = 0x100;
	ctx->registers.a = 0x01;
}

bool cpu::step()
{
	if (!ctx->halted)
	{
		fetchInstruction();
		fetchData();

		char hex_string[20];
		sprintf(hex_string, "%X", ctx->currentOpcode); //convert number to hex
		GBE_INFO("current instruction type: {}", hex_string);

		execute();
	}
	return true;
}

void cpu::fetchInstruction()
{
	ctx->currentOpcode = m_bus->read8bit(ctx->registers.programCounter++);
	ctx->currentInstruction = m_instructions->instructionByOpcode(ctx->currentOpcode);
}

void cpu::fetchData()
{
	ctx->memoryDestination = 0;
	ctx->destinationIsMemory = false;

	//if (ctx->currentInstruction == NULL) {
	//	return;
	//}

	switch (ctx->currentInstruction.mode)
	{
	case AM_IMP:
		GBE_INFO("AM_IMP instruction	Program Count: {}", ctx->registers.programCounter);
		break;
	case AM_R_D16:
	{
		GBE_INFO("AM_R_D16 instruction	Program Count : {}", ctx->registers.programCounter);
		//can only read 8 bytes so we have to do it in 2 parts for 16 bytes
		uint16_t lowValue = m_bus->read8bit(ctx->registers.programCounter);
		emulation::cycles(1);

		uint16_t highValue = m_bus->read8bit(ctx->registers.programCounter + 1);
		emulation::cycles(1);

		//fetched data equal to low or high shifted into the high position
		ctx->fetchedData = lowValue | (highValue << 8);

		ctx->registers.programCounter += 2;
	}
		break;
	case AM_R_R:
		ctx->fetchedData = readRegistry(ctx->currentInstruction.reg2);
		break;
	case AM_MR_R:
		ctx->fetchedData = readRegistry(ctx->currentInstruction.reg2);
		ctx->memoryDestination = readRegistry(ctx->currentInstruction.reg1);
		ctx->destinationIsMemory = true;

		if (ctx->currentInstruction.reg1 == RT_C)
		{
			ctx->memoryDestination |= 0xFF00;
		}
		break;
	case AM_R:
		GBE_INFO("AM_R instruction	Program Count : {}", ctx->registers.programCounter);
		ctx->fetchedData = readRegistry(ctx->currentInstruction.reg1);
		return;
		break;
	case AM_R_D8:
		GBE_INFO("AM_R_D8 instruction	Program Count : {}", ctx->registers.programCounter);
		ctx->fetchedData = m_bus->read8bit(ctx->registers.programCounter);
		emulation::cycles(1);
		ctx->registers.programCounter++;
		break;
	case AM_R_MR:
	{
		uint16_t address = readRegistry(ctx->currentInstruction.reg2);
		
		if (ctx->currentInstruction.reg1 == RT_C)
		{
			address |= 0xFF00;
		}

		ctx->fetchedData = m_bus->read8bit(address);
		emulation::cycles(1);
	}
		break;
	case AM_R_HLI:
		ctx->fetchedData = m_bus->read8bit(readRegistry(ctx->currentInstruction.reg2));
		emulation::cycles(1);
		setRegistry(RT_HL, readRegistry(RT_HL) + 1);
		break;
	case AM_R_HLD:
		ctx->fetchedData = m_bus->read8bit(readRegistry(ctx->currentInstruction.reg2));
		emulation::cycles(1);
		setRegistry(RT_HL, readRegistry(RT_HL) - 1);
		break;
	case AM_HLI_R:
		ctx->fetchedData = readRegistry(ctx->currentInstruction.reg2);
		ctx->memoryDestination = readRegistry(ctx->currentInstruction.reg1);
		ctx->destinationIsMemory = true;
		setRegistry(RT_HL, readRegistry(RT_HL) + 1);
		break;
	case AM_HLD_R:
		ctx->fetchedData = readRegistry(ctx->currentInstruction.reg2);
		ctx->memoryDestination = readRegistry(ctx->currentInstruction.reg1);
		ctx->destinationIsMemory = true;
		setRegistry(RT_HL, readRegistry(RT_HL) - 1);
		break;
	case AM_R_A8:
		ctx->fetchedData = m_bus->read8bit(ctx->registers.programCounter);
		emulation::cycles(1);
		ctx->registers.programCounter++;
		break;
	case AM_A8_R:
		ctx->memoryDestination = m_bus->read8bit(ctx->registers.programCounter) | 0xFF00;
		ctx->destinationIsMemory = true;
		emulation::cycles(1);
		ctx->registers.programCounter++;
		break;
	case AM_HL_SPR:
		ctx->fetchedData = m_bus->read8bit(ctx->registers.programCounter);
		emulation::cycles(1);
		ctx->registers.programCounter++;
		break;
	case  AM_D16:
		//brackets need to create variables in the case statement
	{
		GBE_INFO("AM_D16 instruction	Program Count : {}", ctx->registers.programCounter);
		//can only read 8 bytes so we have to do it in 2 parts for 16 bytes
		uint16_t lowValue = m_bus->read8bit(ctx->registers.programCounter);
		emulation::cycles(1);

		uint16_t highValue = m_bus->read8bit(ctx->registers.programCounter + 1);
		emulation::cycles(1);

		//fetched data equal to low or high shifted into the high position
		ctx->fetchedData = lowValue | (highValue << 8);

		ctx->registers.programCounter += 2;

		//return;
	}
	break;
	case AM_D8:
		ctx->fetchedData = m_bus->read8bit(ctx->registers.programCounter);
		emulation::cycles(1);
		ctx->registers.programCounter++;
		break;
	case AM_D16_R:
	{
		GBE_INFO("AM_D16_R instruction	Program Count : {}", ctx->registers.programCounter);
		//can only read 8 bytes so we have to do it in 2 parts for 16 bytes
		uint16_t lowValue = m_bus->read8bit(ctx->registers.programCounter);
		emulation::cycles(1);

		uint16_t highValue = m_bus->read8bit(ctx->registers.programCounter + 1);
		emulation::cycles(1);

		//fetched data equal to low or high shifted into the high position
		ctx->fetchedData = lowValue | (highValue << 8);
		ctx->destinationIsMemory = true;

		ctx->registers.programCounter += 2;
		ctx->fetchedData = readRegistry(ctx->currentInstruction.reg2);
	}
		break;
	case AM_MR_D8:
		GBE_INFO("AM_MR_D8 instruction	Program Count : {}", ctx->registers.programCounter);
		ctx->fetchedData = m_bus->read8bit(ctx->registers.programCounter);
		emulation::cycles(1);
		ctx->registers.programCounter++;
		ctx->memoryDestination = readRegistry(ctx->currentInstruction.reg2);
		ctx->destinationIsMemory = true;
		break;
	case AM_MR:
		GBE_INFO("AM_MR_D8 instruction	Program Count : {}", ctx->registers.programCounter);
		ctx->memoryDestination = readRegistry(ctx->currentInstruction.reg1);
		ctx->destinationIsMemory = true;
		ctx->fetchedData = m_bus->read8bit(readRegistry(ctx->currentInstruction.reg1));
		emulation::cycles(1);		
		break;
	case AM_A16_R:
	{
		GBE_INFO("AM_D16_R instruction	Program Count : {}", ctx->registers.programCounter);
		//can only read 8 bytes so we have to do it in 2 parts for 16 bytes
		uint16_t lowValue = m_bus->read8bit(ctx->registers.programCounter);
		emulation::cycles(1);

		uint16_t highValue = m_bus->read8bit(ctx->registers.programCounter + 1);
		emulation::cycles(1);

		//fetched data equal to low or high shifted into the high position
		ctx->fetchedData = lowValue | (highValue << 8);
		ctx->destinationIsMemory = true;

		ctx->registers.programCounter += 2;
		ctx->fetchedData = readRegistry(ctx->currentInstruction.reg2);
	}
		break;
	case AM_R_A16:
	{
		GBE_INFO("AM_A16_R instruction	Program Count : {}", ctx->registers.programCounter);
		//can only read 8 bytes so we have to do it in 2 parts for 16 bytes
		uint16_t lowValue = m_bus->read8bit(ctx->registers.programCounter);
		emulation::cycles(1);

		uint16_t highValue = m_bus->read8bit(ctx->registers.programCounter + 1);
		emulation::cycles(1);

		//fetched data equal to low or high shifted into the high position
		uint16_t address = lowValue | (highValue << 8);

		ctx->registers.programCounter += 2;
		ctx->fetchedData = m_bus->read8bit(address);
		emulation::cycles(1);
	}
		break;
	default:
	{
		char hex_string[20];
		sprintf(hex_string, "%X", ctx->currentOpcode);
		GBE_WARN("Unknown Addressing Mode! instruction mode:{} opcode:{}", (int)ctx->currentInstruction.mode, hex_string);
		exit(-7);
		return;
	}
	}
}

void cpu::execute()
{
	switch (ctx->currentInstruction.type)
	{
	case IN_NONE:
		GBE_ERROR("Invalid instruction");
		break;
	case IN_NOP:

		break;
	case IN_ADC:
		GBE_ERROR("IN_ADC TODO");
		break;
	case IN_ADD:
		GBE_ERROR("IN_ADD TODO");
		break;
	case IN_AND:
		GBE_ERROR("IN_AND TODO");
		break;
	case IN_BIT:
		GBE_ERROR("IN_BIT TODO");
		break;
	case IN_CALL:
		GBE_ERROR("IN_CALL TODO");
		break;
	case IN_CB:
		GBE_ERROR("IN_CB TODO");
		break;
	case IN_CCF:
		GBE_ERROR("IN_CCF TODO");
		break;
	case IN_CP:
		GBE_ERROR("IN_CP TODO");
		break;
	case IN_CPL:
		GBE_ERROR("IN_CPL TODO");
		break;
	case IN_DAA:
		GBE_ERROR("IN_DAA TODO");
		break;
	case IN_DEC:
		GBE_ERROR("IN_DEC TODO");
		break;
	case IN_DI:
		ctx->masterInteruptEnabled = false;
		break;
	case IN_EI:
		GBE_ERROR("IN_EI TODO");
		break;
	case IN_ERR:
		GBE_ERROR("IN_ERR TODO");
		break;
	case IN_HALT:
		GBE_ERROR("IN_HALT TODO");
		break;
	case IN_INC:
		GBE_ERROR("IN_INC TODO");
		break;
	case IN_JP:
		if (checkCondition(ctx.get()))
		{
			ctx->registers.programCounter = ctx->fetchedData;
			emulation::cycles(1);
		}
		break;
	case IN_JPHL:
		GBE_ERROR("IN_JPHL TODO");
		break;
	case IN_JR:
		GBE_ERROR("IN_JR TODO");
		break;
	case IN_LD:
		if (ctx->destinationIsMemory)
		{
			//check if 16 bit
			if (ctx->currentInstruction.reg2 >= RT_AF)
			{
				emulation::cycles(1);
				m_bus->write(ctx->memoryDestination, ctx->fetchedData);
			}
			else
			{
				m_bus->write(ctx->memoryDestination, uint8_t(ctx->fetchedData));
			}
		}

		//special case
		if (ctx->currentInstruction.mode == AM_HL_SPR)
		{
			uint8_t hFlag = (readRegistry(ctx->currentInstruction.reg2) & 0xF) + (ctx->fetchedData & 0xF) >= 10;
			uint8_t cFlag = (readRegistry(ctx->currentInstruction.reg2) & 0xFF) + (ctx->fetchedData & 0xFF) >= 0x100;
		
			setFlags(ctx.get(), 0, 0, hFlag, cFlag);
			setRegistry(ctx->currentInstruction.reg1, readRegistry(ctx->currentInstruction.reg2) + (char)ctx->fetchedData); //cast to char as it could be a negative
		}
		break;
	case IN_LDH:
		if (ctx->currentInstruction.reg1 == RT_A)
		{
			setRegistry(ctx->currentInstruction.reg1, m_bus->read8bit(0xFF00 | ctx->fetchedData));
		}
		else {
			m_bus->write(0xFF00 | ctx->fetchedData, ctx->registers.a);
		}

		emulation::cycles(1);
		break;
	case IN_OR:
		GBE_ERROR("IN_OR TODO");
		break;
	case IN_POP:
		GBE_ERROR("IN_POP TODO");
		break;
	case IN_PUSH:
		GBE_ERROR("IN_PUSH TODO");
		break;
	case IN_RES:
		GBE_ERROR("IN_RES TODO");
		break;
	case IN_RET:
		GBE_ERROR("IN_RET TODO");
		break;
	case IN_RETI:
		GBE_ERROR("IN_RETI TODO");
		break;
	case IN_RL:
		GBE_ERROR("IN_RL TODO");
		break;
	case IN_RLA:
		GBE_ERROR("IN_RLA TODO");
		break;
	case IN_RLC:
		GBE_ERROR("IN_RLC TODO");
		break;
	case IN_RLCA:
		GBE_ERROR("IN_RLCA TODO");
		break;
	case IN_RR:
		GBE_ERROR("IN_RR TODO");
		break;
	case IN_RRA:
		GBE_ERROR("IN_RRA TODO");
		break;
	case IN_RRC:
		GBE_ERROR("IN_RRC TODO");
		break;
	case IN_RRCA:
		GBE_ERROR("IN_RRCA TODO");
		break;
	case IN_RST:
		GBE_ERROR("IN_RST TODO");
		break;
	case IN_SBC:
		GBE_ERROR("IN_SBC TODO");
		break;
	case IN_SCF:
		GBE_ERROR("IN_SCF TODO");
		break;
	case IN_SET:
		GBE_ERROR("IN_SET TODO");
		break;
	case IN_SLA:
		GBE_ERROR("IN_SLA TODO");
		break;
	case IN_SRA:
		GBE_ERROR("IN_SRA TODO");
		break;
	case IN_SRL:
		GBE_ERROR("IN_SRL TODO");
		break;
	case IN_STOP:
		GBE_ERROR("IN_STOP TODO");
		break;
	case IN_SUB:
		GBE_ERROR("IN_SUB TODO");
		break;
	case IN_SWAP:
		GBE_ERROR("IN_SWAP TODO");
		break;
	case IN_XOR:
		ctx->registers.a ^= ctx->fetchedData & 0xFF;
		setFlags(ctx.get(), ctx->registers.a == 0, 0, 0, 0);
		break;
	default:
		GBE_ERROR("Unknown instruction");
	}
}

uint16_t cpu::readRegistry(registryType rt)
{
	switch (rt) {
	case RT_A: 
		return ctx->registers.a;
		break;
	case RT_F: 
		return ctx->registers.f;
		break;
	case RT_B: 
		return ctx->registers.b;
		break;
	case RT_C: 
		return ctx->registers.c;
		break;
	case RT_D: 
		return ctx->registers.d;
		break;
	case RT_E: 
		return ctx->registers.e;
		break;
	case RT_H: 
		return ctx->registers.h;
		break;
	case RT_L: 
		return ctx->registers.l;
		break;
	// 16 bit
	case RT_AF: 
		return reverse(*((uint16_t*)&ctx->registers.a));
		break;
	case RT_BC: 
		return reverse(*((uint16_t*)&ctx->registers.b));
		break;
	case RT_DE: 
		return reverse(*((uint16_t*)&ctx->registers.d));
		break;
	case RT_HL: 
		return reverse(*((uint16_t*)&ctx->registers.h));
		break;

	case RT_PC:
		return ctx->registers.programCounter;
		break;
	case RT_SP:
		return ctx->registers.stackCounter;
		break;
	default: return 0;
	}
}

void cpu::setRegistry(registryType rt, uint16_t value)
{
	switch (rt) {
		//8 bit
	case RT_A: 
		ctx->registers.a = value & 0xFF;
		break;
	case RT_F:
		ctx->registers.f = value & 0xFF;
		break;
	case RT_B:
		ctx->registers.b = value & 0xFF;
		break;
	case RT_C:
		ctx->registers.c = value & 0xFF;
		break;
	case RT_D:
		ctx->registers.d = value & 0xFF;
		break;
	case RT_E: 
		ctx->registers.e = value & 0xFF;
		break;
	case RT_H: 
		ctx->registers.h = value & 0xFF;
		break;
	case RT_L:
		ctx->registers.l = value & 0xFF;
		break;
		//16 bit
	case RT_AF:
		*((uint16_t*)&ctx->registers.a) = reverse(value);
		break;
	case RT_BC:
		*((uint16_t*)&ctx->registers.b) = reverse(value);
		break;
	case RT_DE:
		*((uint16_t*)&ctx->registers.d) = reverse(value);
		break;
	case RT_HL: 
		*((uint16_t*)&ctx->registers.h) = reverse(value);
		break;

	case RT_PC:
		ctx->registers.programCounter = value;
		break;
	case RT_SP:
		ctx->registers.stackCounter = value;
		break;
	case RT_NONE:
		break;
	}
}

uint16_t cpu::reverse(uint16_t n)
{
	return ((n & 0xFF00) >> 8) | ((n & 0x00FF) << 8);
}

bool cpu::checkCondition(cpuContext* ctx)
{
	bool z = CPU_FLAG_Z;
	bool c = CPU_FLAG_C;

	switch (ctx->currentInstruction.cond) {
	case CT_NONE: return true;
	case CT_C: return c;
	case CT_NC: return !c;
	case CT_Z: return z;
	case CT_NZ: return !z;
	}

	return false;
}

void cpu::setFlags(cpuContext* ctx, char z, char n, char h, char c)
{
	if (z != -1) {
		BIT_SET(ctx->registers.f, 7, z);
	}

	if (n != -1) {
		BIT_SET(ctx->registers.f, 6, n);
	}

	if (h != -1) {
		BIT_SET(ctx->registers.f, 5, h);
	}

	if (c != -1) {
		BIT_SET(ctx->registers.f, 4, c);
	}
}