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
    case 0x03:
        return instruction(std::string("0x03"), IN_INC, AM_R, RT_BC);
    case 0x04:
        return instruction(std::string("0x04"), IN_INC, AM_R, RT_B);
    case 0x05:
        return instruction(std::string("0x05"), IN_DEC, AM_R, RT_B);
    case 0x06:
        return instruction(std::string("0x06"), IN_LD, AM_R_D8, RT_B);
    case 0x07:
        return instruction(std::string("0x07"), IN_RLCA);
    case 0x08:
        return instruction(std::string("0x08"), IN_LD, AM_D16_R, RT_NONE, RT_SP);
    case 0x09:
        return instruction(std::string("0x09"), IN_ADD, AM_R_R, RT_HL, RT_BC);
    case 0x0A:
        return instruction(std::string("0x0A"), IN_LD, AM_R_MR, RT_A, RT_BC);
    case 0x0B:
        return instruction(std::string("0x0B"), IN_DEC, AM_R, RT_BC);
    case 0x0C:
        return instruction(std::string("0x0C"), IN_INC, AM_R, RT_C);
    case 0x0D:
        return instruction(std::string("0x0D"), IN_DEC, AM_R, RT_C);
    case 0x0E:
        return instruction(std::string("0x0E"), IN_LD, AM_R_D8, RT_C);
    case 0x0F:
        return instruction(std::string("0x0F"), IN_RRCA);
    case 0x10:
        return instruction(std::string("0x10"), IN_STOP);
    case 0x11:
        return instruction(std::string("0x11"), IN_LD, AM_R_D16, RT_DE);
    case 0x12:
        return instruction(std::string("0x12"), IN_LD, AM_MR_R, RT_DE, RT_A);
    case 0x13:
        return instruction(std::string("0x13"), IN_INC, AM_R, RT_DE);
    case 0x14:
        return instruction(std::string("0x14"), IN_INC, AM_R, RT_D);
    case 0x15:
        return instruction(std::string("0x15"), IN_DEC, AM_R, RT_D);
    case 0x16:
        return instruction(std::string("0x16"), IN_LD, AM_R_D8, RT_D);
    case 0x17:
        return instruction(std::string("0x17"), IN_RLA);
    case 0x18:
        return instruction(std::string("0x18"), IN_JR, AM_D8);
    case 0x19:
        return instruction(std::string("0x19"), IN_ADD, AM_R_R, RT_HL, RT_DE);
    case 0x1A:
        return instruction(std::string("0x1A"), IN_LD, AM_R_MR, RT_A, RT_DE);
    case 0x1B:
        return instruction(std::string("0x1B"), IN_DEC, AM_R, RT_DE);
    case 0x1C:
        return instruction(std::string("0x1C"), IN_INC, AM_R, RT_E);
    case 0x1D:
        return instruction(std::string("0x1D"), IN_DEC, AM_R, RT_E);
    case 0x1E:
        return instruction(std::string("0x1E"), IN_LD, AM_R_D8, RT_E);
    case 0x1F:
        return instruction(std::string("0x1F"), IN_RRA);
    case 0x20:
        return instruction(std::string("0x20"), IN_JR, AM_D8, RT_NONE, RT_NONE, CT_NZ);
    case 0x21:
        return instruction(std::string("0x21"), IN_LD, AM_R_D16, RT_HL);
    case 0x22:
        return instruction(std::string("0x22"), IN_LD, AM_HLI_R, RT_HL, RT_A);
    case 0x23:
        return instruction(std::string("0x23"), IN_INC, AM_R, RT_HL);
    case 0x24:
        return instruction(std::string("0x24"), IN_INC, AM_R, RT_H);
    case 0x25:
        return instruction(std::string("0x25"), IN_DEC, AM_R, RT_H);
    case 0x26:
        return instruction(std::string("0x26"), IN_LD, AM_R_D8, RT_H);
    case 0x27:
        return instruction(std::string("0x27"), IN_DAA);
    case 0x28:
        return instruction(std::string("0x28"), IN_JR, AM_D8, RT_NONE, RT_NONE, CT_Z);
    case 0x29:
        return instruction(std::string("0x29"), IN_ADD, AM_R_R, RT_HL, RT_HL);
    case 0x2A:
        return instruction(std::string("0x2A"), IN_LD, AM_R_HLI, RT_A, RT_HL);
    case 0x2B:
        return instruction(std::string("0x2B"), IN_DEC, AM_R, RT_HL);
    case 0x2C:
        return instruction(std::string("0x2C"), IN_INC, AM_R, RT_L);
    case 0x2D:
        return instruction(std::string("0x2D"), IN_DEC, AM_R, RT_L);
    case 0x2E:
        return instruction(std::string("0x2E"), IN_LD, AM_R_D8, RT_L);
    case 0x2F:
        return instruction(std::string("0x2F"), IN_CPL);
    case 0x30:
        return instruction(std::string("0x30"), IN_JR, AM_D8, RT_NONE, RT_NONE, CT_NC);
    case 0x31:
        return instruction(std::string("0x31"), IN_LD, AM_R_D16, RT_SP);
    case 0x32:
        return instruction(std::string("0x32"), IN_LD, AM_HLD_R, RT_HL, RT_A);
    case 0x33:
        return instruction(std::string("0x33"), IN_INC, AM_R, RT_SP);
    case 0x34:
        return instruction(std::string("0x34"), IN_INC, AM_MR, RT_HL);
    case 0x35:
        return instruction(std::string("0x35"), IN_DEC, AM_MR, RT_HL);
    case 0x36:
        return instruction(std::string("0x36"), IN_LD, AM_MR_D8, RT_HL);
    case 0x37:
        return instruction(std::string("0x37"), IN_SCF);
    case 0x38:
        return instruction(std::string("0x38"), IN_JR, AM_D8, RT_NONE, RT_NONE, CT_C);
    case 0x39:
        return instruction(std::string("0x39"), IN_ADD, AM_R_R, RT_HL, RT_SP);
    case 0x3A:
        return instruction(std::string("0x3A"), IN_LD, AM_R_HLD, RT_A, RT_HL);
    case 0x3B:
        return instruction(std::string("0x3B"), IN_DEC, AM_R, RT_SP);
    case 0x3C:
        return instruction(std::string("0x3C"), IN_INC, AM_R, RT_A);
    case 0x3D:
        return instruction(std::string("0x3D"), IN_DEC, AM_R, RT_A);
    case 0x3E:
        return instruction(std::string("0x3E"), IN_LD, AM_R_D8, RT_A);
    case 0x3F:
        return instruction(std::string("0x3F"), IN_CCF);
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
    case 0x80:
        return instruction(std::string("0x80"), IN_ADD, AM_R_R, RT_A, RT_B);
    case 0x81:
        return instruction(std::string("0x81"), IN_ADD, AM_R_R, RT_A, RT_C);
    case 0x82:
        return instruction(std::string("0x82"), IN_ADD, AM_R_R, RT_A, RT_D);
    case 0x83:
        return instruction(std::string("0x83"), IN_ADD, AM_R_R, RT_A, RT_E);
    case 0x84:
        return instruction(std::string("0x84"), IN_ADD, AM_R_R, RT_A, RT_H);
    case 0x85:
        return instruction(std::string("0x85"), IN_ADD, AM_R_R, RT_A, RT_L);
    case 0x86:
        return instruction(std::string("0x86"), IN_ADD, AM_R_MR, RT_A, RT_HL);
    case 0x87:
        return instruction(std::string("0x87"), IN_ADD, AM_R_R, RT_A, RT_A);
    case 0x88:
        return instruction(std::string("0x88"), IN_ADC, AM_R_R, RT_A, RT_B);
    case 0x89:
        return instruction(std::string("0x89"), IN_ADC, AM_R_R, RT_A, RT_C);
    case 0x8A:
        return instruction(std::string("0x8A"), IN_ADC, AM_R_R, RT_A, RT_D);
    case 0x8B:
        return instruction(std::string("0x8B"), IN_ADC, AM_R_R, RT_A, RT_E);
    case 0x8C:
        return instruction(std::string("0x8C"), IN_ADC, AM_R_R, RT_A, RT_H);
    case 0x8D:
        return instruction(std::string("0x8D"), IN_ADC, AM_R_R, RT_A, RT_L);
    case 0x8E:
        return instruction(std::string("0x8E"), IN_ADC, AM_R_MR, RT_A, RT_HL);
    case 0x8F:
        return instruction(std::string("0x8F"), IN_ADC, AM_R_R, RT_A, RT_A);
    case 0x90:
        return instruction(std::string("0x90"), IN_SUB, AM_R_R, RT_A, RT_B);
    case 0x91:
        return instruction(std::string("0x91"), IN_SUB, AM_R_R, RT_A, RT_C);
    case 0x92:
        return instruction(std::string("0x92"), IN_SUB, AM_R_R, RT_A, RT_D);
    case 0x93:
        return instruction(std::string("0x93"), IN_SUB, AM_R_R, RT_A, RT_E);
    case 0x94:
        return instruction(std::string("0x94"), IN_SUB, AM_R_R, RT_A, RT_H);
    case 0x95:
        return instruction(std::string("0x95"), IN_SUB, AM_R_R, RT_A, RT_L);
    case 0x96:
        return instruction(std::string("0x96"), IN_SUB, AM_R_MR, RT_A, RT_HL);
    case 0x97:
        return instruction(std::string("0x97"), IN_SUB, AM_R_R, RT_A, RT_A);
    case 0x98:
        return instruction(std::string("0x98"), IN_SBC, AM_R_R, RT_A, RT_B);
    case 0x99:
        return instruction(std::string("0x99"), IN_SBC, AM_R_R, RT_A, RT_C);
    case 0x9A:
        return instruction(std::string("0x9A"), IN_SBC, AM_R_R, RT_A, RT_D);
    case 0x9B:
        return instruction(std::string("0x9B"), IN_SBC, AM_R_R, RT_A, RT_E);
    case 0x9C:
        return instruction(std::string("0x9C"), IN_SBC, AM_R_R, RT_A, RT_H);
    case 0x9D:
        return instruction(std::string("0x9D"), IN_SBC, AM_R_R, RT_A, RT_L);
    case 0x9E:
        return instruction(std::string("0x9E"), IN_SBC, AM_R_MR, RT_A, RT_HL);
    case 0x9F:
        return instruction(std::string("0x9F"), IN_SBC, AM_R_R, RT_A, RT_A);
    case 0xA0:
        return instruction(std::string("0xA0"), IN_AND, AM_R_R, RT_A, RT_B);
    case 0xA1:
        return instruction(std::string("0xA1"), IN_AND, AM_R_R, RT_A, RT_C);
    case 0xA2:
        return instruction(std::string("0xA2"), IN_AND, AM_R_R, RT_A, RT_D);
    case 0xA3:
        return instruction(std::string("0xA3"), IN_AND, AM_R_R, RT_A, RT_E);
    case 0xA4:
        return instruction(std::string("0xA4"), IN_AND, AM_R_R, RT_A, RT_H);
    case 0xA5:
        return instruction(std::string("0xA5"), IN_AND, AM_R_R, RT_A, RT_L);
    case 0xA6:
        return instruction(std::string("0xA6"), IN_AND, AM_R_MR, RT_A, RT_HL);
    case 0xA7:
        return instruction(std::string("0xA7"), IN_AND, AM_R_R, RT_A, RT_A);
    case 0xA8:
        return instruction(std::string("0xA8"), IN_XOR, AM_R_R, RT_A, RT_B);
    case 0xA9:
        return instruction(std::string("0xA9"), IN_XOR, AM_R_R, RT_A, RT_C);
    case 0xAA:
        return instruction(std::string("0xAA"), IN_XOR, AM_R_R, RT_A, RT_D);
    case 0xAB:
        return instruction(std::string("0xAB"), IN_XOR, AM_R_R, RT_A, RT_E);
    case 0xAC:
        return instruction(std::string("0xAC"), IN_XOR, AM_R_R, RT_A, RT_H);
    case 0xAD:
        return instruction(std::string("0xAD"), IN_XOR, AM_R_R, RT_A, RT_L);
    case 0xAE:
        return instruction(std::string("0xAE"), IN_XOR, AM_R_MR, RT_A, RT_HL);
    case 0xAF:
        return instruction(std::string("0xAF"), IN_XOR, AM_R_R, RT_A, RT_A);
    case 0xB0:
        return instruction(std::string("0xB0"), IN_OR, AM_R_R, RT_A, RT_B);
    case 0xB1:
        return instruction(std::string("0xB1"), IN_OR, AM_R_R, RT_A, RT_C);
    case 0xB2:
        return instruction(std::string("0xB2"), IN_OR, AM_R_R, RT_A, RT_D);
    case 0xB3:
        return instruction(std::string("0xB3"), IN_OR, AM_R_R, RT_A, RT_E);
    case 0xB4:
        return instruction(std::string("0xB4"), IN_OR, AM_R_R, RT_A, RT_H);
    case 0xB5:
        return instruction(std::string("0xB5"), IN_OR, AM_R_R, RT_A, RT_L);
    case 0xB6:
        return instruction(std::string("0xB6"), IN_OR, AM_R_MR, RT_A, RT_HL);
    case 0xB7:
        return instruction(std::string("0xB7"), IN_OR, AM_R_R, RT_A, RT_A);
    case 0xB8:
        return instruction(std::string("0xB8"), IN_CP, AM_R_R, RT_A, RT_B);
    case 0xB9:
        return instruction(std::string("0xB9"), IN_CP, AM_R_R, RT_A, RT_C);
    case 0xBA:
        return instruction(std::string("0xBA"), IN_CP, AM_R_R, RT_A, RT_D);
    case 0xBB:
        return instruction(std::string("0xBB"), IN_CP, AM_R_R, RT_A, RT_E);
    case 0xBC:
        return instruction(std::string("0xBC"), IN_CP, AM_R_R, RT_A, RT_H);
    case 0xBD:
        return instruction(std::string("0xBD"), IN_CP, AM_R_R, RT_A, RT_L);
    case 0xBE:
        return instruction(std::string("0xBE"), IN_CP, AM_R_MR, RT_A, RT_HL);
    case 0xBF:
        return instruction(std::string("0xBF"), IN_CP, AM_R_R, RT_A, RT_A);
    case 0xC0:
        return instruction(std::string("0xC0"), IN_RET, AM_IMP, RT_NONE, RT_NONE, CT_NZ);
    case 0xC1:
        return instruction(std::string("0xC1"), IN_POP, AM_R, RT_BC);
    case 0xC2:
        return instruction(std::string("0xC2"), IN_JP, AM_D16, RT_NONE, RT_NONE, CT_NZ);
    case 0xC3:
        return instruction(std::string("0xC3"), IN_JP, AM_D16);
    case 0xC4:
        return instruction(std::string("0xC4"), IN_CALL, AM_D16, RT_NONE, RT_NONE, CT_NZ);
    case 0xC5:
        return instruction(std::string("0xC5"), IN_PUSH, AM_R, RT_BC);
    case 0xC6:
        return instruction(std::string("0xC6"), IN_ADD, AM_R_D8, RT_A);
    case 0xC7:
        return instruction(std::string("0xC7"), IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x00);
    case 0xC8:
        return instruction(std::string("0xC8"), IN_RET, AM_IMP, RT_NONE, RT_NONE, CT_Z);
    case 0xC9:
        return instruction(std::string("0xC9"), IN_RET);
    case 0xCA:
        return instruction(std::string("0xCA"), IN_JP, AM_D16, RT_NONE, RT_NONE, CT_Z);
    case 0xCB:
        return instruction(std::string("0xCB"), IN_CB);
    case 0xCC:
        return instruction(std::string("0xCC"), IN_CALL, AM_D16, RT_NONE, RT_NONE, CT_Z);
    case 0xCD:
        return instruction(std::string("0xCD"), IN_CALL, AM_D16);
    case 0xCE:
        return instruction(std::string("0xCE"), IN_ADC, AM_R_D8, RT_A);
    case 0xCF:
        return instruction(std::string("0xCF"), IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x08);
    case 0xD0:
        return instruction(std::string("0xD0"), IN_RET, AM_IMP, RT_NONE, RT_NONE, CT_NC);
    case 0xD1:
        return instruction(std::string("0xD1"), IN_POP, AM_R, RT_DE);
    case 0xD2:
        return instruction(std::string("0xD2"), IN_JP, AM_D16, RT_NONE, RT_NONE, CT_NC);
    //case 0xD3:
        //return instruction(std::string("0xD3"), IN_INVALID);
    case 0xD4:
        return instruction(std::string("0xD4"), IN_CALL, AM_D16, RT_NONE, RT_NONE, CT_NC);
    case 0xD5:
        return instruction(std::string("0xD5"), IN_PUSH, AM_R, RT_DE);
    case 0xD6:
        return instruction(std::string("0xD6"), IN_SUB, AM_R_D8, RT_A);
    case 0xD7:
        return instruction(std::string("0xD7"), IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x10);
    case 0xD8:
        return instruction(std::string("0xD8"), IN_RET, AM_IMP, RT_NONE, RT_NONE, CT_C);
    case 0xD9:
        return instruction(std::string("0xD9"), IN_RETI);
    case 0xDA:
        return instruction(std::string("0xDA"), IN_JP, AM_D16, RT_NONE, RT_NONE, CT_C);
    case 0xDC:
        return instruction(std::string("0xDC"), IN_CALL, AM_D16, RT_NONE, RT_NONE, CT_C);
    case 0xDE:
        return instruction(std::string("0xDE"), IN_SBC, AM_R_D8, RT_A);
    case 0xDF:
        return instruction(std::string("0xDF"), IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x18);
    case 0xE0:
        return instruction(std::string("0xE0"), IN_LDH, AM_A8_R, RT_NONE, RT_A);
    case 0xE1:
        return instruction(std::string("0xE1"), IN_POP, AM_R, RT_HL);
    case 0xE2:
        return instruction(std::string("0xE2"), IN_LDH, AM_MR_R, RT_C, RT_A);
    case 0xE5:
        return instruction(std::string("0xE5"), IN_PUSH, AM_R, RT_HL);
    case 0xE6:
        return instruction(std::string("0xE6"), IN_AND, AM_R_D8, RT_A);
    case 0xE7:
        return instruction(std::string("0xE7"), IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x20);
    case 0xE8:
        return instruction(std::string("0xE8"), IN_ADD, AM_R_D8, RT_SP);
    case 0xE9:
        return instruction(std::string("0xE9"), IN_JP, AM_R, RT_HL);
    case 0xEA:
        return instruction(std::string("0xEA"), IN_LD, AM_MR_R, RT_NONE, RT_A);
    case 0xEE:
        return instruction(std::string("0xEE"), IN_XOR, AM_R_D8, RT_A);
    case 0xEF:
        return instruction(std::string("0xEF"), IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x28);
    case 0xF0:
        return instruction(std::string("0xF0"), IN_LDH, AM_R_MR, RT_A);
    case 0xF1:
        return instruction(std::string("0xF1"), IN_POP, AM_R, RT_AF);
    case 0xF2:
        return instruction(std::string("0xF2"), IN_LDH, AM_R_MR, RT_A, RT_C);
    case 0xF3:
        return instruction(std::string("0xF3"), IN_DI);
    case 0xF5:
        return instruction(std::string("0xF5"), IN_PUSH, AM_R, RT_AF);
    case 0xF6:
        return instruction(std::string("0xF6"), IN_OR, AM_R_D8, RT_A);
    case 0xF7:
        return instruction(std::string("0xF7"), IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x30);
    case 0xF8:
        return instruction(std::string("0xF8"), IN_LD, AM_HL_SPR, RT_HL, RT_SP);
    case 0xF9:
        return instruction(std::string("0xF9"), IN_LD, AM_R_R, RT_SP, RT_HL);
    case 0xFA:
        return instruction(std::string("0xFA"), IN_LD, AM_R_MR, RT_A, RT_A);
    case 0xFB:
        return instruction(std::string("0xFB"), IN_EI);
    case 0xFE:
        return instruction(std::string("0xFE"), IN_CP, AM_R_D8, RT_A);
    case 0xFF:
        return instruction(std::string("0xFF"), IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x38);
    default:
        return instruction(std::string("0x00"), IN_NOP);
    }

}

std::string instructions::getInstructionTypeName(instruction t)
{
    switch (t.type)
    {
    case IN_NONE:
        return std::string("NONE");
    case IN_NOP:
        return std::string("NOP");
    case IN_LD:
        return std::string("LD");
    case IN_INC:
        return std::string("INC");
    case IN_DEC:
        return std::string("DEC");
    case IN_RLCA:
        return std::string("RLCA");
    case IN_ADD:
        return std::string("ADD");
    case IN_RRCA:
        return std::string("RRCA");
    case IN_STOP:
        return std::string("STOP");
    case IN_RLA:
        return std::string("RLA");
    case IN_JR:
        return std::string("JR");
    case IN_RRA:
        return std::string("RRA");
    case IN_DAA:
        return std::string("DAA");
    case IN_CPL:
        return std::string("CPL");
    case IN_SCF:
        return std::string("SCF");
    case IN_CCF:
        return std::string("CCF");
    case IN_HALT:
        return std::string("HALT");
    case IN_ADC:
        return std::string("ADC");
    case IN_SUB:
        return std::string("SUB");
    case IN_SBC:
        return std::string("SBC");
    case IN_AND:
        return std::string("AND");
    case IN_XOR:
        return std::string("XOR");
    case IN_OR:
        return std::string("OR");
    case IN_CP:
        return std::string("CP");
    case IN_POP:
        return std::string("POP");
    case IN_JP:
        return std::string("JP");
    case IN_PUSH:
        return std::string("PUSH");
    case IN_RET:
        return std::string("RET");
    case IN_CB:
        return std::string("CB");
    case IN_CALL:
        return std::string("CALL");
    case IN_RETI:
        return std::string("RETI");
    case IN_LDH:
        return std::string("LDH");
    case IN_JPHL:
        return std::string("JPHL");
    case IN_DI:
        return std::string("DI");
    case IN_EI:
        return std::string("EI");
    case IN_RST:
        return std::string("RST");
    case IN_ERR:
        return std::string("ERR");
    case IN_RLC:
        return std::string("RLC");
    case IN_RRC:
        return std::string("RRC");
    case IN_RL:
        return std::string("RL");
    case IN_RR:
        return std::string("RR");
    case IN_SLA:
        return std::string("SLA");
    case IN_SRA:
        return std::string("SRA");
    case IN_SWAP:
        return std::string("SWAP");
    case IN_SRL:
        return std::string("SRL");
    case IN_BIT:
        return std::string("BIT");
    case IN_RES:
        return std::string("RES");
    case IN_SET:
        return std::string("SET");
    }
}

registryType instructions::registryLookup(uint8_t reg)
{
    switch (reg)
    {
    case RT_NONE:
        return RT_NONE;
    case RT_A:
        return RT_A;
    case RT_F:
        return RT_F;
    case RT_B:
        return RT_B;
    case RT_C:
        return RT_C;
    case RT_D:
        return RT_D;
    case RT_E:
        return RT_E;
    case RT_H:
        return RT_H;
    case RT_L:
        return RT_L;
    case RT_AF:
        return RT_AF;
    case RT_BC:
        return RT_BC;
    case RT_DE:
        return RT_DE;
    case RT_HL:
        return RT_HL;
    case RT_SP:
        return RT_SP;
    case RT_PC:
        return RT_PC;
    default:
        return RT_NONE;
    }
}


