#include "instructions.h"
#include "cpu.h"

instruction::instruction(std::string m_name,instructionType m_type, addressMode m_mode, registryType m_reg1, registryType m_reg2, conditionType m_cond, uint8_t m_param)
{
	name = m_name;
	type = m_type;
	mode = m_mode;
	reg1 = m_reg1;
	reg2 = m_reg2;
	cond = m_cond;
	param = m_param;
}

instruction instructions::instructionByOpcode(uint8_t opcode)
{
	switch (opcode)
	{
	case 0x00:
		return instruction(std::string("0x00"),instructionType::IN_NOP, addressMode::AM_IMP);
		break;
	case 0x05:
		return instruction(std::string("0x05"), instructionType::IN_DEC, addressMode::AM_R, registryType::RT_B);
		break;
	case 0x0E:
		return instruction(std::string("0x0E"), instructionType::IN_LD, addressMode::AM_R_D8, registryType::RT_C);
		break;
	case 0xAF:
		return instruction(std::string("0xAF"), instructionType::IN_XOR, addressMode::AM_R, registryType::RT_A);
		break;
	case 0xC3:
		return instruction(std::string("0xC3"), instructionType::IN_JP, addressMode::AM_D16);
		break;
	default:
		return instruction();
	}
}

char* instructions::instructionName(instructionType t)
{
	return nullptr;
}


