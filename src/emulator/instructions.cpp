#include "instructions.h"
#include "cpu.h"

instruction::instruction()
{
	name = "";
	type = instructionType::IN_NONE;
	mode = addressMode::AM_IMP;
	reg1 = registryType::RT_NONE;
	reg2 = registryType::RT_NONE;
	cond = conditionType::CT_NONE;
	param = 0;
}

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
        return instruction(std::string("0x00"), IN_NOP, AM_IMP);
    case 0x01:
        return instruction(std::string("0x01"), IN_LD, AM_R_D16, RT_BC);
    case 0x02:
        return instruction(std::string("0x02"), IN_LD, AM_MR_R, RT_BC, RT_A);
    case 0x05:
        return instruction(std::string("0x05"), IN_DEC, AM_R, RT_B);
    case 0x06:
        return instruction(std::string("0x06"), IN_LD, AM_R_D8, RT_B);
    case 0x08:
        return instruction(std::string("0x08"), IN_LD, AM_A16_R, RT_NONE, RT_SP);
    case 0x0A:
        return instruction(std::string("0x0A"), IN_LD, AM_R_MR, RT_A, RT_BC);
    case 0x0E:
        return instruction(std::string("0x0E"), IN_LD, AM_R_D8, RT_C);
    case 0x11:
        return instruction(std::string("0x11"), IN_LD, AM_R_D16, RT_DE);
    case 0x12:
        return instruction(std::string("0x12"), IN_LD, AM_MR_R, RT_DE, RT_A);
    case 0x15:
        return instruction(std::string("0x15"), IN_DEC, AM_R, RT_D);
    case 0x16:
        return instruction(std::string("0x16"), IN_LD, AM_R_D8, RT_D);
    case 0x1A:
        return instruction(std::string("0x1A"), IN_LD, AM_R_MR, RT_A, RT_DE);
    case 0x1E:
        return instruction(std::string("0x1E"), IN_LD, AM_R_D8, RT_E);
    case 0x21:
        return instruction(std::string("0x21"), IN_LD, AM_R_D16, RT_HL);
    case 0x22:
        return instruction(std::string("0x22"), IN_LD, AM_HLI_R, RT_HL, RT_A);
    case 0x25:
        return instruction(std::string("0x25"), IN_DEC, AM_R, RT_H);
    case 0x26:
        return instruction(std::string("0x26"), IN_LD, AM_R_D8, RT_H);
    case 0x2A:
        return instruction(std::string("0x2A"), IN_LD, AM_R_HLI, RT_A, RT_HL);
    case 0x2E:
        return instruction(std::string("0x2E"), IN_LD, AM_R_D8, RT_L);
    case 0x31:
        return instruction(std::string("0x31"), IN_LD, AM_R_D16, RT_SP);
    case 0x32:
        return instruction(std::string("0x32"), IN_LD, AM_HLD_R, RT_HL, RT_A);
    case 0x35:
        return instruction(std::string("0x35"), IN_DEC, AM_R, RT_HL);
    case 0x36:
        return instruction(std::string("0x36"), IN_LD, AM_MR_D8, RT_HL);
    case 0x3A:
        return instruction(std::string("0x3A"), IN_LD, AM_R_HLD, RT_A, RT_HL);
    case 0x3E:
        return instruction(std::string("0x3E"), IN_LD, AM_R_D8, RT_A);
    case 0x40:
        return instruction(std::string("0x40"), IN_LD, AM_R_R, RT_B, RT_B);
    case 0x41:
        return instruction(std::string("0x41"), IN_LD, AM_R_R, RT_B, RT_C);
    case 0x42:
        return instruction(std::string("0x42"), IN_LD, AM_R_R, RT_B, RT_D);
    case 0x43:
        return instruction(std::string("0x43"), IN_LD, AM_R_R, RT_B, RT_E);
    case 0x44:
        return instruction(std::string("0x44"), IN_LD, AM_R_R, RT_B, RT_H);
    case 0x45:
        return instruction(std::string("0x45"), IN_LD, AM_R_R, RT_B, RT_L);
    case 0x46:
        return instruction(std::string("0x46"), IN_LD, AM_R_MR, RT_B, RT_HL);
    case 0x47:
        return instruction(std::string("0x47"), IN_LD, AM_R_R, RT_B, RT_A);
    case 0x48:
        return instruction(std::string("0x48"), IN_LD, AM_R_R, RT_C, RT_B);
    case 0x49:
        return instruction(std::string("0x49"), IN_LD, AM_R_R, RT_C, RT_C);
    case 0x4A:
        return instruction(std::string("0x4A"), IN_LD, AM_R_R, RT_C, RT_D);
    case 0x4B:
        return instruction(std::string("0x4B"), IN_LD, AM_R_R, RT_C, RT_E);
    case 0x4C:
        return instruction(std::string("0x4C"), IN_LD, AM_R_R, RT_C, RT_H);
    case 0x4D:
        return instruction(std::string("0x4D"), IN_LD, AM_R_R, RT_C, RT_L);
    case 0x4E:
        return instruction(std::string("0x4E"), IN_LD, AM_R_MR, RT_C, RT_HL);
    case 0x4F:
        return instruction(std::string("0x4F"), IN_LD, AM_R_R, RT_C, RT_A);
    case 0x50:
        return instruction(std::string("0x50"), IN_LD, AM_R_R, RT_D, RT_B);
    case 0x51:
        return instruction(std::string("0x51"), IN_LD, AM_R_R, RT_D, RT_C);
    case 0x52:
        return instruction(std::string("0x52"), IN_LD, AM_R_R, RT_D, RT_D);
    case 0x53:
        return instruction(std::string("0x53"), IN_LD, AM_R_R, RT_D, RT_E);
    case 0x54:
        return instruction(std::string("0x54"), IN_LD, AM_R_R, RT_D, RT_H);
    case 0x55:
        return instruction(std::string("0x55"), IN_LD, AM_R_R, RT_D, RT_L);
    case 0x56:
        return instruction(std::string("0x56"), IN_LD, AM_R_MR, RT_D, RT_HL);
    case 0x57:
        return instruction(std::string("0x57"), IN_LD, AM_R_R, RT_D, RT_A);
    case 0x58:
        return instruction(std::string("0x58"), IN_LD, AM_R_R, RT_E, RT_B);
    case 0x59:
        return instruction(std::string("0x59"), IN_LD, AM_R_R, RT_E, RT_C);
    case 0x5A:
        return instruction(std::string("0x5A"), IN_LD, AM_R_R, RT_E, RT_D);
    case 0x5B:
        return instruction(std::string("0x5B"), IN_LD, AM_R_R, RT_E, RT_E);
    case 0x5C:
        return instruction(std::string("0x5C"), IN_LD, AM_R_R, RT_E, RT_H);
    case 0x5D:
        return instruction(std::string("0x5D"), IN_LD, AM_R_R, RT_E, RT_L);
    case 0x5E:
        return instruction(std::string("0x5E"), IN_LD, AM_R_MR, RT_E, RT_HL);
    case 0x5F:
        return instruction(std::string("0x5F"), IN_LD, AM_R_R, RT_E, RT_A);
    case 0x60:
        return instruction(std::string("0x60"), IN_LD, AM_R_R, RT_H, RT_B);
    case 0x61:
        return instruction(std::string("0x61"), IN_LD, AM_R_R, RT_H, RT_C);
    case 0x62:
        return instruction(std::string("0x62"), IN_LD, AM_R_R, RT_H, RT_D);
    case 0x63:
        return instruction(std::string("0x63"), IN_LD, AM_R_R, RT_H, RT_E);
    case 0x64:
        return instruction(std::string("0x64"), IN_LD, AM_R_R, RT_H, RT_H);
    case 0x65:
        return instruction(std::string("0x65"), IN_LD, AM_R_R, RT_H, RT_L);
    case 0x66:
        return instruction(std::string("0x66"), IN_LD, AM_R_MR, RT_H, RT_HL);
    case 0x67:
        return instruction(std::string("0x67"), IN_LD, AM_R_R, RT_H, RT_A);
    case 0x68:
        return instruction(std::string("0x68"), IN_LD, AM_R_R, RT_L, RT_B);
    case 0x69:
        return instruction(std::string("0x69"), IN_LD, AM_R_R, RT_L, RT_C);
    case 0x6A:
        return instruction(std::string("0x6A"), IN_LD, AM_R_R, RT_L, RT_D);
    case 0x6B:
        return instruction(std::string("0x6B"), IN_LD, AM_R_R, RT_L, RT_E);
    case 0x6C:
        return instruction(std::string("0x6C"), IN_LD, AM_R_R, RT_L, RT_H);
    case 0x6D:
        return instruction(std::string("0x6D"), IN_LD, AM_R_R, RT_L, RT_L);
    case 0x6E:
        return instruction(std::string("0x6E"), IN_LD, AM_R_MR, RT_L, RT_HL);
    case 0x6F:
        return instruction(std::string("0x6F"), IN_LD, AM_R_R, RT_L, RT_A);
    case 0x70:
        return instruction(std::string("0x70"), IN_LD, AM_MR_R, RT_HL, RT_B);
    case 0x71:
        return instruction(std::string("0x71"), IN_LD, AM_MR_R, RT_HL, RT_C);
    case 0x72:
        return instruction(std::string("0x72"), IN_LD, AM_MR_R, RT_HL, RT_D);
    case 0x73:
        return instruction(std::string("0x73"), IN_LD, AM_MR_R, RT_HL, RT_E);
    case 0x74:
        return instruction(std::string("0x74"), IN_LD, AM_MR_R, RT_HL, RT_H);
    case 0x75:
        return instruction(std::string("0x75"), IN_LD, AM_MR_R, RT_HL, RT_L);
    case 0x76:
        return instruction(std::string("0x76"), IN_HALT);
    case 0x77:
        return instruction(std::string("0x77"), IN_LD, AM_MR_R, RT_HL, RT_A);
    case 0x78:
        return instruction(std::string("0x78"), IN_LD, AM_R_R, RT_A, RT_B);
    case 0x79:
        return instruction(std::string("0x79"), IN_LD, AM_R_R, RT_A, RT_C);
    case 0x7A:
        return instruction(std::string("0x7A"), IN_LD, AM_R_R, RT_A, RT_D);
    case 0x7B:
        return instruction(std::string("0x7B"), IN_LD, AM_R_R, RT_A, RT_E);
    case 0x7C:
        return instruction(std::string("0x7C"), IN_LD, AM_R_R, RT_A, RT_H);
    case 0x7D:
        return instruction(std::string("0x7D"), IN_LD, AM_R_R, RT_A, RT_L);
    case 0x7E:
        return instruction(std::string("0x7E"), IN_LD, AM_R_MR, RT_A, RT_HL);
    case 0x7F:
        return instruction(std::string("0x7F"), IN_LD, AM_R_R, RT_A, RT_A);
	case 0xAF:
		return instruction(std::string("0xAF"), IN_XOR, AM_R, RT_A);
	case 0xC3:
		return instruction(std::string("0xC3"), IN_JP, AM_D16);
	case 0xF3:
		return instruction(std::string("0xF3"), IN_DI);
	default:
		return instruction();
	}
}

char* instructions::instructionName(instructionType t)
{
	return nullptr;
}


