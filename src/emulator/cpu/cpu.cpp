#include "cpu.h"
#include "../bus.h"
#include "../emulator.h"
#include "spdlog/fmt/bin_to_hex.h"
#include <format>

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

void cpu::goToAddress(std::weak_ptr<cpuContext> ctx, uint16_t address, bool pushPC)
{
	if (auto context = ctx.lock())
	{
		if (checkCondition(context))
		{
			if (pushPC) {
				emulation::cycles(2);
				pushStack(context->registers.programCounter);
			}

			context->registers.programCounter = context->fetchedData;
			emulation::cycles(1);
		}
	}
}

bool cpu::step()
{
	if (!ctx->halted)
	{
		fetchInstruction();
		fetchData();
		GBE_INFO("current instruction type: {} A: {} BC: {}{} DE: {}{} HL: {}{}", utility::uint8ToHex(ctx->currentOpcode), utility::uint8ToHex(ctx->registers.a), utility::uint8ToHex(ctx->registers.b), utility::uint8ToHex(ctx->registers.c), utility::uint8ToHex(ctx->registers.d), utility::uint8ToHex(ctx->registers.e), utility::uint8ToHex(ctx->registers.h), utility::uint8ToHex(ctx->registers.l));

		execute();
	}
	else
	{
		emulation::cycles(1);
		if (ctx->interruptFlags)
		{
			ctx->halted = false;
		}
	}

	if (ctx->masterInterruptEnabled)
	{
		//handleInterrupts(&ctx);
		ctx->enableIME = false;
	}

	if (ctx->enableIME)
	{
		ctx->masterInterruptEnabled = true;
	}
	return true;
}

void cpu::fetchInstruction()
{
	ctx->currentOpcode = m_bus->read(ctx->registers.programCounter++);
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
		break;
	case AM_R_D16:
	{
		//can only read 8 bytes so we have to do it in 2 parts for 16 bytes
		uint16_t lowValue = m_bus->read(ctx->registers.programCounter);
		emulation::cycles(1);

		uint16_t highValue = m_bus->read(ctx->registers.programCounter + 1);
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
		ctx->fetchedData = readRegistry(ctx->currentInstruction.reg1);
		return;
		break;
	case AM_R_D8:
		ctx->fetchedData = m_bus->read(ctx->registers.programCounter);
		emulation::cycles(1);
		ctx->registers.programCounter++;
		break;
	case AM_R_MR:
	{
		uint16_t address = readRegistry(ctx->currentInstruction.reg2);
		
		if (ctx->currentInstruction.reg2 == RT_C)
		{
			address |= 0xFF00;
		}

		ctx->fetchedData = m_bus->read(address);
		emulation::cycles(1);
	}
		break;
	case AM_R_HLI:
		ctx->fetchedData = m_bus->read(readRegistry(ctx->currentInstruction.reg2));
		emulation::cycles(1);
		setRegistry(RT_HL, (uint16_t)(readRegistry(RT_HL) + 1));
		break;
	case AM_R_HLD:
		ctx->fetchedData = m_bus->read(readRegistry(ctx->currentInstruction.reg2));
		emulation::cycles(1);
		setRegistry(RT_HL, (uint16_t)(readRegistry(RT_HL) - 1));
		break;
	case AM_HLI_R:
		ctx->fetchedData = readRegistry(ctx->currentInstruction.reg2);
		ctx->memoryDestination = readRegistry(ctx->currentInstruction.reg1);
		ctx->destinationIsMemory = true;
		setRegistry(RT_HL, (uint16_t)(readRegistry(RT_HL) + 1));
		break;
	case AM_HLD_R:
		ctx->fetchedData = readRegistry(ctx->currentInstruction.reg2);
		ctx->memoryDestination = readRegistry(ctx->currentInstruction.reg1);
		ctx->destinationIsMemory = true;
		setRegistry(RT_HL, (uint16_t)(readRegistry(RT_HL) - 1));
		break;
	case AM_R_A8:
		ctx->fetchedData = m_bus->read(ctx->registers.programCounter);
		emulation::cycles(1);
		ctx->registers.programCounter++;
		break;
	case AM_A8_R:
		ctx->memoryDestination = m_bus->read(ctx->registers.programCounter) | 0xFF00;
		ctx->destinationIsMemory = true;
		emulation::cycles(1);
		ctx->registers.programCounter++;
		break;
	case AM_HL_SPR:
		ctx->fetchedData = m_bus->read(ctx->registers.programCounter);
		emulation::cycles(1);
		ctx->registers.programCounter++;
		break;
	case  AM_D16:
		//brackets need to create variables in the case statement
	{
		//can only read 8 bytes so we have to do it in 2 parts for 16 bytes
		uint16_t lowValue = m_bus->read(ctx->registers.programCounter);
		emulation::cycles(1);

		uint16_t highValue = m_bus->read(ctx->registers.programCounter + 1);
		emulation::cycles(1);

		//fetched data equal to low or high shifted into the high position
		ctx->fetchedData = lowValue | (highValue << 8);

		ctx->registers.programCounter += 2;
	}
	break;
	case AM_D8:
		ctx->fetchedData = m_bus->read(ctx->registers.programCounter);
		emulation::cycles(1);
		ctx->registers.programCounter++;
		break;
	case AM_D16_R:
	{
		//can only read 8 bytes so we have to do it in 2 parts for 16 bytes
		uint16_t lowValue = m_bus->read(ctx->registers.programCounter);
		emulation::cycles(1);

		uint16_t highValue = m_bus->read(ctx->registers.programCounter + 1);
		emulation::cycles(1);

		//fetched data equal to low or high shifted into the high position
		ctx->fetchedData = lowValue | (highValue << 8);
		ctx->destinationIsMemory = true;

		ctx->registers.programCounter += 2;
		ctx->fetchedData = readRegistry(ctx->currentInstruction.reg2);
	}
		break;
	case AM_MR_D8:
		ctx->fetchedData = m_bus->read(ctx->registers.programCounter);
		emulation::cycles(1);
		ctx->registers.programCounter++;
		ctx->memoryDestination = readRegistry(ctx->currentInstruction.reg2);
		ctx->destinationIsMemory = true;
		break;
	case AM_MR:
		ctx->memoryDestination = readRegistry(ctx->currentInstruction.reg1);
		ctx->destinationIsMemory = true;
		ctx->fetchedData = m_bus->read(readRegistry(ctx->currentInstruction.reg1));
		emulation::cycles(1);		
		break;
	case AM_A16_R:
	{
		//can only read 8 bytes so we have to do it in 2 parts for 16 bytes
		uint16_t lowValue = m_bus->read(ctx->registers.programCounter);
		emulation::cycles(1);

		uint16_t highValue = m_bus->read(ctx->registers.programCounter + 1);
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
		//can only read 8 bytes so we have to do it in 2 parts for 16 bytes
		uint16_t lowValue = m_bus->read(ctx->registers.programCounter);
		emulation::cycles(1);

		uint16_t highValue = m_bus->read(ctx->registers.programCounter + 1);
		emulation::cycles(1);

		//fetched data equal to low or high shifted into the high position
		uint16_t address = lowValue | (highValue << 8);

		ctx->registers.programCounter += 2;
		ctx->fetchedData = m_bus->read(address);
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
		instructionADC();
		break;
	case IN_ADD:
		instructionADD();
		break;
	case IN_AND:
		instructionAND();
		break;
	case IN_BIT:
		instructionBIT();
		break;
	case IN_CALL:
		instructionCALL();
		break;
	case IN_CB:
		instructionCB();
		break;
	case IN_CCF:
		instructionCCF();
		break;
	case IN_CP:
		instructionCP();
		break;
	case IN_CPL:
		instructionCPL();
		break;
	case IN_DAA:
		instructionDAA();
		break;
	case IN_DEC:
		instructionDEC(m_bus);
		break;
	case IN_DI:
		instructionDI();
		break;
	case IN_EI:
		instructionEI();
		break;
	case IN_ERR:
		instructionERR();
		break;
	case IN_HALT:
		instructionHALT();
		break;
	case IN_INC:
		instructionINC(m_bus);
		break;
	case IN_JP:
		instructionJP();
		break;
	case IN_JPHL:
		instructionJPHL();
		break;
	case IN_JR:
		instructionJR();
		break;
	case IN_LD:
		instructionLD(m_bus);
	case IN_LDH:
		instructionLDH(m_bus);
	case IN_OR:
		instructionOR();
		break;
	case IN_POP:
		instructionPOP();
		break;
	case IN_PUSH:
		instructionPUSH();
		break;
	case IN_RES:
		instructionRES();
		break;
	case IN_RET:
		instructionRET();
	case IN_RETI:
		instructionRETI();
	case IN_RL:
		instructionRL();
		break;
	case IN_RLA:
		instructionRLA();
		break;
	case IN_RLC:
		instructionRLC();
		break;
	case IN_RLCA:
		instructionRLCA();
		break;
	case IN_RR:
		instructionRR();
		break;
	case IN_RRA:
		instructionRRA();
		break;
	case IN_RRC:
		instructionRRC();
		break;
	case IN_RRCA:
		instructionRRCA();
		break;
	case IN_RST:
		instructionRST();
		break;
	case IN_SBC:
		instructionSBC();
		break;
	case IN_SCF:
		instructionSCF();
		break;
	case IN_SET:
		instructionSET();
		break;
	case IN_SLA:
		instructionSLA();
		break;
	case IN_SRA:
		instructionSRA();
		break;
	case IN_SRL:
		instructionSRL();
		break;
	case IN_STOP:
		instructionSTOP();
		break;
	case IN_SUB:
		instructionSUB();
		break;
	case IN_SWAP:
		instructionSWAP();
		break;
	case IN_XOR:
		instructionXOR();
		break;
	default:
		GBE_ERROR("Unknown instruction");
		exit(-7);
	}
}

uint8_t cpu::getIERegister()
{
	return ctx->interruptEnableRegister;
}

void cpu::setIERegister(uint8_t value)
{
	ctx->interruptEnableRegister = value;
}

uint8_t cpu::getInterruptFlags()
{
	return ctx->interruptFlags;
}

void cpu::setInterruptFlags(uint8_t value)
{
	ctx->interruptFlags = value;
}

void cpu::pushStack(uint8_t value)
{
	ctx->registers.stackCounter--;
	m_bus->write(ctx->registers.stackCounter, value);
}

void cpu::pushStack(uint16_t value)
{
	ctx->registers.stackCounter--;
	m_bus->write(ctx->registers.stackCounter, uint8_t((value >> 8) & 0xFF));
	ctx->registers.stackCounter--;
	m_bus->write(ctx->registers.stackCounter, uint8_t(value & 0xFF));
}

void cpu::pushStack(uint16_t high, uint16_t low)
{
	ctx->registers.stackCounter--;
	m_bus->write(ctx->registers.stackCounter, uint8_t((high >> 8) & 0xFF));
	ctx->registers.stackCounter--;
	m_bus->write(ctx->registers.stackCounter, uint8_t(low & 0xFF));
}

uint16_t cpu::popStack(int bits)
{
	if (bits == 8)
	{
		return m_bus->read(ctx->registers.stackCounter++);
	}
	else if (bits == 16)
	{
		uint16_t lo = m_bus->read(ctx->registers.stackCounter++);
		uint16_t hi = m_bus->read(ctx->registers.stackCounter++);

		return (hi << 8) | lo;
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

uint8_t cpu::cbReadReg(registryType rt)
{
	switch (rt) {
	case RT_A: 
		return ctx->registers.a;
	case RT_F: 
		return ctx->registers.f;
	case RT_B: 
		return ctx->registers.b;
	case RT_C: 
		return ctx->registers.c;
	case RT_D: 
		return ctx->registers.d;
	case RT_E: 
		return ctx->registers.e;
	case RT_H: 
		return ctx->registers.h;
	case RT_L: 
		return ctx->registers.l;
	case RT_HL: {
		//returns a memory address
		return m_bus->read(readRegistry(RT_HL));
	}
	default:
		GBE_ERROR("INVALID REG8");
	}
}

void cpu::cbsetReg(registryType rt, uint8_t value)
{
	switch (rt) {
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
	case RT_HL:
		m_bus->write(readRegistry(RT_HL), value);
		break;
	default:
		GBE_ERROR("INVALID REG8");
	}
}

uint16_t cpu::reverse(uint16_t n)
{
	return ((n & 0xFF00) >> 8) | ((n & 0x00FF) << 8);
}

bool cpu::checkCondition(std::weak_ptr<cpuContext> ctx)
{
	if(auto context = ctx.lock())
	{ 
		bool z = utility::checkBit(context->registers.f, 7);
		bool c = utility::checkBit(context->registers.f, 4);
		switch (context->currentInstruction.cond)
		{
		case CT_NONE: return true;
		case CT_C: return c;
		case CT_NC: return !c;
		case CT_Z: return z;
		case CT_NZ: return !z;
		}
	}
	else
	{
		GBE_ERROR("CPU context doesn't exist");
	}

	return false;
}

void cpu::setFlags(std::weak_ptr<cpuContext> ctx, char z, char n, char h, char c)
{
	if (auto context = ctx.lock())
	{
		setZeroFlag(context, z);

		setSubtractFlag(context, n);

		setHalfCarryFlag(context, h);

		setCarryFlag(context, c);
	}
	else
	{
		GBE_ERROR("CPU context doesn't exist");
	}
}

void cpu::setZeroFlag(std::weak_ptr<cpuContext> ctx, char z)
{
	if (auto context = ctx.lock())
	{
		if (z != -1) 
		{
			utility::setBitTo(context->registers.f, 7, z);
		}
		else
		{
			GBE_ERROR("Zero Flag Error");
		}
	}
	else
	{
		GBE_ERROR("CPU context doesn't exist");
	}
}

void cpu::setSubtractFlag(std::weak_ptr<cpuContext> ctx, char n)
{
	if (auto context = ctx.lock())
	{
		if (n != -1)
		{
			utility::setBitTo(context->registers.f, 6, n);
		}
		else
		{
			GBE_ERROR("Subtract Flag Error");
		}
	}
	else
	{
		GBE_ERROR("CPU context doesn't exist");
	}
}

void cpu::setHalfCarryFlag(std::weak_ptr<cpuContext> ctx, char h)
{
	if (auto context = ctx.lock())
	{
		if (h != -1)
		{
			utility::setBitTo(context->registers.f, 5, h);
		}
		else
		{
			GBE_ERROR("Half Carry Flag Error");
		}
	}
	else
	{
		GBE_ERROR("CPU context doesn't exist");
	}
}

void cpu::setCarryFlag(std::weak_ptr<cpuContext> ctx, char c)
{
	if (auto context = ctx.lock())
	{
		if (c != -1)
		{
			utility::setBitTo(context->registers.f, 4, c);
		}
		else
		{
			GBE_ERROR("Carry Flag Error");
		}
	}
	else
	{
		GBE_ERROR("CPU context doesn't exist");
	}
}

void cpu::isFlagSet()
{
}

