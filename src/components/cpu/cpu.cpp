//
// Created by Jack_ on 31/08/2025.
//

#include "cpu.h"
#include "../bus.h"
#include <cstdint>
#include <algorithm>
#include "../../Utility/SerialPortDebugger.h"
#include "../../Utility/utility.h"

// Scoped opcode enums; using enum keeps enumerators usable unqualified here.
using enum opcodeType;
using enum addressMode;
using enum registryType;
using enum conditionType;

#ifdef ENABLE_TESTING
#pragma message("ENABLE_TESTING is defined")
#else
#pragma message("ENABLE_TESTING is NOT defined")
#endif

void cpu::init()
{
	m_registers.pc = 0x100;
	m_registers.a = 0x01;
	m_registers.f = 0xB0;
	m_registers.b = 0x00;
	m_registers.c = 0x13;
	m_registers.d = 0x00;
	m_registers.e = 0xD8;
	m_registers.h = 0x01;
	m_registers.l = 0x4D;
	m_registers.sp = 0xFFFE;

	masterInterruptEnabled = false;
	enablingIME = false;


	// Force disable boot ROM
	//m_bus->write(0xFF50, 0x01);

	//LOG_INFO("CPU initialized, PC=0x{:04X}, boot ROM disabled", m_registers.pc);
}

void cpu::fetchOpcode()
{
	currentOpcode = m_bus->read(m_registers.pc++);
	getClock()->cycles(1); // the opcode fetch is itself a 1 M-cycle memory read
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
			uint16_t lowValue = m_bus->read(m_registers.pc);
			getClock()->cycles(1);

			uint16_t highValue = m_bus->read(m_registers.pc + 1);
			getClock()->cycles(1);

			//fetched data equal to low and high shifted into the high position
			fetchedData = lowValue | (highValue << 8);

			m_registers.pc += 2;
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
			fetchedData = m_bus->read(m_registers.pc);
			getClock()->cycles(1);
			m_registers.pc++;
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
			getClock()->cycles(1);
			break;
		}
		case AM_R_HLI: // Register, HL increment: Load value from memory at HL into register, then increment HL
		{
			fetchedData = m_bus->read(readRegister(currentOpcodeData.reg2));
			getClock()->cycles(1);
			writeRegister(RT_HL, static_cast<uint16_t>(readRegister(RT_HL) + 1));
			break;
		}
		case AM_R_HLD: // Register, HL decrement: Load value from memory at HL into register, then decrement HL
		{
			fetchedData = m_bus->read(readRegister(currentOpcodeData.reg2));
			getClock()->cycles(1);
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
			fetchedData = m_bus->read(m_registers.pc);
			getClock()->cycles(1);
			m_registers.pc++;
			break;
		}
		case AM_A8_R: // 8-bit address, Register: Write register value to high memory at 0xFF00 + 8-bit immediate
		{
			memoryDestination = m_bus->read(m_registers.pc) | 0xFF00;
			destinationIsMemory = true;
			getClock()->cycles(1);
			m_registers.pc++;
			break;
		}
		case AM_HL_SPR: // HL, SP plus 8-bit signed immediate: Add signed immediate to SP and store result in HL
		{
			fetchedData = m_bus->read(m_registers.pc);
			getClock()->cycles(1);
			m_registers.pc++;
			break;
		}
		case AM_I16: // 16-bit immediate: Load 16-bit immediate value
		{
			//can only read 8 bytes so we have to do it in 2 parts for 16 bytes
			const uint16_t lowValue = m_bus->read(m_registers.pc);
			getClock()->cycles(1);

			const uint16_t highValue = m_bus->read(m_registers.pc + 1);
			getClock()->cycles(1);

			//fetched data equal to low or high shifted into the high position
			fetchedData = lowValue | (highValue << 8);

			m_registers.pc += 2;
			break;
		}
		case AM_I8: // 8-bit immediate: Load 8-bit immediate value
		{
			fetchedData = m_bus->read(m_registers.pc);
			getClock()->cycles(1);
			m_registers.pc++;
			break;
		}
		case AM_I16_R: // 16-bit immediate, Register: Load 16-bit immediate into register
		{
			//can only read 8 bytes so we have to do it in 2 parts for 16 bytes
			const uint16_t lowValue = m_bus->read(m_registers.pc);
			getClock()->cycles(1);

			const uint16_t highValue = m_bus->read(m_registers.pc + 1);
			getClock()->cycles(1);

			//fetched data equal to low or high shifted into the high position
			memoryDestination = lowValue | (highValue << 8);
			destinationIsMemory = true;

			m_registers.pc += 2;
			fetchedData = readRegister(currentOpcodeData.reg2);
			break;
		}
		case AM_MR_D8: // Memory (register), 8-bit immediate: Write 8-bit immediate to memory address in register
		{
			fetchedData = m_bus->read(m_registers.pc);
			getClock()->cycles(1);
			m_registers.pc++;
			memoryDestination = readRegister(currentOpcodeData.reg1);
			destinationIsMemory = true;
			break;
		}
		case AM_MR: // Memory (register): Read value from memory address in register
		{
			memoryDestination = readRegister(currentOpcodeData.reg1);
			destinationIsMemory = true;
			fetchedData = m_bus->read(readRegister(currentOpcodeData.reg1));
			getClock()->cycles(1);
			break;
		}
		case AM_A16_R: // 16-bit address, Register: Write register value to memory at 16-bit immediate address
		{
			//can only read 8 bytes so we have to do it in 2 parts for 16 bytes
			const uint16_t lowValue = m_bus->read(m_registers.pc);
			getClock()->cycles(1);

			const uint16_t highValue = m_bus->read(m_registers.pc + 1);
			getClock()->cycles(1);

			//fetched data equal to low or high shifted into the high position
			memoryDestination = lowValue | (highValue << 8);
			destinationIsMemory = true;

			m_registers.pc += 2;
			fetchedData = readRegister(currentOpcodeData.reg2);
			break;
		}
		case AM_R_A16: // Register, 16-bit address: Load value from memory at 16-bit immediate address into register
		{
			//can only read 8 bytes so we have to do it in 2 parts for 16 bytes
			const uint16_t lowValue = m_bus->read(m_registers.pc);
			getClock()->cycles(1);

			const uint16_t highValue = m_bus->read(m_registers.pc + 1);
			getClock()->cycles(1);

			//fetched data equal to low or high shifted into the high position
			uint16_t address = lowValue | (highValue << 8);

			m_registers.pc += 2;
			fetchedData = m_bus->read(address);
			getClock()->cycles(1);
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
			return m_registers.a;
		case RT_F:
			return m_registers.f;
		case RT_B:
			return m_registers.b;
		case RT_C:
			return m_registers.c;
		case RT_D:
			return m_registers.d;
		case RT_E:
			return m_registers.e;
		case RT_H:
			return m_registers.h;
		case RT_L:
			return m_registers.l;
		// 16 bit
		case RT_AF:
			return m_registers.af;
		case RT_BC:
			return m_registers.bc;
		case RT_DE:
			return m_registers.de;
		case RT_HL:
			return m_registers.hl;
		case RT_PC:
			return m_registers.pc;
		case RT_SP:
			return m_registers.sp;
		default:
			return 0;
	}
}


void cpu::traceInstruction(uint16_t pc)
{
	// Gameboy Doctor line: CPU state at the START of the instruction at `pc`.
	// PC/PCMEM use the pre-fetch pc; F is the raw flags byte.
	CPU_TRACE("A:{:02X} F:{:02X} B:{:02X} C:{:02X} D:{:02X} E:{:02X} H:{:02X} L:{:02X} SP:{:04X} PC:{:04X} PCMEM:{:02X},{:02X},{:02X},{:02X}",
		m_registers.a, m_registers.f, m_registers.b, m_registers.c, m_registers.d, m_registers.e, m_registers.h, m_registers.l,
		m_registers.sp, pc,
		m_bus->read(pc), m_bus->read(static_cast<uint16_t>(pc + 1)),
		m_bus->read(static_cast<uint16_t>(pc + 2)), m_bus->read(static_cast<uint16_t>(pc + 3)));
}


void cpu::recordOpcode(const uint8_t opcodeByte)
{
	// Single producer (emulation thread). Off by default, so normal play does no work here.
	if (!m_recordOpcodeHistory.load(std::memory_order_relaxed))
	{
		return;
	}
	const uint64_t count = m_opcodeHistoryCount.load(std::memory_order_relaxed);
	m_opcodeHistory[count % OpcodeHistoryCapacity].store(opcodeByte, std::memory_order_relaxed);
	// Publish the incremented count last (release) so a reader that observes it also sees the byte above.
	m_opcodeHistoryCount.store(count + 1, std::memory_order_release);
}

std::vector<std::string> cpu::getOpcodesHistory() const
{
	const uint64_t count = m_opcodeHistoryCount.load(std::memory_order_acquire);
	const uint64_t n = std::min<uint64_t>(count, OpcodeHistoryCapacity);
	std::vector<std::string> out;
	out.reserve(static_cast<std::size_t>(n));
	// Oldest first: the n most recent entries live at logical indices [count - n, count). A slot may
	// be overwritten by the producer mid-copy; benign tearing is acceptable for a debug view.
	for (uint64_t i = count - n; i < count; ++i)
	{
		const uint8_t op = m_opcodeHistory[i % OpcodeHistoryCapacity].load(std::memory_order_relaxed);
		out.emplace_back(getOpcodeName(op));
	}
	return out;
}


void cpu::emulateCycle()
{
	if (!halted)
	{
		uint16_t pc_before = m_registers.pc;
		if (steppingMode == true)
		{
			if (stepComplete == false)
			{
				if (traceLogging)
				{
					traceInstruction(pc_before);
				}
				fetchOpcode();
				fetchData();
				if (currentOpcodeData.execute != nullptr)
				{
					//LOG_INFO("Executing {}: {:02X} Program Counter: {:04X}", currentOpcodeData.name, currentOpcode, pc_before);
					currentOpcodeData.execute(*this);
					recordOpcode(currentOpcode);
					/*LOG_INFO("A:{:02X} F:{:02X} B:{:02X} C:{:02X} D:{:02X} E:{:02X} H:{:02X} L:{:02X} SP:{:04X} PC:{:04X} PCMEM:{:02X},{:02X},{:02X},{:02X}",
																	m_registers.a, m_registers.f, m_registers.b, m_registers.c, m_registers.d, m_registers.e, m_registers.h, m_registers.l, m_registers.sp, pc_before,
																	m_bus->read(pc_before), m_bus->read(pc_before+1), m_bus->read(pc_before+2), m_bus->read(pc_before+3));*/
				}
				else
				{
					LOG_ERROR("Unimplemented {}: {:02X} at PC: {:04X}", currentOpcodeData.name, currentOpcode, m_registers.pc - 1);
				}
				stepComplete = true;
			}
		}
		else
		{
			if (m_serialDebugger.update(*m_bus))
			{
				m_serialDebugger.print();
			}
			if (traceLogging)
			{
				traceInstruction(pc_before);
			}
			fetchOpcode();
			fetchData();
			if (currentOpcodeData.execute != nullptr)
			{

				//LOG_INFO("Executing {}: {:02X} Program Counter: {:04X}", currentOpcodeData.name, currentOpcode, pc_before);
				currentOpcodeData.execute(*this);
				//LOG_INFO("A: {:02X} F:{:02X} B:{:02X} C:{:02X} D:{:02X} E:{:02X} H:{:02X} L:{:02X} SP:{} PC:{} PCMEM:{},{},{},{}= {} PC=0x{:04X}, opcode=0x{:02X}, A=0x{:02X}, BC=0x{:04X}, DE=0x{:04X}, HL=0x{:04X}",
				//		 cycleCount, m_registers.pc, m_bus->read(m_registers.pc),
				//		 m_registers.a, m_registers.bc, m_registers.de, m_registers.hl);
				recordOpcode(currentOpcode);
				/*LOG_INFO("A:{:02X} F:{:02X} B:{:02X} C:{:02X} D:{:02X} E:{:02X} H:{:02X} L:{:02X} SP:{:04X} PC:{:04X} PCMEM:{:02X},{:02X},{:02X},{:02X}",
																	m_registers.a, m_registers.f, m_registers.b, m_registers.c, m_registers.d, m_registers.e, m_registers.h, m_registers.l, m_registers.sp, pc_before,
																	m_bus->read(pc_before), m_bus->read(pc_before+1), m_bus->read(pc_before+2), m_bus->read(pc_before+3));
*/
			}
			else
			{
				LOG_ERROR("Unimplemented {}: {:02X} at PC: {:04X}", currentOpcodeData.name, currentOpcode, m_registers.pc - 1);
			}
		}
	}
	else
	{
		getClock()->cycles(1);

		// Exit HALT only when an *enabled* interrupt is pending (IE & IF), not on
		// any IF bit. A flagged-but-disabled source -- e.g. the joypad on a game
		// that never enables its interrupt -- leaves its IF bit set permanently;
		// waking on that would stop HALT from ever blocking and wreck frame pacing.
		if (m_bus->read(0xFF0F) & m_bus->read(0xFFFF) & 0x1F)
		{
			halted = false;
		}
	}

	if (masterInterruptEnabled)
	{
		handleInterrupts();
		enablingIME = false;
	}

	if (enablingIME)
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
			m_registers.a = value & 0xFF;
			break;
		case RT_F:
			m_registers.f = value & 0xFF;
			break;
		case RT_B:
			m_registers.b = value & 0xFF;
			break;
		case RT_C:
			m_registers.c = value & 0xFF;
			break;
		case RT_D:
			m_registers.d = value & 0xFF;
			break;
		case RT_E:
			m_registers.e = value & 0xFF;
			break;
		case RT_H:
			m_registers.h = value & 0xFF;
			break;
		case RT_L:
			m_registers.l = value & 0xFF;
			break;
		//16 bit
		case RT_AF:
			m_registers.af = value;
			break;
		case RT_BC:
			m_registers.bc = value;
			break;
		case RT_DE:
			m_registers.de = value;
			break;
		case RT_HL:
			m_registers.hl = value;
			break;

		case RT_PC:
			m_registers.pc = value;
			break;
		case RT_SP:
			m_registers.sp = value;
			break;
		case RT_NONE:
			break;
		default:
			LOG_ERROR("Invalid register");
	}
}

void cpu::handleInterrupts()
{
	if (interruptCheck(0x40, interruptTypes::INT_VBLANK))
	{
		LOG_TRACE("INT_VBLANK interrupt");
		return;
	}
	if (interruptCheck(0x48, interruptTypes::INT_LCD_STAT))
	{
		LOG_TRACE("INT_LCD_STAT interrupt");
		return;
	}
	if (interruptCheck(0x50, interruptTypes::INT_TIMER))
	{
		LOG_TRACE("INT_TIMER interrupt");
		return;
	}
	if (interruptCheck(0x58, interruptTypes::INT_SERIAL))
	{
		LOG_TRACE("INT_SERIAL interrupt");
		return;
	}
	if (interruptCheck(0x60, interruptTypes::INT_JOYPAD))
	{
		LOG_TRACE("INT_JOYPAD interrupt");
		return;
	}
}

void cpu::handleInterrupt(uint16_t address)
{
	pushStack16(m_registers.pc);
	m_registers.pc = address;
}

bool cpu::interruptCheck(uint16_t address, interruptTypes type)
{
	const uint8_t interruptFlags = m_bus->read(0xFF0F);
	const uint8_t interruptEnable = m_bus->read(0xFFFF);
	const auto bit = static_cast<uint8_t>(type);
	if (interruptFlags & bit && interruptEnable & bit)
	{
		handleInterrupt(address);
		m_bus->write(0xFF0F, static_cast<uint8_t>(interruptFlags & ~bit));
		halted = false;
		masterInterruptEnabled = false;
		return true;
	}
	return false;
}

void cpu::requestInterrupt(interruptTypes type)
{
	m_bus->write(0xFF0F, static_cast<uint8_t>(m_bus->read(0xFF0F) | static_cast<uint8_t>(type)));
}

opcode cpu::getCurrentOpcodeData() const
{
	return currentOpcodeData;
}

bool cpu::checkConditionFlags()
{
	const bool z = utility::checkBit(m_registers.f, 7);
	const bool c = utility::checkBit(m_registers.f, 4);

	//LOG_INFO("checkConditionFlags: f_register=0x{:02X}, z_bit_7={}, c_bit_4={}, cond={}",
	//			 m_registers.f, z, c, static_cast<int>(currentOpcodeData.cond));

	switch (currentOpcodeData.cond)
	{
		case CT_NONE:
			//LOG_INFO("CT_NONE: returning true");
			return true;
		case CT_C:
			//LOG_INFO("CT_C: returning {}", c);
			return c;
		case CT_NC:
			//LOG_INFO("CT_NC: returning {}", !c);
			return !c;
		case CT_Z:
			//LOG_INFO("CT_Z: returning {}", z);
			return z;
		case CT_NZ:
			//LOG_INFO("CT_NZ: returning {} (z={})", !z, z);
			return !z;
		default:
			LOG_ERROR("Unknown condition type: {}", static_cast<int>(currentOpcodeData.cond));
	}
	//
	return false;
}

void cpu::setFlags(const int8_t z, const int8_t n, const int8_t h, const int8_t c)
{
		setZeroFlag(z);

		setSubtractFlag(n);

		setHalfCarryFlag(h);

		setCarryFlag(c);
}

void cpu::setZeroFlag(const int8_t z)
{
	if (z != -1)
	{
		//LOG_INFO("BEFORE setBitTo: m_registers.f=0x{:02X}, setting bit 7 to {}", m_registers.f, z);
		utility::setBitTo(m_registers.f, 7, z);
		//LOG_INFO("AFTER setBitTo: m_registers.f=0x{:02X}", m_registers.f);
	}
}

void cpu::setSubtractFlag(const int8_t n)
{
	if (n != -1)
	{
		utility::setBitTo(m_registers.f, 6, n);
	}
}

void cpu::setHalfCarryFlag(const int8_t h)
{
	if (h != -1)
	{
		utility::setBitTo(m_registers.f, 5, h);
	}
}

void cpu::setCarryFlag(const int8_t c)
{
	if (c != -1)
	{
		utility::setBitTo(m_registers.f, 4, c);
	}
}

void cpu::pushStack(const uint8_t value)
{
	getRegisters()->sp--;
	m_bus->write(getRegisters()->sp, value);
}

uint8_t cpu::popStack()
{
	getRegisters()->sp++;
	return m_bus->read(getRegisters()->sp - 1);
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

bool cpu::isZeroFlagSet() const {
	return utility::checkBit(m_registers.f, 7);
}

bool cpu::isSubtractFlagSet() const {
	return utility::checkBit(m_registers.f, 6);
}

bool cpu::isHalfCarryFlagSet() const {
	return utility::checkBit(m_registers.f, 5);
}

bool cpu::isCarryFlagSet() const {
	return utility::checkBit(m_registers.f, 4);
}

void cpu::execSingleInstructionWithOpcode(uint8_t opcode)
{
	currentOpcode = opcode;
	currentOpcodeData = getOpcode(currentOpcode);
	fetchData();
	if (currentOpcodeData.execute != nullptr)
	{
		currentOpcodeData.execute(*this);
	}
}

void cpu::execSingleInstruction()
{
	fetchOpcode();
	fetchData();
	if (currentOpcodeData.execute != nullptr)
	{
		currentOpcodeData.execute(*this);
	}
}