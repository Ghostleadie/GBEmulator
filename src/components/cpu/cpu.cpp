//
// Created by Jack_ on 31/08/2025.
//

#include "cpu.h"
#include "../bus.h"
#include <cstdint>
#include "../../emulator.h"
#include "../../Utility/dbg.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "../../Utility/utility.h"

void cpu::init()
{
	registers.pc = 0x100;
	registers.a = 0x01;
	registers.f = 0xB0;
	registers.b = 0x00;
	registers.c = 0x13;
	registers.d = 0x00;
	registers.e = 0xD8;
	registers.h = 0x01;
	registers.l = 0x4D;
	registers.sp = 0xFFFE;

	// Force disable boot ROM
	m_bus->write(0xFF50, 0x01);

	LOG_INFO("CPU initialized, PC=0x{:04X}, boot ROM disabled", registers.pc);
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


	/*
	How to get the address mode
	1.	Find the Opcode: Locate the opcode in the table by hex value e.g 0x4.
	2.	Check the Mnemonic: Look at the instruction mnemonic and its operands e.g: LD BC, n16.
	3.	Identify Operand Types:
		Registers (A, B, C, D, E, H, L, HL, SP, etc.)
		Immediate values (n for 8-bit, nn for 16-bit)
		Memory addresses ((HL), (C), (a8), (a16))
		Match to Addressing Mode: Use your emulator's addressing mode enum (e.g., AM_R_R, AM_R_D8, AM_MR_R, etc.) and match based on operand types:
			LD A, B → Register to register (AM_R_R)
			LD A, (HL) → Register, memory (register) (AM_R_MR)
			LD A, n → Register, 8-bit immediate (AM_R_D8)
			LD (a16), A → 16-bit address, register (AM_A16_R)
			LD (C), A → Memory (register), register (AM_MR_R)
			LD HL, SP+n → HL, SP plus 8-bit signed immediate (AM_HL_SPR)
			INC HL → Implied (AM_IMP)
	*/

	//based on addressing mode
	switch (currentOpcodeData.mode)
	{
		case AM_IMP: // Implied: No operand, operation uses implicit registers or flags
		{
			break;
		}
		case AM_R_D16: // Register, 16-bit immediate: Load 16-bit immediate into register
		{
			//can only read 8 bytes so we have to do it in 2 parts for 16 bytes
			uint16_t lowValue = m_bus->read(registers.pc);
			emulator::cycles(1);

			uint16_t highValue = m_bus->read(registers.pc + 1);
			emulator::cycles(1);

			//fetched data equal to low and high shifted into the high position
			fetchedData = lowValue | (highValue << 8);

			registers.pc += 2;
			break;
		}
		case AM_R_R: // Register to register: Transfer value between registers
		{
			fetchedData = readRegister(currentOpcodeData.reg2);
			break;
		}
		case AM_MR_R: // Memory (register), register: Write register value to memory address in register
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
		case AM_R: // Register: Read value from register
		{
			fetchedData = readRegister(currentOpcodeData.reg1);
			break;
		}
		case AM_R_D8: // Register, 8-bit immediate: Load 8-bit immediate into register
		{
			fetchedData = m_bus->read(registers.pc);
			emulator::cycles(1);
			registers.pc++;
			break;
		}
		case AM_R_MR: // Register, Memory (register): Load value from memory address in register into register
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
		case AM_R_HLI: // Register, HL increment: Load value from memory at HL into register, then increment HL
		{
			fetchedData = m_bus->read(readRegister(currentOpcodeData.reg2));
			emulator::cycles(1);
			writeRegister(RT_HL, static_cast<uint16_t>(readRegister(RT_HL) + 1));
			break;
		}
		case AM_R_HLD: // Register, HL decrement: Load value from memory at HL into register, then decrement HL
		{
			fetchedData = m_bus->read(readRegister(currentOpcodeData.reg2));
			emulator::cycles(1);
			writeRegister(RT_HL, static_cast<uint16_t>(readRegister(RT_HL) - 1));
			break;
		}
		case AM_HLI_R: // HL increment, Register: Write register value to memory at HL, then increment HL
		{
			fetchedData = readRegister(currentOpcodeData.reg2);
			memoryDestination = readRegister(currentOpcodeData.reg1);
			destinationIsMemory = true;
			writeRegister(RT_HL, static_cast<uint16_t>(readRegister(RT_HL) + 1));
			break;
		}
		case AM_HLD_R: // HL decrement, Register: Write register value to memory at HL, then decrement HL
		{
			fetchedData = readRegister(currentOpcodeData.reg2);
			memoryDestination = readRegister(currentOpcodeData.reg1);
			destinationIsMemory = true;
			writeRegister(RT_HL, static_cast<uint16_t>(readRegister(RT_HL) - 1));
			break;
		}
		case AM_R_A8: // Register, 8-bit address: Load value from high memory at 0xFF00 + 8-bit immediate into register
		{
			fetchedData = m_bus->read(registers.pc);
			emulator::cycles(1);
			registers.pc++;
			break;
		}
		case AM_A8_R: // 8-bit address, Register: Write register value to high memory at 0xFF00 + 8-bit immediate
		{
			memoryDestination = m_bus->read(registers.pc) | 0xFF00;
			destinationIsMemory = true;
			emulator::cycles(1);
			registers.pc++;
			break;
		}
		case AM_HL_SPR: // HL, SP plus 8-bit signed immediate: Add signed immediate to SP and store result in HL
		{
			fetchedData = m_bus->read(registers.pc);
			emulator::cycles(1);
			registers.pc++;
			break;
		}
		case AM_D16: // 16-bit immediate: Load 16-bit immediate value
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
		case AM_D8: // 8-bit immediate: Load 8-bit immediate value
		{
			fetchedData = m_bus->read(registers.pc);
			emulator::cycles(1);
			registers.pc++;
			break;
		}
		case AM_D16_R: // 16-bit immediate, Register: Load 16-bit immediate into register
		{
			//can only read 8 bytes so we have to do it in 2 parts for 16 bytes
			const uint16_t lowValue = m_bus->read(registers.pc);
			emulator::cycles(1);

			const uint16_t highValue = m_bus->read(registers.pc + 1);
			emulator::cycles(1);

			//fetched data equal to low or high shifted into the high position
			memoryDestination = lowValue | (highValue << 8);
			destinationIsMemory = true;

			registers.pc += 2;
			fetchedData = readRegister(currentOpcodeData.reg2);
			break;
		}
		case AM_MR_D8: // Memory (register), 8-bit immediate: Write 8-bit immediate to memory address in register
		{
			fetchedData = m_bus->read(registers.pc);
			emulator::cycles(1);
			registers.pc++;
			memoryDestination = readRegister(currentOpcodeData.reg2);
			destinationIsMemory = true;
			break;
		}
		case AM_MR: // Memory (register): Read value from memory address in register
		{
			memoryDestination = readRegister(currentOpcodeData.reg1);
			destinationIsMemory = true;
			fetchedData = m_bus->read(readRegister(currentOpcodeData.reg1));
			emulator::cycles(1);
			break;
		}
		case AM_A16_R: // 16-bit address, Register: Write register value to memory at 16-bit immediate address
		{
			//can only read 8 bytes so we have to do it in 2 parts for 16 bytes
			const uint16_t lowValue = m_bus->read(registers.pc);
			emulator::cycles(1);

			const uint16_t highValue = m_bus->read(registers.pc + 1);
			emulator::cycles(1);

			//fetched data equal to low or high shifted into the high position
			memoryDestination = lowValue | (highValue << 8);
			destinationIsMemory = true;

			registers.pc += 2;
			fetchedData = readRegister(currentOpcodeData.reg2);
			break;
		}
		case AM_R_A16: // Register, 16-bit address: Load value from memory at 16-bit immediate address into register
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
		case RT_SP:
			return registers.sp;
		default:
			return 0;
	}
}


void cpu::emulateCycle()
{
	// Test memory access
	//LOG_INFO("Testing serial register access...");
	//m_bus->write(0xFF01, 'A');  // Write test character
	//m_bus->write(0xFF02, 0x81); // Set transfer start bit
	//LOG_INFO("Wrote test data to serial registers");

	if (registers.pc == 0x0216) {
		uint8_t opcode = m_bus->read(registers.pc);
		uint8_t data1 = m_bus->read(registers.pc + 1);
		uint8_t data2 = m_bus->read(registers.pc + 2);
		LOG_INFO("At 0x0216: opcode=0x{:02X}, data1=0x{:02X}, data2=0x{:02X}",
				 opcode, data1, data2);
	}

	static uint16_t lastPC = 0;
	if (registers.pc != lastPC) {
		// Check if this instruction might write to serial
		uint8_t opcode = m_bus->read(registers.pc);
		if (opcode == 0xEA || opcode == 0x02 || opcode == 0x12) { // LD (nn),A or LD (BC),A or LD (DE),A
			uint16_t addr = 0;
			if (opcode == 0xEA) {
				addr = m_bus->read(registers.pc + 1) | (m_bus->read(registers.pc + 2) << 8);
			}
			if (addr == 0xFF01 || addr == 0xFF02) {
				LOG_INFO("Potential serial write at PC=0x{:04X}, opcode=0x{:02X}, addr=0x{:04X}",
						 registers.pc, opcode, addr);
			}
		}
		lastPC = registers.pc;
	}

	if (!halted)
	{

		if (steppingMode == true)
		{

			if (stepComplete == false)
			{
				fetchOpcode();
				fetchData();
				dbg::dbgUpdate(m_bus);
				dbg::dbgPrint();
				if (currentOpcodeData.execute != nullptr)
				{
					LOG_INFO("Executing {}: {:02X} Program Counter: {:04X}", currentOpcodeData.name, currentOpcode, registers.pc);
					currentOpcodeData.execute(*this);
					opcodesHistory.push_back(currentOpcodeData);
				}
				else
				{
					LOG_ERROR("Unimplemented {}: {:02X} at PC: {:04X}", currentOpcodeData.name, currentOpcode, registers.pc - 1);
				}
				stepComplete = true;
			}
		}
		else
		{
			static uint64_t cycleCount = 0;
			cycleCount++;

			if (cycleCount % 250000 == 0) {
				LOG_INFO("Cycle {}: PC=0x{:04X}, opcode=0x{:02X}, A=0x{:02X}, BC=0x{:04X}, DE=0x{:04X}, HL=0x{:04X}",
						 cycleCount, registers.pc, m_bus->read(registers.pc),
						 registers.a, registers.bc, registers.de, registers.hl);
			}
			fetchOpcode();
			fetchData();
			dbg::dbgUpdate(m_bus);
			dbg::dbgPrint();
			if (currentOpcodeData.execute != nullptr)
			{

				LOG_INFO("Executing {}: {:02X} Program Counter: {:04X}", currentOpcodeData.name, currentOpcode, registers.pc);
				currentOpcodeData.execute(*this);
				opcodesHistory.push_back(currentOpcodeData);
			}
			else
			{
				LOG_ERROR("Unimplemented {}: {:02X} at PC: {:04X}", currentOpcodeData.name, currentOpcode, registers.pc - 1);
			}
		}
	}
	else
	{
		emulator::cycles(1);

		if (interruptFlags)
		{
			halted = false;
		}
	}

	if (masterInterruptEnabled)
	{
		enablingIME = false;
	}
	else if (enablingIME)
	{
		masterInterruptEnabled = true;
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
			registers.af = value;
			break;
		case RT_BC:
			registers.bc = value;
			break;
		case RT_DE:
			registers.de = value;
			break;
		case RT_HL:
			registers.hl = value;
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

void cpu::handleInterrupts()
{
	if (interruptCheck(0x40, INT_VBLANK))
	{
		LOG_TRACE("INT_BLANK interrupt");
	}
	else if (interruptCheck(0x48, INT_LCD_STAT))
	{
		LOG_TRACE("INT_LCD_STAT interrupt");
	}
	else if (interruptCheck(0x50, INT_TIMER))
	{
		LOG_TRACE("INT_TIMER interrupt");
	}
	else if (interruptCheck(0x58, INT_SERIAL))
	{
		LOG_TRACE("INT_SERIAL interrupt");
	}
	else if (interruptCheck(0x60, INT_JOYPAD))
	{
		LOG_TRACE("INT_JOYPAD interrupt");
	}
}

void cpu::handleInterrupt(uint16_t address)
{
	pushStack16(registers.pc);
	registers.pc = address;
}

bool cpu::interruptCheck(uint16_t address, interruptTypes type)
{
	if (interruptFlags & type && interruptEnableRegister & type)
	{
		handleInterrupt(address);
		interruptFlags &= ~type;
		halted = false;
		masterInterruptEnabled = false;
		return true;
	}
	return false;
}

uint8_t cpu::getIERegister() const
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

bool cpu::checkConditionFlags()
{
	const bool z = utility::checkBit(registers.f, 7);
	const bool c = utility::checkBit(registers.f, 4);

	LOG_INFO("checkConditionFlags: f_register=0x{:02X}, z_bit_7={}, c_bit_4={}, cond={}",
				 registers.f, z, c, static_cast<int>(currentOpcodeData.cond));

	switch (currentOpcodeData.cond)
	{
		case CT_NONE:
			LOG_INFO("CT_NONE: returning true");
			return true;
		case CT_C:
			LOG_INFO("CT_C: returning {}", c);
			return c;
		case CT_NC:
			LOG_INFO("CT_NC: returning {}", !c);
			return !c;
		case CT_Z:
			LOG_INFO("CT_Z: returning {}", z);
			return z;
		case CT_NZ:
			LOG_INFO("CT_NZ: returning {} (z={})", !z, z);
			return !z;
	}
	LOG_WARN("Unknown condition type: {}", static_cast<int>(currentOpcodeData.cond));
	return false;
}

void cpu::setFlags(const uint8_t z, const uint8_t n, const uint8_t h, const uint8_t c)
{
		setZeroFlag(z);

		setSubtractFlag(n);

		setHalfCarryFlag(h);

		setCarryFlag(c);
}

void cpu::setZeroFlag(const uint8_t z)
{
	if (z != static_cast<uint8_t>(-1))
	{
		LOG_INFO("BEFORE setBitTo: registers.f=0x{:02X}, setting bit 7 to {}", registers.f, z);
		utility::setBitTo(registers.f, 7, z);
		LOG_INFO("AFTER setBitTo: registers.f=0x{:02X}", registers.f);
	}
	else
	{
		LOG_ERROR("Zero Flag Error");
	}
}

void cpu::setSubtractFlag(const uint8_t n)
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

void cpu::setHalfCarryFlag(const uint8_t h)
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

void cpu::setCarryFlag(const uint8_t c)
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

void cpu::pushStack(const uint8_t value)
{
	getRegisters().sp--;
	m_bus->write(getRegisters().sp, value);
}

uint8_t cpu::popStack()
{
	getRegisters().sp++;
	return m_bus->read(getRegisters().sp - 1);
}

void cpu::pushStack16(const uint16_t value)
{
	pushStack((value >> 8) & 0xFF);
	pushStack(value & 0xFF);
}

uint16_t cpu::popStack16()
{
	const uint16_t low = popStack();
	const uint16_t high = popStack();
	return (high << 8) | low;
}

// Returns true if the Zero flag is set
bool cpu::isZeroFlagSet() const {
	return utility::checkBit(registers.f, 7);
}

// Returns true if the Subtract flag is set
bool cpu::isSubtractFlagSet() const {
	return utility::checkBit(registers.f, 6);
}

// Returns true if the Half Carry flag is set
bool cpu::isHalfCarryFlagSet() const {
	return utility::checkBit(registers.f, 5);
}

// Returns true if the Carry flag is set
bool cpu::isCarryFlagSet() const {
	return utility::checkBit(registers.f, 4);
}