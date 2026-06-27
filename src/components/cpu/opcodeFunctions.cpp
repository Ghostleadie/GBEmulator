//
// Created by Jack_ on 08/09/2025.
//

#include "opcodeFunctions.h"

#include <iostream>

#include "cpu.h"
#include "../../emulator.h"
#include "../../components/bus.h"
#include "../../Utility/utility.h"
#include <unordered_map>

void AdcCommand::execute(cpu& m_cpu)
{
	LOG_TRACE("Running ADC Command");
	const uint16_t u = m_cpu.getFetchedData();
	const uint16_t a = m_cpu.getRegisters()->a;
	const uint16_t c = utility::checkBit(m_cpu.getRegisters()->f, 4);

	m_cpu.getRegisters()->a = (a + u + c) & 0xFF;

	m_cpu.setFlags(m_cpu.getRegisters()->a == 0, 0, (a & 0xF) + (u & 0xF) + c > 0xF, a + u + c > 0xFF);
}

void Add8BitCommand::execute(cpu& m_cpu)
{
	LOG_TRACE("Running ADD 8 Bit Command");
	const uint8_t reg1Val = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) & 0xFF;
	const uint8_t fetchedVal = m_cpu.getFetchedData() & 0xFF;
	const uint16_t result = reg1Val + fetchedVal;

	const int z = (result & 0xFF) == 0;
	const int h = (reg1Val & 0x0F) + (fetchedVal & 0x0F) > 0x0F;
	const int c = result > 0xFF;

	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, result & 0xFF);
	m_cpu.setFlags(z, 0, h, c);
}

void Add16BitCommand::execute(cpu& m_cpu)
{
	const uint16_t reg1Val = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1);
	const uint16_t fetchedVal = m_cpu.getFetchedData();
	const uint32_t result = reg1Val + fetchedVal;

	m_cpu.getClock()->cycles(1);

	const int h = (reg1Val & 0x0FFF) + (fetchedVal & 0x0FFF) >= 0x1000;
	const uint32_t n = static_cast<uint32_t>(reg1Val) + static_cast<uint32_t>(fetchedVal);
	const int c = n >= 0x10000;

	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, result & 0xFFFF);
	m_cpu.setFlags(-1, 0, h, c);
}

void AddSPCommand::execute(cpu& m_cpu)
{
	const uint16_t sp = m_cpu.readRegister(RT_SP);
	const int8_t offset = static_cast<int8_t>(m_cpu.getFetchedData() & 0xFF);
	const uint16_t result = sp + offset;

	const int h = ((sp & 0xF) + (offset & 0xF)) > 0x0F;
	const int c = ((sp & 0xFF) + (offset & 0xFF)) > 0xFF;

	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, result & 0xFFFF);
	m_cpu.setFlags(0, 0, h, c);
}

void AndCommand::execute(cpu& m_cpu)
{
	m_cpu.getRegisters()->a &= m_cpu.getFetchedData();
	m_cpu.setFlags(m_cpu.getRegisters()->a == 0, 0, 1, 0);
}

void CallCommand::execute(cpu& m_cpu)
{
	if (m_cpu.checkConditionFlags())
	{
		m_cpu.getClock()->cycles(2);
		m_cpu.pushStack16(m_cpu.getRegisters()->pc);

		m_cpu.getRegisters()->pc = m_cpu.getFetchedData();
		m_cpu.getClock()->cycles(1);
	}
}

void CcfCommand::execute(cpu& m_cpu)
{
	m_cpu.setFlags(-1, 0, 0, utility::checkBit(m_cpu.getRegisters()->f, 4) ^ 1);
}

void CbCommand::execute(cpu& m_cpu)
{
	/*const extendedCBOpcode cbOp = getCBOpcode((m_cpu.getFetchedData() >> 6) & 0b11);
	uint8_t value = static_cast<uint8_t>(m_cpu.readRegister(cbOp.reg));
	uint8_t bit = (m_cpu.getFetchedData() >> 3) & 0b111;
	if (cbOp.reg == RT_HL)
	{
		m_cpu.getBus().lock()->write(m_cpu.readRegister(RT_HL), value);
	}

	m_cpu.getClock()->cycles(1);
	if (cbOp.reg == RT_HL)
	{
		m_cpu.getClock()->cycles(2);
	}

	switch (cbOp.type)
	{
		case OP_BIT:
		{
			m_cpu.setFlags(!(value & (1 << bit)), 0, 1, -1);
			return;
		}
		case OP_RES:
		{
			value &= static_cast<uint8_t>(~(1u << bit));
			m_cpu.writeRegister(cbOp.reg, value);
			return;
		}
		case OP_SET:
		{
			value |= static_cast<uint8_t>(1u << bit);
			m_cpu.writeRegister(cbOp.reg, value);
			return;
		}
		case OP_RLC:
		{
			bool isCSet = false;
			uint8_t result = (value << 1) & 0xFF;

			if ((value & (1 << 7)) != 0) {
				result |= 1;
				isCSet = true;
			}

			m_cpu.writeRegister(cbOp.reg, result);
			m_cpu.setFlags(result == 0, 0, 0, isCSet);
			return;
		}
		case OP_RRC:
		{
			uint8_t oldValue = value;
			value >>= 1;
			value |= (oldValue << 7);

			m_cpu.writeRegister(cbOp.reg, value);
			m_cpu.setFlags(!value, 0, 0, oldValue & 1);
			return;
		}
		case OP_RL:
		{
			uint8_t oldValue = value;
			value <<= 1;
			value |= utility::checkBit(m_cpu.getRegisters()->f, 4);

			m_cpu.writeRegister(cbOp.reg, value);
			m_cpu.setFlags(!value, 0, 0, !!(oldValue & 0x80));
			return;
		}
		case OP_RR:
		{
			uint8_t oldValue = value;
			value >>= 1;
			value |= (utility::checkBit(m_cpu.getRegisters()->f, 4) << 7);

			m_cpu.writeRegister(cbOp.reg, value);
			m_cpu.setFlags(!value, 0, 0, oldValue & 1);
			return;
		}
		case OP_SLA:
		{
			uint8_t oldValue = value;
			value <<= 1;

			m_cpu.writeRegister(cbOp.reg, value);
			m_cpu.setFlags(!value, 0, 0, !!(oldValue & 0x80));
			return;
		}
		case OP_SRA:
		{
			uint8_t shifted = static_cast<int8_t>(value >> 1);
			m_cpu.writeRegister(cbOp.reg, shifted);
			m_cpu.setFlags(!shifted, 0, 0, value & 1);
			return;
		}
		case OP_SWAP:
		{
			value = ((value & 0xF0) >> 4) | ((value & 0x0F) << 4);
			m_cpu.writeRegister(cbOp.reg, value);
			m_cpu.setFlags(value == 0, 0, 0, 0);
			return;
		}
		case OP_SRL:
		{
			uint8_t shifted = value >> 1;
			m_cpu.writeRegister(cbOp.reg, shifted);
			m_cpu.setFlags(!shifted, 0, 0, value & 1);
			return;
		}
		default:
		{
			LOG_ERROR("Invalid CB Opcode");
		};*/

		const extendedCBOpcode cbOp = getCBOpcode(m_cpu.getFetchedData());

		// Load the operand properly (register or (HL))
		uint8_t value;
		const bool operandIsMemHL = (cbOp.reg == RT_HL);

		if (operandIsMemHL) {
			value = m_cpu.getBus().lock()->read(m_cpu.readRegister(RT_HL));
		} else {
			value = static_cast<uint8_t>(m_cpu.readRegister(cbOp.reg));
		}

		m_cpu.getClock()->cycles(1);
		if (operandIsMemHL) {
			m_cpu.getClock()->cycles(2);
		}

		const uint8_t bit = static_cast<uint8_t>((m_cpu.getFetchedData() >> 3) & 0b111);

		auto writeBack = [&](uint8_t v) {
			if (operandIsMemHL) {
				m_cpu.getBus().lock()->write(m_cpu.readRegister(RT_HL), v);
			} else {
				m_cpu.writeRegister(cbOp.reg, v);
			}
		};

		switch (cbOp.type)
		{
			case OP_BIT:
				m_cpu.setFlags(!(value & (1u << bit)), 0, 1, -1);
				return;

			case OP_RES:
				value &= static_cast<uint8_t>(~(1u << bit));
				writeBack(value);
				return;

			case OP_SET:
				// BUGFIX: was `value |= ~(1 << bit)` which forces 0xFF
				value |= static_cast<uint8_t>(1u << bit);
				writeBack(value);
				return;

			case OP_RLC: {
				const bool c = (value & 0x80u) != 0;
				uint8_t result = static_cast<uint8_t>((value << 1) | (c ? 1u : 0u));
				writeBack(result);
				m_cpu.setFlags(result == 0, 0, 0, c);
				return;
			}

			case OP_RRC: {
				const uint8_t old = value;
				value = static_cast<uint8_t>((value >> 1) | (old << 7));
				writeBack(value);
				m_cpu.setFlags(value == 0, 0, 0, (old & 1u) != 0);
				return;
			}

			case OP_RL: {
				const uint8_t old = value;
				value = static_cast<uint8_t>((value << 1) | (utility::checkBit(m_cpu.getRegisters()->f, 4)));
				writeBack(value);
				m_cpu.setFlags(value == 0, 0, 0, (old & 0x80u) != 0);
				return;
			}

			case OP_RR: {
				const uint8_t old = value;
				value = static_cast<uint8_t>((value >> 1) | (utility::checkBit(m_cpu.getRegisters()->f, 4) << 7));
				writeBack(value);
				m_cpu.setFlags(value == 0, 0, 0, (old & 1u) != 0);
				return;
			}

			case OP_SLA: {
				const uint8_t old = value;
				value = static_cast<uint8_t>(value << 1);
				writeBack(value);
				m_cpu.setFlags(value == 0, 0, 0, (old & 0x80u) != 0);
				return;
			}

			case OP_SRA: {
				const uint8_t old = value;
				// Keep MSB
				value = static_cast<uint8_t>((value & 0x80u) | (value >> 1));
				writeBack(value);
				m_cpu.setFlags(value == 0, 0, 0, (old & 1u) != 0);
				return;
			}

			case OP_SWAP: {
				value = static_cast<uint8_t>(((value & 0xF0u) >> 4) | ((value & 0x0Fu) << 4));
				writeBack(value);
				m_cpu.setFlags(value == 0, 0, 0, 0);
				return;
			}

			case OP_SRL: {
				const uint8_t old = value;
				value = static_cast<uint8_t>(value >> 1);
				writeBack(value);
				m_cpu.setFlags(value == 0, 0, 0, (old & 1u) != 0);
				return;
			}

			default:
				LOG_ERROR("Invalid CB Opcode");
				return;
		}
}
//}

void CplCommand::execute(cpu& m_cpu)
{
	m_cpu.getRegisters()->a = ~m_cpu.getRegisters()->a;
	m_cpu.setFlags(-1, 1, 1, -1);
}

void CpCommand::execute(cpu& m_cpu)
{
	const int n = static_cast<int>(m_cpu.getRegisters()->a) - static_cast<int>(m_cpu.getFetchedData());

	m_cpu.setFlags(n == 0, 1, (static_cast<int>(m_cpu.getRegisters()->a) & 0x0F) - (static_cast<int>(m_cpu.getFetchedData()) & 0x0F) < 0, n < 0);
}

void DaaCommand::execute(cpu& m_cpu)
{
	uint8_t u = 0;
	int fullCarry = 0;

	if (utility::checkBit(m_cpu.getRegisters()->f, 5) || (!utility::checkBit(m_cpu.getRegisters()->f, 6) && (m_cpu.getRegisters()->a & 0xF) > 9)) {
		u = 0x06;
	}

	if (utility::checkBit(m_cpu.getRegisters()->f, 4) || (!utility::checkBit(m_cpu.getRegisters()->f, 6) && m_cpu.getRegisters()->a > 0x99)) {
		u |= 0x60;
		fullCarry = 1;
	}

	m_cpu.getRegisters()->a += utility::checkBit(m_cpu.getRegisters()->f, 6) ? -u : u;

	m_cpu.setFlags(m_cpu.getRegisters()->a == 0, -1, 0, fullCarry);
}

void Dec16BitCommand::execute(cpu& m_cpu)
{
	uint16_t value = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) - 1;
	m_cpu.getClock()->cycles(1);
	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, value);
}

void Dec8BitCommand::execute(cpu& m_cpu)
{
	uint16_t value = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) - 1;

	if (m_cpu.getCurrentOpcodeData().reg1 == RT_HL && m_cpu.getCurrentOpcodeData().mode == AM_MR) {
		value = m_cpu.getBus().lock()->read(m_cpu.readRegister(RT_HL)) - 1;
		m_cpu.getBus().lock()->write(m_cpu.readRegister(RT_HL), value);
	} else {
		m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, value);
		value = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1);
	}

	if ((m_cpu.getCurrentOpcode() & 0x03) == 0x03) {
		return;
	}

	m_cpu.setFlags(value == 0, 1, (value & 0x0F) == 0x0F, -1);
}

void DiCommand::execute(cpu& m_cpu)
{
	m_cpu.setMasterInterruptEnabled(false);
}

void EiCommand::execute(cpu& m_cpu)
{
	m_cpu.setEnablingIME(true);
}

void HaltCommand::execute(cpu& m_cpu)
{
	m_cpu.setHalted(true);
}

void Inc8BitCommand::execute(cpu& m_cpu)
{
	uint16_t oldValue, value;

	if (m_cpu.getCurrentOpcodeData().reg1 == RT_HL && m_cpu.getCurrentOpcodeData().mode == AM_MR) {
		oldValue = m_cpu.getBus().lock()->read(m_cpu.readRegister(RT_HL));
		value = (oldValue + 1) & 0xFF;
		m_cpu.getBus().lock()->write(m_cpu.readRegister(RT_HL), value);
	} else {
		oldValue = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1);
		value = (oldValue + 1) & 0xFF;
		m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, value);
	}

	if ((m_cpu.getCurrentOpcode() & 0x03) == 0x03) {
		return;
	}

	int h = ((oldValue & 0x0F) + 1) > 0x0F;
	m_cpu.setFlags(value == 0, 0, h, -1);
}

void Inc16BitCommand::execute(cpu& m_cpu)
{
	uint16_t oldValue, value;
	if (m_cpu.getCurrentOpcodeData().reg1 == RT_HL && m_cpu.getCurrentOpcodeData().mode == AM_MR)
	{
		// Memory-mapped case
		oldValue = m_cpu.getBus().lock()->read(m_cpu.readRegister(RT_HL));
		value = (oldValue + 1) & 0xFF;
		m_cpu.getBus().lock()->write(m_cpu.readRegister(RT_HL), value);

		int h = ((oldValue & 0x0F) + 1) > 0x0F;
		m_cpu.setFlags(value == 0, 0, h, -1);
		return;
	}
	else
	{
		value = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) + 1;
		m_cpu.getClock()->cycles(1);

		m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, value);
	}
}

void JpCommand::execute(cpu& m_cpu)
{
	if (m_cpu.checkConditionFlags())
	{
		m_cpu.getRegisters()->pc = m_cpu.getFetchedData();
		m_cpu.getClock()->cycles(1);
	}
}

void JphlCommand::execute(cpu& m_cpu)
{
	m_cpu.getRegisters()->pc = m_cpu.readRegister(RT_HL);
	m_cpu.getClock()->cycles(1);
}

void JrCommand::execute(cpu& m_cpu)
{
	const int8_t rel = static_cast<int8_t>(m_cpu.getFetchedData() & 0xFF);
	const uint16_t address  = m_cpu.getRegisters()->pc + rel;

	bool shouldJump = m_cpu.checkConditionFlags();

	//LOG_INFO("JR: opcode=0x{:02X}, rel={}, target_addr=0x{:04X}, should_jump={}, pc_before=0x{:04X}",
	//		 m_cpu.getCurrentOpcode(), rel, address, shouldJump, m_cpu.getRegisters()->pc);
	if (shouldJump)
	{
		//LOG_INFO("JR: Taking jump to 0x{:04X}", address);
		m_cpu.getRegisters()->pc = address;
		m_cpu.getClock()->cycles(1);
	}
	else
	{
		//LOG_INFO("JR: Not taking jump, continuing to next instruction");
	}

}

void Ld8BitCommand::execute(cpu& m_cpu)
{
	//check if writing to memory or a register
	if (m_cpu.getDestinationIsMemory()) {
		
		m_cpu.getBus().lock()->write(m_cpu.getMemoryDestination(), static_cast<uint8_t>(m_cpu.getFetchedData()));
		
		m_cpu.getClock()->cycles(1);

		return;
	}
	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, m_cpu.getFetchedData());
}
//check if writing to memort or a register
void Ld16BitCommand::execute(cpu& m_cpu)
{
	if (m_cpu.getDestinationIsMemory()) {
		m_cpu.getClock()->cycles(1);
		m_cpu.getBus().lock()->write16(m_cpu.getMemoryDestination(), m_cpu.getFetchedData());
		m_cpu.getClock()->cycles(1);

		return;
	}
	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, m_cpu.getFetchedData());
}

void LdSpecialCommand::execute(cpu& m_cpu)
{
	const int8_t offset = static_cast<int8_t>(m_cpu.getFetchedData() & 0xFF);
	const uint16_t sp = m_cpu.readRegister(RT_SP);
	const uint16_t result = static_cast<uint16_t>(sp + offset);

	const uint8_t offset_u8 = static_cast<uint8_t>(offset);
	const int h = ((sp & 0xF) + (offset_u8 & 0x0F)) > 0x0F;
	const int c = ((sp & 0xFF) + (offset_u8 & 0xFF)) > 0xFF;

	m_cpu.writeRegister(RT_HL, result);
	m_cpu.setFlags(0, 0, h, c);
	m_cpu.getClock()->cycles(1);
}

void LdhCommand::execute(cpu& m_cpu)
{
	/*// Support both LDH (a8),A / LDH A,(a8) and LD (FF00+C),A / LD A,(FF00+C)
	const bool usesC = (m_cpu.getCurrentOpcodeData().reg1 == RT_C) || (m_cpu.getCurrentOpcodeData().reg2 == RT_C);
	const uint16_t offset = usesC ? (m_cpu.readRegister(RT_C) & 0xFF) : (m_cpu.getFetchedData() & 0xFF);
	const uint16_t addr = static_cast<uint16_t>(0xFF00 + offset);

	// For 0xE0: LDH (a8),A - store A to memory
	// For 0xF0: LDH A,(a8) - load from memory to A
	if (m_cpu.getCurrentOpcode() == 0xF0) {
		// Load from memory to A
		const uint8_t v = m_cpu.getBus().lock()->read(addr);
		m_cpu.writeRegister(RT_A, v);
	}
	else
	{
		// Store A to memory
		m_cpu.getBus().lock()->write(addr, m_cpu.getRegisters()->a);
	}*/

	if (m_cpu.getPC() == 0xCB92)
	{
		LOG_INFO("LDH");
	}

	if (m_cpu.getCurrentOpcodeData().reg1 == RT_A) {
		m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, m_cpu.getBus().lock()->read(0xFF00 | m_cpu.getFetchedData()));
	} else {
		m_cpu.getBus().lock()->write(m_cpu.getMemoryDestination(), m_cpu.getRegisters()->a);
	}


	m_cpu.getClock()->cycles(1);

}

void NopCommand::execute(cpu& m_cpu)
{

}

void OrCommand::execute(cpu& m_cpu)
{
	m_cpu.getRegisters()->a |= m_cpu.getFetchedData() & 0xFF;
	m_cpu.setFlags(m_cpu.getRegisters()->a == 0, 0, 0, 0);
}

void PopCommand::execute(cpu& m_cpu)
{
	//using popStack instead of popStack16 to account for cycles
	const uint16_t low = m_cpu.popStack();
	m_cpu.getClock()->cycles(1);
	const uint16_t high = m_cpu.popStack();
	m_cpu.getClock()->cycles(1);
	const uint16_t value = (high << 8) | low;

	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, value);
}

void PopSpecialCommand::execute(cpu& m_cpu)
{
	//using popStack instead of popStack16 to account for cycles
	const uint16_t low = m_cpu.popStack();
	m_cpu.getClock()->cycles(1);
	const uint16_t high = m_cpu.popStack();
	m_cpu.getClock()->cycles(1);
	const uint16_t value = (high << 8) | low;
	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, value & 0xFFF0);
}

void PushCommand::execute(cpu& m_cpu)
{
	const uint16_t high = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) >> 8 & 0xFF;
	m_cpu.getClock()->cycles(1);
	m_cpu.pushStack(high);

	const uint16_t low = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) & 0xFF;
	m_cpu.getClock()->cycles(1);
	m_cpu.pushStack(low);

	m_cpu.getClock()->cycles(1);
}

void RetCommand::execute(cpu& m_cpu)
{
	if (m_cpu.getCurrentOpcodeData().cond != CT_NONE)
	{
		m_cpu.getClock()->cycles(1);
	}

	if (m_cpu.checkConditionFlags())
	{
		//using popStack instead of popStack16 to account for cycles
		const uint16_t low = m_cpu.popStack();
		m_cpu.getClock()->cycles(1);
		const uint16_t high = m_cpu.popStack();
		m_cpu.getClock()->cycles(1);

		const uint16_t value = (high << 8) | low;
		m_cpu.getRegisters()->pc = value;
		m_cpu.getClock()->cycles(1);
	}

}

void RetiCommand::execute(cpu& m_cpu)
{
	m_cpu.setMasterInterruptEnabled(true);
	if (m_cpu.getCurrentOpcodeData().cond != CT_NONE)
	{
		m_cpu.getClock()->cycles(1);
	}

	if (m_cpu.checkConditionFlags())
	{
		//using popStack instead of popStack16 to account for cycles
		const uint16_t low = m_cpu.popStack();
		m_cpu.getClock()->cycles(1);
		const uint16_t high = m_cpu.popStack();
		m_cpu.getClock()->cycles(1);

		const uint16_t value = (high << 8) | low;
		m_cpu.getRegisters()->pc = value;
		m_cpu.getClock()->cycles(1);
	}
}

void RlaCommand::execute(cpu& m_cpu)
{
	const uint8_t u = m_cpu.getRegisters()->a;
	const uint8_t cf = utility::checkBit(m_cpu.getRegisters()->f, 4);
	const uint8_t c = (u >> 7) & 1;

	m_cpu.getRegisters()->a = (u << 1) | cf;
	m_cpu.setFlags(0, 0, 0, c);
}

void RlcaCommand::execute(cpu& m_cpu)
{
	uint8_t u = m_cpu.getRegisters()->a;
	const bool c = (u >> 7) & 1;
	u = (u << 1) | static_cast<int>(c);
	m_cpu.getRegisters()->a = u;

	m_cpu.setFlags(0, 0, 0, c);
}

void RraCommand::execute(cpu& m_cpu)
{
	const uint8_t carry = utility::checkBit(m_cpu.getRegisters()->f, 4);
	const uint8_t new_c = m_cpu.getRegisters()->a & 1;

	m_cpu.getRegisters()->a >>= 1;
	m_cpu.getRegisters()->a |= (carry << 7);

	m_cpu.setFlags( 0, 0, 0, new_c);
}

void RrcaCommand::execute(cpu& m_cpu)
{
	const uint8_t b = m_cpu.getRegisters()->a & 1;
	m_cpu.getRegisters()->a >>= 1;
	m_cpu.getRegisters()->a |= (b << 7);

	m_cpu.setFlags( 0, 0, 0, b);
}

void RstCommand::execute(cpu& m_cpu)
{
	if (m_cpu.checkConditionFlags())
	{
		m_cpu.getClock()->cycles(1);
		m_cpu.pushStack16(m_cpu.getRegisters()->pc);
		m_cpu.getClock()->cycles(1);
		m_cpu.getRegisters()->pc = m_cpu.getCurrentOpcodeData().param;

	}
}

void SbcCommand::execute(cpu& m_cpu)
{
	/*const uint16_t value = m_cpu.getFetchedData() + utility::checkBit(m_cpu.getRegisters()->f, 4);

	const int16_t z = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) - value == 0;
	const int16_t h = static_cast<int16_t>(m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) & 0x0F) - static_cast<int16_t>(m_cpu.getFetchedData() & 0x0F) - static_cast<int>(utility::checkBit(m_cpu.getRegisters()->f, 4)) < 0;
	const int16_t c = static_cast<int16_t>(m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1)) - static_cast<int16_t>(m_cpu.getFetchedData()) - static_cast<int>(utility::checkBit(m_cpu.getRegisters()->f, 4)) < 0;

	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, value);
	m_cpu.setFlags(z,1,h,c);*/

	const uint8_t a  = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) & 0xFF;
	const uint8_t u  = m_cpu.getFetchedData() & 0xFF;
	const uint8_t cy = utility::checkBit(m_cpu.getRegisters()->f, 4);

	const uint16_t sub = static_cast<uint16_t>(u) + static_cast<uint16_t>(cy);
	const uint16_t res16 = static_cast<uint16_t>(a) - sub;
	const uint8_t result = static_cast<uint8_t>(res16 & 0xFF);

	const int z = (result == 0);
	const int h = (a & 0x0F) < ((u & 0x0F) + cy);
	const int c = a < sub;

	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, result);
	m_cpu.setFlags(z, 1, h, c);
}

void ScfCommand::execute(cpu& m_cpu)
{
	m_cpu.setFlags(-1,0,0,1);
}

void StopCommand::execute(cpu& m_cpu)
{
	// Read and discard the next byte (part of STOP instruction format)
	m_cpu.getBus().lock()->read(m_cpu.getRegisters()->pc);
	m_cpu.getRegisters()->pc++;

	// Set CPU to stopped state (you may need to add this method to cpu class)
	m_cpu.setHalted(true);
}

void SubCommand::execute(cpu& m_cpu)
{
	/*const uint16_t value = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) - m_cpu.getFetchedData();

	const int16_t z = value == 0;
	const int16_t h = static_cast<int16_t>(m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) & 0x0F) - static_cast<int16_t>(m_cpu.getFetchedData() & 0x0F) < 0;
	const int16_t c = static_cast<int16_t>(m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1)) - static_cast<int16_t>(m_cpu.getFetchedData()) < 0;

	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) - value);
	m_cpu.setFlags(z,1,h,c);*/

	const uint8_t a = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) & 0xFF;
	const uint8_t u = m_cpu.getFetchedData() & 0xFF;
	const uint16_t res16 = static_cast<uint16_t>(a) - static_cast<uint16_t>(u);
	const uint8_t result = static_cast<uint8_t>(res16 & 0xFF);

	const int z = (result == 0);
	const int h = (a & 0x0F) < (u & 0x0F);
	const int c = a < u;

	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, result);
	m_cpu.setFlags(z, 1, h, c);
}

void XorCommand::execute(cpu& m_cpu)
{
	m_cpu.getRegisters()->a ^= m_cpu.getFetchedData() & 0xFF;
	m_cpu.setFlags(m_cpu.getRegisters()->a == 0, 0, 0, 0);
}

std::unique_ptr<OpcodeCommand> OpcodeCommandFactory::createCommand(const opcode& opcode)
{

    switch (opcode.type) {
        case OP_NONE:    // No operation
        {
	        return nullptr;
        }
        case OP_NOP:     // NOP: No operation
        {
	        return std::make_unique<NopCommand>();
        }
    	case OP_LD:     // LD 8-bit: Load 8-bit value
        {
        	if (opcode.reg1 == RT_HL && opcode.reg2 == RT_SP)
        	{
        		return std::make_unique<LdSpecialCommand>();
        	}
        	if (opcode.reg2 >= RT_AF)
        	{
        		return std::make_unique<Ld16BitCommand>();
        	}
	        return std::make_unique<Ld8BitCommand>();
        }
        case OP_INC:    // INC 8-bit: Increment 8-bit register
        {
        	if (opcode.reg1 == RT_HL && opcode.mode == AM_MR)
        	{
        		return std::make_unique<Inc8BitCommand>();
        	}
        	if ((opcode.reg1 == RT_BC || opcode.reg1 == RT_DE || opcode.reg1 == RT_HL || opcode.reg1 == RT_SP))
        	{
        		return std::make_unique<Inc16BitCommand>();
        	}
	        return std::make_unique<Inc8BitCommand>();
        }
        case OP_DEC:    // DEC 8-bit: Decrement 8-bit register
        {
        	if (opcode.reg2 >= RT_AF/* || opcode.reg1 >= RT_AF*/)
        	{
        		return std::make_unique<Dec16BitCommand>();
        	}
	        return std::make_unique<Dec8BitCommand>();
        }
        case OP_ADD:    // ADD 8-bit: Add 8-bit value
        {
	        if (opcode.reg1 == RT_SP)
	        {
	        	return std::make_unique<AddSPCommand>();
	        }
        	// Only use 16-bit ADD when reg1 is HL and we're adding 16-bit registers
        	if (opcode.reg1 == RT_HL && (opcode.reg2 == RT_BC || opcode.reg2 == RT_DE || opcode.reg2 == RT_HL || opcode.reg2 == RT_SP))
        	{
        		return std::make_unique<Add16BitCommand>();
        	}
        	return std::make_unique<Add8BitCommand>();
        }
        case OP_ADC:     // ADC: Add with carry
        {
	        return std::make_unique<AdcCommand>();
        }
        case OP_SUB:     // SUB: Subtract
        {
	        return std::make_unique<SubCommand>();
        }
        case OP_SBC:     // SBC: Subtract with carry
        {
	        return std::make_unique<SbcCommand>();
        }
        case OP_AND:     // AND: Logical AND
        {
	        return std::make_unique<AndCommand>();
        }
        case OP_XOR:     // XOR: Logical XOR
        {
	        return std::make_unique<XorCommand>();
        }
        case OP_OR:      // OR: Logical OR
        {
	        return std::make_unique<OrCommand>();
        }
        case OP_CP:      // CP: Compare
        {
	        return std::make_unique<CpCommand>();
        }
        case OP_JP:      // JP: Jump
        {
	        return std::make_unique<JpCommand>();
        }
        case OP_JPHL:    // JP (HL): Jump to address in HL
        {
	        return std::make_unique<JphlCommand>();
        }
        case OP_JR:      // JR: Relative jump
        {
	        return std::make_unique<JrCommand>();
        }
        case OP_CALL:    // CALL: Call subroutine
        {
	        return std::make_unique<CallCommand>();
        }
        case OP_RET:     // RET: Return from subroutine
        {
	        return std::make_unique<RetCommand>();
        }
        case OP_RETI:    // RETI: Return and enable interrupts
        {
	        return std::make_unique<RetiCommand>();
        }
        case OP_RST:     // RST: Restart
        {
	        return std::make_unique<RstCommand>();
        }
        case OP_PUSH:    // PUSH: Push to stack
        {
	        return std::make_unique<PushCommand>();
        }
        case OP_POP:     // POP: Pop from stack
        {
        	if (opcode.reg1 == RT_AF)
			{
				return std::make_unique<PopSpecialCommand>();
			}
	        return std::make_unique<PopCommand>();
        }
        case OP_DAA:     // DAA: Decimal adjust accumulator
        {
	        return std::make_unique<DaaCommand>();
        }
        case OP_CPL:     // CPL: Complement accumulator
        {
	        return std::make_unique<CplCommand>();
        }
        case OP_SCF:     // SCF: Set carry flag
        {
	        return std::make_unique<ScfCommand>();
        }
        case OP_CCF:     // CCF: Complement carry flag
        {
	        return std::make_unique<CcfCommand>();
        }
        case OP_DI:      // DI: Disable interrupts
        {
	        return std::make_unique<DiCommand>();
        }
        case OP_EI:      // EI: Enable interrupts
        {
	        return std::make_unique<EiCommand>();
        }
        case OP_HALT:    // HALT: Halt CPU
        {
	        return std::make_unique<HaltCommand>();
        }
        case OP_STOP:    // STOP: Stop CPU
        {
	        return std::make_unique<StopCommand>();
        }
        case OP_CB:      // CB: Prefix for bit operations
        {
	        return std::make_unique<CbCommand>();
        }
        case OP_LDH:     // LDH: Load high memory
        {
	        return std::make_unique<LdhCommand>();
        }
    	case OP_RRA:
        {
        	return std::make_unique<RraCommand>();
        }
    	case OP_RRCA:
        {
	        return std::make_unique<RrcaCommand>();
        }
    	case OP_RLCA:
        {
	        return std::make_unique<RlcaCommand>();
		}
    	case OP_RLA:
        {
	        return std::make_unique<RlaCommand>();
        }
        default:
        {
	        LOG_ERROR("Execution Failed: Unimplemented Opcode");
        	return nullptr;
        }
    }
}