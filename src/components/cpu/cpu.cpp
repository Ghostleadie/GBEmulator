//
// Created by Jack_ on 31/08/2025.
//

#include "cpu.h"
#include "../bus.h"
#include <cstdint>
#include "../../emulator.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "../../Utility/utility.h"

void cpu::init()
{
	registers.pc = 0x100;
	registers.a = 0x01;
}

void cpu::fetchOpcode()
{
	currentOpcode = m_bus->read(registers.pc++);
	currentOpcodeData = getOpcode(currentOpcode);
}

void cpu::fetchData()
{
	memoryDestination = 0;
	destinationIsMemory = false;

	//based on addressing mode
	switch (currentOpcodeData.mode)
	{
		case AM_IMP:
		{
			break;
		}
		case AM_R_D16:
		{
			//can only read 8 bytes so we have to do it in 2 parts for 16 bytes
			uint16_t lowValue = m_bus->read(registers.pc);
			//emulator::cycles(1);

			uint16_t highValue = m_bus->read(registers.pc + 1);
			emulator::cycles(1);

			//fetched data equal to low or high shifted into the high position
			fetchedData = lowValue | (highValue << 8);

			registers.pc += 2;
			break;
		}
		case AM_R_R:
		{
			fetchedData = readRegister(currentOpcodeData.reg2);
			break;
		}
		case AM_MR_R:
		{
			fetchedData = readRegister(currentOpcodeData.reg2);
			memoryDestination = readRegister(currentOpcodeData.reg1);
			destinationIsMemory = true;

			if (currentOpcodeData.reg1 == RT_C)
			{
				memoryDestination |= 0xFF00;
			}
			break;
		}
		case AM_R:
		{
			fetchedData = readRegister(currentOpcodeData.reg1);
			//return;
			break;
		}
		case AM_R_D8:
		{
			fetchedData = m_bus->read(registers.pc);
			//emulator::cycles(1);
			registers.pc++;
			break;
		}
		case AM_R_MR:
		{
			uint16_t address = readRegister(currentOpcodeData.reg2);

			if (currentOpcodeData.reg2 == RT_C)
			{
				address |= 0xFF00;
			}

			fetchedData = m_bus->read(address);
			emulator::cycles(1);
			break;
		}
		case AM_R_HLI:
		{
			fetchedData = m_bus->read(readRegister(currentOpcodeData.reg2));
			emulator::cycles(1);
			writeRegister(RT_HL, static_cast<uint16_t>(readRegister(RT_HL) + 1));
			break;
		}
		case AM_R_HLD:
		{
			fetchedData = m_bus->read(readRegister(currentOpcodeData.reg2));
			emulator::cycles(1);
			writeRegister(RT_HL, static_cast<uint16_t>(readRegister(RT_HL) - 1));
			break;
		}
		case AM_HLI_R:
		{
			fetchedData = readRegister(currentOpcodeData.reg2);
			memoryDestination = readRegister(currentOpcodeData.reg1);
			destinationIsMemory = true;
			writeRegister(RT_HL, static_cast<uint16_t>(readRegister(RT_HL) + 1));
			break;
		}
		case AM_HLD_R:
		{
			fetchedData = readRegister(currentOpcodeData.reg2);
			memoryDestination = readRegister(currentOpcodeData.reg1);
			destinationIsMemory = true;
			writeRegister(RT_HL, static_cast<uint16_t>(readRegister(RT_HL) - 1));
			break;
		}
		case AM_R_A8:
		{
			fetchedData = m_bus->read(registers.pc);
			emulator::cycles(1);
			registers.pc++;
			break;
		}
		case AM_A8_R:
		{
			memoryDestination = m_bus->read(registers.pc) | 0xFF00;
			destinationIsMemory = true;
			emulator::cycles(1);
			registers.pc++;
			break;
		}
		case AM_HL_SPR:
		{
			fetchedData = m_bus->read(registers.pc);
			emulator::cycles(1);
			registers.pc++;
			break;
		}
		case AM_D16:
		{
			//can only read 8 bytes so we have to do it in 2 parts for 16 bytes
			const uint16_t lowValue = m_bus->read(registers.pc);
			emulator::cycles(1);

			const uint16_t highValue = m_bus->read(registers.pc + 1);
			emulator::cycles(1);

			//fetched data equal to low or high shifted into the high position
			fetchedData = lowValue | (highValue << 8);

			registers.pc += 2;
			break;
		}
		case AM_D8:
		{
			fetchedData = m_bus->read(registers.pc);
			emulator::cycles(1);
			registers.pc++;
			break;
		}
		case AM_D16_R:
		{
			//can only read 8 bytes so we have to do it in 2 parts for 16 bytes
			const uint16_t lowValue = m_bus->read(registers.pc);
			emulator::cycles(1);

			const uint16_t highValue = m_bus->read(registers.pc + 1);
			emulator::cycles(1);

			//fetched data equal to low or high shifted into the high position
			fetchedData = lowValue | (highValue << 8);
			destinationIsMemory = true;

			registers.pc += 2;
			fetchedData = readRegister(currentOpcodeData.reg2);
			break;
		}
		case AM_MR_D8:
		{
			fetchedData = m_bus->read(registers.pc);
			emulator::cycles(1);
			registers.pc++;
			memoryDestination = readRegister(currentOpcodeData.reg2);
			destinationIsMemory = true;
			break;
		}
		case AM_MR:
		{
			memoryDestination = readRegister(currentOpcodeData.reg1);
			destinationIsMemory = true;
			fetchedData = m_bus->read(readRegister(currentOpcodeData.reg1));
			emulator::cycles(1);
			break;
		}
		case AM_A16_R:
		{
			//can only read 8 bytes so we have to do it in 2 parts for 16 bytes
			const uint16_t lowValue = m_bus->read(registers.pc);
			emulator::cycles(1);

			const uint16_t highValue = m_bus->read(registers.pc + 1);
			emulator::cycles(1);

			//fetched data equal to low or high shifted into the high position
			fetchedData = lowValue | (highValue << 8);
			destinationIsMemory = true;

			registers.pc += 2;
			fetchedData = readRegister(currentOpcodeData.reg2);
			break;
		}
		case AM_R_A16:
		{
			//can only read 8 bytes so we have to do it in 2 parts for 16 bytes
			const uint16_t lowValue = m_bus->read(registers.pc);
			emulator::cycles(1);

			const uint16_t highValue = m_bus->read(registers.pc + 1);
			emulator::cycles(1);

			//fetched data equal to low or high shifted into the high position
			uint16_t address = lowValue | (highValue << 8);

			registers.pc += 2;
			fetchedData = m_bus->read(address);
			emulator::cycles(1);
			break;
		}
		default:
		{
			LOG_WARN("Unknown Addressing Mode! instruction mode:{} opcode:{:02X}", static_cast<int>(currentOpcodeData.mode), currentOpcode);
			exit(-7);
		}
	}
}

void cpu::execute(opcode opcode)
{
	LOG_INFO("Executing opcode: {:02X} Program Counter: {:04X}", currentOpcode, registers.pc);
	if (opcode.name == "0xCB")
	{
		//execute CB opcodes
	}
	else
	{
		//execute normal opcodes
	}
}

uint16_t cpu::readRegister(const registryType reg) const
{
	switch (reg)
	{
		case RT_A:
			return registers.a;
		case RT_F:
			return registers.f;
		case RT_B:
			return registers.b;
		case RT_C:
			return registers.c;
		case RT_D:
			return registers.d;
		case RT_E:
			return registers.e;
		case RT_H:
			return registers.h;
		case RT_L:
			return registers.l;
		// 16 bit
		case RT_AF:
			return registers.af;
		case RT_BC:
			return registers.bc;
		case RT_DE:
			return registers.de;
		case RT_HL:
			return registers.hl;
		case RT_PC:
			return registers.pc;
			break;
		case RT_SP:
			return registers.sp;
			break;
		default:
			return 0;
	}
}


void cpu::emulateCycle()
{
	if (steppingMode == true)
	{
		if (stepComplete == false)
		{
			fetchOpcode();
			fetchData();
			execute(currentOpcodeData);
			stepComplete = true;
		}
	}
	else
	{
		fetchOpcode();
		fetchData();
		if (currentOpcodeData.execute != nullptr)
		{
			LOG_INFO("Executing opcode: {:02X} Program Counter: {:04X}", currentOpcode, registers.pc);
			currentOpcodeData.execute(*this);
		}
		else
		{
			LOG_ERROR("Unimplemented opcode: {:02X} at PC: {:04X}", currentOpcode, registers.pc - 1);
		}
		//execute(currentOpcodeData);
	}

}

void cpu::writeRegister(const registryType reg, const uint16_t& value)
{
	switch (reg)
	{
		//8 bit
		case RT_A:
			registers.a = value & 0xFF;
			break;
		case RT_F:
			registers.f = value & 0xFF;
			break;
		case RT_B:
			registers.b = value & 0xFF;
			break;
		case RT_C:
			registers.c = value & 0xFF;
			break;
		case RT_D:
			registers.d = value & 0xFF;
			break;
		case RT_E:
			registers.e = value & 0xFF;
			break;
		case RT_H:
			registers.h = value & 0xFF;
			break;
		case RT_L:
			registers.l = value & 0xFF;
			break;
		//16 bit
		case RT_AF:
			registers.af = reverse(value);
			break;
		case RT_BC:
			registers.bc = reverse(value);
			break;
		case RT_DE:
			registers.de = reverse(value);
			break;
		case RT_HL:
			registers.hl = reverse(value);
			break;

		case RT_PC:
			registers.pc = value;
			break;
		case RT_SP:
			registers.sp = value;
			break;
		case RT_NONE:
			break;
		default:
			LOG_ERROR("Invalid register");
	}
}

uint8_t cpu::getIERegister()
{
	return interruptEnableRegister;
}

void cpu::setIERegister(const uint8_t value)
{
	interruptEnableRegister = value;
}

opcode cpu::getCurrentOpcodeData() const
{
	return currentOpcodeData;
}

bool cpu::checkConditionFlags() const
{
	const bool z = utility::checkBit(registers.f, 7);
	const bool c = utility::checkBit(registers.f, 4);
	switch (currentOpcodeData.cond)
	{
		case CT_NONE:
			return true;
		case CT_C:
			return c;
		case CT_NC:
			return !c;
		case CT_Z:
			return z;
		case CT_NZ:
			return !z;
	}

	return false;
}

void cpu::setFlags(const uint8_t z, const uint8_t n, const uint8_t h, const uint8_t c) const
{
		setZeroFlag(z);

		setSubtractFlag(n);

		setHalfCarryFlag(h);

		setCarryFlag(c);
}

void cpu::setZeroFlag(const uint8_t z) const
{
		if (z != -1)
		{
			utility::setBitTo(registers.f, 7, z);
		}
		else
		{
			LOG_ERROR("Zero Flag Error");
		}
}

void cpu::setSubtractFlag(const uint8_t n) const
{
		if (n != -1)
		{
			utility::setBitTo(registers.f, 6, n);
		}
		else
		{
			LOG_ERROR("Subtract Flag Error");
		}
}

void cpu::setHalfCarryFlag(const uint8_t h) const
{
		if (h != -1)
		{
			utility::setBitTo(registers.f, 5, h);
		}
		else
		{
			LOG_ERROR("Half Carry Flag Error");
		}
}

void cpu::setCarryFlag(const uint8_t c) const
{
		if (c != -1)
		{
			utility::setBitTo(registers.f, 4, c);
		}
		else
		{
			LOG_ERROR("Carry Flag Error");
		}
}

void cpu::isFlagSet()
{
}