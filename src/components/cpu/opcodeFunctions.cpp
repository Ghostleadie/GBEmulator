//
// Created by Jack_ on 08/09/2025.
//

#include "opcodeFunctions.h"
#include "cpu.h"
#include "../../emulator.h"
#include "../../components/bus.h"
#include "../../Utility/utility.h"
#include <unordered_map>

void AdcCommand::execute(cpu& m_cpu)
{
	LOG_TRACE("Running ADC Command");
	const uint16_t u = m_cpu.getFetchedData();
	const uint16_t a = m_cpu.getRegisters().a;
	const uint16_t c = utility::checkBit(m_cpu.getRegisters().f, 4);

	m_cpu.getRegisters().a = (a + u + c) & 0xFF;

	m_cpu.setFlags(m_cpu.getRegisters().a == 0, 0, (a & 0xF) + (u & 0xF) + c > 0xF, a + u + c > 0xFF);
}

void Add8BitCommand::execute(cpu& m_cpu)
{
	LOG_TRACE("Running ADD 8 Bit Command");
	const uint8_t reg1Val = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) & 0xFF;
	const uint8_t fetchedVal = m_cpu.getFetchedData() & 0xFF;
	const uint16_t result = reg1Val + fetchedVal;

	const int z = (result & 0xFF) == 0;
	const int h = (reg1Val & 0xF) + (fetchedVal & 0xF) >= 0x10;
	const int c = result > 0xFF;

	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, result & 0xFFFF);
	m_cpu.setFlags(z, 0, h, c);
}

void Add16BitCommand::execute(cpu& m_cpu)
{
	const uint16_t reg1Val = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1);
	const uint16_t fetchedVal = m_cpu.getFetchedData();
	const uint32_t result = reg1Val + fetchedVal;

	emulator::cycles(1);

	const int h = (reg1Val & 0xFFF) + (fetchedVal & 0xFFF) >= 0x1000;
	const int c = result >= 0x10000;

	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, result & 0xFFFF);
	m_cpu.setFlags(-1, 0, h, c);
}

void AddSPCommand::execute(cpu& m_cpu)
{
	const uint16_t sp = m_cpu.readRegister(RT_SP);
	const int8_t offset = static_cast<int8_t>(m_cpu.getFetchedData() & 0xFF);
	const uint16_t result = sp + offset;

	const int h = (sp & 0xF) + (offset & 0xF) >= 0x10;
	const int c = (sp & 0xFF) + (offset & 0xFF) > 0xFF;

	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, result & 0xFFFF);
	m_cpu.setFlags(0, 0, h, c);
}

void AndCommand::execute(cpu& m_cpu)
{
	m_cpu.getRegisters().a &= m_cpu.getFetchedData();
	m_cpu.setFlags(m_cpu.getRegisters().a == 0, 0, 1, 0);
}

void CallCommand::execute(cpu& m_cpu)
{
	if (m_cpu.checkConditionFlags())
	{
		emulator::cycles(2);
		m_cpu.pushStack16(m_cpu.getRegisters().pc);

		m_cpu.getRegisters().pc = m_cpu.getFetchedData();
		emulator::cycles(1);
	}
}

void CcfCommand::execute(cpu& m_cpu)
{
	m_cpu.setFlags(-1, 0, 0, utility::checkBit(m_cpu.getRegisters().f, 4) ^ 1);
}

void CbCommand::execute(cpu& m_cpu)
{
	const extendedCBOpcode cbOp = getCBOpcode((m_cpu.getFetchedData() >> 6) & 0b11);
	uint8_t value = static_cast<uint8_t>(m_cpu.readRegister(cbOp.reg));
	uint8_t bit = (m_cpu.getFetchedData() >> 3) & 0b111;
	if (cbOp.reg == RT_HL)
	{
		m_cpu.getBus().lock()->write(m_cpu.readRegister(RT_HL), value);
	}

	emulator::cycles(1);
	if (cbOp.reg == RT_HL)
	{
		emulator::cycles(2);
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
			value &= ~(1 << bit);
			m_cpu.writeRegister(cbOp.reg, value);
			return;
		}
		case OP_SET:
		{
			value |= ~(1 << bit);
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
			value |= utility::checkBit(m_cpu.getRegisters().f, 4);

			m_cpu.writeRegister(cbOp.reg, value);
			m_cpu.setFlags(!value, 0, 0, !!(oldValue & 0x80));
			return;
		}
		case OP_RR:
		{
			uint8_t oldValue = value;
			value >>= 1;
			value |= (utility::checkBit(m_cpu.getRegisters().f, 4) << 7);

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
		};
	}
}

void CplCommand::execute(cpu& m_cpu)
{
	m_cpu.getRegisters().a = ~m_cpu.getRegisters().a;
	m_cpu.setFlags(-1, 1, 1, -1);
}

void CpCommand::execute(cpu& m_cpu)
{
	const int n = static_cast<int>(m_cpu.getRegisters().a) - static_cast<int>(m_cpu.getFetchedData());

	m_cpu.setFlags(n == 0, 1, (static_cast<int>(m_cpu.getRegisters().a) & 0x0F) - (static_cast<int>(m_cpu.getFetchedData()) & 0x0F) < 0, n < 0);
}

void DaaCommand::execute(cpu& m_cpu)
{
	uint8_t u = 0;
	int fc = 0;

	if (utility::checkBit(m_cpu.getRegisters().f, 5) || (!utility::checkBit(m_cpu.getRegisters().f, 6) && (m_cpu.getRegisters().a & 0xF) > 9)) {
		u = 6;
	}

	if (utility::checkBit(m_cpu.getRegisters().f, 4) || (!utility::checkBit(m_cpu.getRegisters().f, 6) && m_cpu.getRegisters().a > 0x99)) {
		u |= 0x60;
		fc = 1;
	}

	m_cpu.getRegisters().a += utility::checkBit(m_cpu.getRegisters().f, 6) ? -u : u;

	m_cpu.setFlags(m_cpu.getRegisters().a == 0, -1, 0, fc);
}

void Dec16BitCommand::execute(cpu& m_cpu)
{
	uint16_t value = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) - 1;
	emulator::cycles(1);

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

	m_cpu.setFlags(value == 0, 0, (value & 0x0F) == 0x0F, -1);
}

void Dec8BitCommand::execute(cpu& m_cpu)
{
	uint16_t value = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) - 1;

	// Log C register specifically for debugging
	if (m_cpu.getCurrentOpcodeData().reg1 == RT_C) {
		LOG_INFO("DEC C: old_value=0x{:02X}, new_value=0x{:02X}",
				 value + 1, value & 0xFF);
	}

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

	m_cpu.setFlags(value == 0, 0, (value & 0x0F) == 0x0F, -1);
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

	if (m_cpu.getCurrentOpcodeData().reg1 == RT_HL && m_cpu.getCurrentOpcodeData().mode == AM_MR)
	{
		// Memory-mapped case
		oldValue = m_cpu.getBus().lock()->read(m_cpu.readRegister(RT_HL));
		value = (oldValue + 1) & 0xFF;
		m_cpu.getBus().lock()->write(m_cpu.readRegister(RT_HL), value);
	}
	else
	{
		// Regular register case
		oldValue = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) & 0xFF;
		value = (oldValue + 1) & 0xFF;
		m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, value);
	}

	LOG_INFO("INC: reg={}, old_value=0x{:02X}, new_value=0x{:02X}, zero_flag={}",
			 static_cast<int>(m_cpu.getCurrentOpcodeData().reg1),
			 oldValue,
			 value,
			 value == 0);

	// Set flags for all INC 8-bit operations except INC 16-bit (which have different opcodes)
	m_cpu.setFlags(value == 0, 0, (oldValue & 0x0F) == 0x0F, -1);
	/*uint16_t value = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) + 1;

	LOG_INFO("INC: reg={}, old_value=0x{:02X}, new_value=0x{:02X}, zero_flag={}",
		static_cast<int>(m_cpu.getCurrentOpcodeData().reg1),
		m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1),
		value & 0xFF,
		(value & 0xFF) == 0);

	if (m_cpu.getCurrentOpcodeData().reg1 == RT_HL && m_cpu.getCurrentOpcodeData().mode == AM_MR)
	{
		uint16_t oldValue = m_cpu.getBus().lock()->read(m_cpu.readRegister(RT_HL));
        value = (oldValue + 1) & 0xFF;
		m_cpu.getBus().lock()->write(m_cpu.readRegister(RT_HL), value);
	}
	else
	{
		m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, value);
		value &= 0xFF; // Ensure 8-bit result for flag calculation
	}

	if ((m_cpu.getCurrentOpcode() & 0x03) == 0x03) {
		return;
	}

	m_cpu.setFlags((value & 0xFF) == 0, 0, (value & 0x0F) == 0, -1);*/
}

void Inc16BitCommand::execute(cpu& m_cpu)
{
	uint16_t value = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) + 1;
	emulator::cycles(1);

	if (m_cpu.getCurrentOpcodeData().reg1 == RT_HL && m_cpu.getCurrentOpcodeData().mode == AM_MR) {
		value = m_cpu.getBus().lock()->read(m_cpu.readRegister(RT_HL)) + 1;
		value &= 0xFF;
		m_cpu.getBus().lock()->write(m_cpu.readRegister(RT_HL), value);
	} else {
		m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, value);
		value = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1);
	}

	if ((m_cpu.getCurrentOpcode() & 0x03) == 0x03) {
		return;
	}

	m_cpu.setFlags(value == 0, 0, (value & 0x0F) == 0, -1);
}

void JpCommand::execute(cpu& m_cpu)
{
	if (m_cpu.checkConditionFlags())
	{
		m_cpu.getRegisters().pc = m_cpu.getFetchedData();
		emulator::cycles(1);
	}
}

void JphlCommand::execute(cpu& m_cpu)
{
	LOG_WARN("not implemented");
}

void JrCommand::execute(cpu& m_cpu)
{
	const int8_t rel = static_cast<int8_t>(m_cpu.getFetchedData() & 0xFF);
	const uint16_t address  = m_cpu.getRegisters().pc + rel;

	bool shouldJump = m_cpu.checkConditionFlags();

	LOG_INFO("JR: opcode=0x{:02X}, rel={}, target_addr=0x{:04X}, should_jump={}, pc_before=0x{:04X}",
			 m_cpu.getCurrentOpcode(), rel, address, shouldJump, m_cpu.getRegisters().pc);
	if (shouldJump)
	{
		LOG_INFO("JR: Taking jump to 0x{:04X}", address);
		m_cpu.getRegisters().pc = address;
		emulator::cycles(1);
	}
	else
	{
		LOG_INFO("JR: Not taking jump, continuing to next instruction");
	}

}

void Ld8BitCommand::execute(cpu& m_cpu)
{
	//check if writing to memory or a register
	if (m_cpu.getDestinationIsMemory()) {
		
		m_cpu.getBus().lock()->write(m_cpu.getMemoryDestination(), m_cpu.getFetchedData());
		
		emulator::cycles(1);

		return;
	}
	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, m_cpu.getFetchedData());
}
//check if writing to memort or a register
void Ld16BitCommand::execute(cpu& m_cpu)
{
	if (m_cpu.getDestinationIsMemory()) {
		emulator::cycles(1);
		m_cpu.getBus().lock()->write16(m_cpu.getMemoryDestination(), m_cpu.getFetchedData());
		emulator::cycles(1);

		return;
	}
	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, m_cpu.getFetchedData());
}

void LdSpecialCommand::execute(cpu& m_cpu)
{
	const auto offset = static_cast<int8_t>(m_cpu.getFetchedData() & 0xFF);
	const uint16_t sp = m_cpu.readRegister(RT_SP);
	const auto result = static_cast<uint16_t>(sp + offset);

	const int h = ((sp & 0xF) + (offset & 0xF)) >= 0x10;
	const int c = ((sp & 0xFF) + (offset & 0xFF)) >= 0x100;

	m_cpu.writeRegister(RT_HL, result);
	m_cpu.setFlags(0, 0, h, c);
	emulator::cycles(1);
}

void LdhCommand::execute(cpu& m_cpu)
{
	if (m_cpu.getCurrentOpcodeData().reg1 == RT_A)
	{
		m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, m_cpu.getBus().lock()->read(0xFF00 + m_cpu.getFetchedData() ));
	}
	else
	{
		m_cpu.getBus().lock()->write(0xFF00 | m_cpu.getFetchedData(), m_cpu.getRegisters().a);
	}

	emulator::cycles(1);
}

void NopCommand::execute(cpu& m_cpu)
{

}

void OrCommand::execute(cpu& m_cpu)
{
	m_cpu.getRegisters().a |= m_cpu.getFetchedData() & 0xFF;
	m_cpu.setFlags(m_cpu.getRegisters().a == 0, 0, 0, 0);
}

void PopCommand::execute(cpu& m_cpu)
{
	//using popStack instead of popStack16 to account for cycles
	const uint16_t low = m_cpu.popStack();
	emulator::cycles(1);
	const uint16_t high = m_cpu.popStack();
	emulator::cycles(1);
	const uint16_t value = (high << 8) | low;

	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, value);
}

void PopSpecialCommand::execute(cpu& m_cpu)
{
	//using popStack instead of popStack16 to account for cycles
	const uint16_t low = m_cpu.popStack();
	emulator::cycles(1);
	const uint16_t high = m_cpu.popStack();
	emulator::cycles(1);
	const uint16_t value = (high << 8) | low;
	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, value & 0xFFF0);
}

void PushCommand::execute(cpu& m_cpu)
{
	const uint16_t high = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) >> 8 & 0xFF;
	emulator::cycles(1);
	m_cpu.pushStack(high);

	const uint16_t low = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg2) & 0xFF;
	emulator::cycles(1);
	m_cpu.pushStack(low);

	emulator::cycles(1);
}

void RetCommand::execute(cpu& m_cpu)
{
	if (m_cpu.getCurrentOpcodeData().cond != CT_NONE)
	{
		emulator::cycles(1);
	}

	if (m_cpu.checkConditionFlags())
	{
		//using popStack instead of popStack16 to account for cycles
		const uint16_t low = m_cpu.popStack();
		emulator::cycles(1);
		const uint16_t high = m_cpu.popStack();
		emulator::cycles(1);

		const uint16_t value = (high << 8) | low;
		m_cpu.getRegisters().pc = value;
		emulator::cycles(1);
	}

}

void RetiCommand::execute(cpu& m_cpu)
{
	m_cpu.setMasterInterruptEnabled(true);
	if (m_cpu.getCurrentOpcodeData().cond != CT_NONE)
	{
		emulator::cycles(1);
	}

	if (m_cpu.checkConditionFlags())
	{
		//using popStack instead of popStack16 to account for cycles
		const uint16_t low = m_cpu.popStack();
		emulator::cycles(1);
		const uint16_t high = m_cpu.popStack();
		emulator::cycles(1);

		const uint16_t value = (high << 8) | low;
		m_cpu.getRegisters().pc = value;
		emulator::cycles(1);
	}
}

void RlaCommand::execute(cpu& m_cpu)
{
	const uint8_t u = m_cpu.getRegisters().a;
	const uint8_t cf = utility::checkBit(m_cpu.getRegisters().f, 4);
	const uint8_t c = (u >> 7) & 1;

	m_cpu.getRegisters().a = (u << 1) | cf;
	m_cpu.setFlags(0, 0, 0, c);
}

void RlcaCommand::execute(cpu& m_cpu)
{
	uint8_t u = m_cpu.getRegisters().a;
	const bool c = (u >> 7) & 1;
	u = (u << 1) | static_cast<int>(c);
	m_cpu.getRegisters().a = u;

	m_cpu.setFlags(0, 0, 0, c);
}

void RraCommand::execute(cpu& m_cpu)
{
	const uint8_t carry = utility::checkBit(m_cpu.getRegisters().f, 4);
	const uint8_t new_c = m_cpu.getRegisters().a & 1;

	m_cpu.getRegisters().a >>= 1;
	m_cpu.getRegisters().a |= (carry << 7);

	m_cpu.setFlags( 0, 0, 0, new_c);
}

void RrcaCommand::execute(cpu& m_cpu)
{
	const uint8_t b = m_cpu.getRegisters().a & 1;
	m_cpu.getRegisters().a >>= 1;
	m_cpu.getRegisters().a |= (b << 7);

	m_cpu.setFlags( 0, 0, 0, b);
}

void RstCommand::execute(cpu& m_cpu)
{
	if (m_cpu.checkConditionFlags())
	{
		emulator::cycles(2);
		m_cpu.pushStack16(m_cpu.getRegisters().pc);

		m_cpu.getRegisters().pc = m_cpu.getCurrentOpcodeData().param;
		emulator::cycles(1);
	}
}

void SbcCommand::execute(cpu& m_cpu)
{
	const uint16_t value = m_cpu.getFetchedData() + utility::checkBit(m_cpu.getRegisters().f, 4);

	const int16_t z = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) - value == 0;
	const int16_t h = static_cast<int16_t>(m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) & 0x0F) - static_cast<int16_t>(m_cpu.getFetchedData() & 0x0F) - static_cast<int>(utility::checkBit(m_cpu.getRegisters().f, 4)) < 0;
	const int16_t c = static_cast<int16_t>(m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1)) - static_cast<int16_t>(m_cpu.getFetchedData()) - static_cast<int>(utility::checkBit(m_cpu.getRegisters().f, 4)) < 0;

	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, value);
	m_cpu.setFlags(z,1,h,c);
}

void ScfCommand::execute(cpu& m_cpu)
{
	m_cpu.setFlags(-1,0,0,1);
}

void StopCommand::execute(cpu& m_cpu)
{
	LOG_WARN("Stopping!");
}

void SubCommand::execute(cpu& m_cpu)
{
	const uint16_t value = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) - m_cpu.getFetchedData();

	const int16_t z = value == 0;
	const int16_t h = static_cast<int16_t>(m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) & 0x0F) - static_cast<int16_t>(m_cpu.getFetchedData() & 0x0F) < 0;
	const int16_t c = static_cast<int16_t>(m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1)) - static_cast<int16_t>(m_cpu.getFetchedData()) < 0;

	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) - value);
	m_cpu.setFlags(z,1,h,c);
}

void XorCommand::execute(cpu& m_cpu)
{
	m_cpu.getRegisters().a ^= m_cpu.getFetchedData() & 0xFF;
	m_cpu.setFlags(m_cpu.getRegisters().a == 0, 0, 0, 0);
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
        	if (opcode.reg2 >= RT_AF)
        	{
        		return std::make_unique<Ld16BitCommand>();
        	}
			else if (opcode.mode == AM_HL_SPR)
			{
				return std::make_unique<LdSpecialCommand>();
			}
	        return std::make_unique<Ld8BitCommand>();
        }
        case OP_INC:    // INC 8-bit: Increment 8-bit register
        {
        	if (opcode.reg2 >= RT_AF)
        	{
        		return std::make_unique<Inc16BitCommand>();
        	}
	        return std::make_unique<Inc8BitCommand>();
        }
        case OP_DEC:    // DEC 8-bit: Decrement 8-bit register
        {
        	if (opcode.reg2 >= RT_AF)
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
        	if (opcode.reg2 >= RT_AF)
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
        //case OP_ADDSP:   // ADD SP, e: Add signed immediate to SP
            //return std::make_unique<AddSPCommand>();
        default:
        {
	        LOG_ERROR("Execution Failed: Unimplemented Opcode");
        	return nullptr;
        }
    }
}