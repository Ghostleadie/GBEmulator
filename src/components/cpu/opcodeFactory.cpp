//
// Created by Jack_ on 08/09/2025.
//

#include "opcodeFactory.h"

opcode opcodeFactory::create(uint8_t currentopcode)
{
	switch (currentopcode)
    {
    case 0x00:
        return opcode(std::string("0x00"), OP_NOP, AM_IMP);
    case 0x01:
        return opcode(std::string("0x01"), OP_LD, AM_R_D16, RT_BC);
    case 0x02:
        return opcode(std::string("0x02"), OP_LD, AM_MR_R, RT_BC, RT_A);
    case 0x03:
        return opcode(std::string("0x03"), OP_INC, AM_R, RT_BC);
    case 0x04:
        return opcode(std::string("0x04"), OP_INC, AM_R, RT_B);
    case 0x05:
        return opcode(std::string("0x05"), OP_DEC, AM_R, RT_B);
    case 0x06:
        return opcode(std::string("0x06"), OP_LD, AM_R_D8, RT_B);
    case 0x07:
        return opcode(std::string("0x07"), OP_RLCA);
    case 0x08:
        return opcode(std::string("0x08"), OP_LD, AM_D16_R, RT_NONE, RT_SP);
    case 0x09:
        return opcode(std::string("0x09"), OP_ADD, AM_R_R, RT_HL, RT_BC);
    case 0x0A:
        return opcode(std::string("0x0A"), OP_LD, AM_R_MR, RT_A, RT_BC);
    case 0x0B:
        return opcode(std::string("0x0B"), OP_DEC, AM_R, RT_BC);
    case 0x0C:
        return opcode(std::string("0x0C"), OP_INC, AM_R, RT_C);
    case 0x0D:
        return opcode(std::string("0x0D"), OP_DEC, AM_R, RT_C);
    case 0x0E:
        return opcode(std::string("0x0E"), OP_LD, AM_R_D8, RT_C);
    case 0x0F:
        return opcode(std::string("0x0F"), OP_RRCA);
    case 0x10:
        return opcode(std::string("0x10"), OP_STOP);
    case 0x11:
        return opcode(std::string("0x11"), OP_LD, AM_R_D16, RT_DE);
    case 0x12:
        return opcode(std::string("0x12"), OP_LD, AM_MR_R, RT_DE, RT_A);
    case 0x13:
        return opcode(std::string("0x13"), OP_INC, AM_R, RT_DE);
    case 0x14:
        return opcode(std::string("0x14"), OP_INC, AM_R, RT_D);
    case 0x15:
        return opcode(std::string("0x15"), OP_DEC, AM_R, RT_D);
    case 0x16:
        return opcode(std::string("0x16"), OP_LD, AM_R_D8, RT_D);
    case 0x17:
        return opcode(std::string("0x17"), OP_RLA);
    case 0x18:
        return opcode(std::string("0x18"), OP_JR, AM_D8);
    case 0x19:
        return opcode(std::string("0x19"), OP_ADD, AM_R_R, RT_HL, RT_DE);
    case 0x1A:
        return opcode(std::string("0x1A"), OP_LD, AM_R_MR, RT_A, RT_DE);
    case 0x1B:
        return opcode(std::string("0x1B"), OP_DEC, AM_R, RT_DE);
    case 0x1C:
        return opcode(std::string("0x1C"), OP_INC, AM_R, RT_E);
    case 0x1D:
        return opcode(std::string("0x1D"), OP_DEC, AM_R, RT_E);
    case 0x1E:
        return opcode(std::string("0x1E"), OP_LD, AM_R_D8, RT_E);
    case 0x1F:
        return opcode(std::string("0x1F"), OP_RRA);
    case 0x20:
        return opcode(std::string("0x20"), OP_JR, AM_D8, RT_NONE, RT_NONE, CT_NZ);
    case 0x21:
        return opcode(std::string("0x21"), OP_LD, AM_R_D16, RT_HL);
    case 0x22:
        return opcode(std::string("0x22"), OP_LD, AM_HLI_R, RT_HL, RT_A);
    case 0x23:
        return opcode(std::string("0x23"), OP_INC, AM_R, RT_HL);
    case 0x24:
        return opcode(std::string("0x24"), OP_INC, AM_R, RT_H);
    case 0x25:
        return opcode(std::string("0x25"), OP_DEC, AM_R, RT_H);
    case 0x26:
        return opcode(std::string("0x26"), OP_LD, AM_R_D8, RT_H);
    case 0x27:
        return opcode(std::string("0x27"), OP_DAA);
    case 0x28:
        return opcode(std::string("0x28"), OP_JR, AM_D8, RT_NONE, RT_NONE, CT_Z);
    case 0x29:
        return opcode(std::string("0x29"), OP_ADD, AM_R_R, RT_HL, RT_HL);
    case 0x2A:
        return opcode(std::string("0x2A"), OP_LD, AM_R_HLI, RT_A, RT_HL);
    case 0x2B:
        return opcode(std::string("0x2B"), OP_DEC, AM_R, RT_HL);
    case 0x2C:
        return opcode(std::string("0x2C"), OP_INC, AM_R, RT_L);
    case 0x2D:
        return opcode(std::string("0x2D"), OP_DEC, AM_R, RT_L);
    case 0x2E:
        return opcode(std::string("0x2E"), OP_LD, AM_R_D8, RT_L);
    case 0x2F:
        return opcode(std::string("0x2F"), OP_CPL);
    case 0x30:
        return opcode(std::string("0x30"), OP_JR, AM_D8, RT_NONE, RT_NONE, CT_NC);
    case 0x31:
        return opcode(std::string("0x31"), OP_LD, AM_R_D16, RT_SP);
    case 0x32:
        return opcode(std::string("0x32"), OP_LD, AM_HLD_R, RT_HL, RT_A);
    case 0x33:
        return opcode(std::string("0x33"), OP_INC, AM_R, RT_SP);
    case 0x34:
        return opcode(std::string("0x34"), OP_INC, AM_MR, RT_HL);
    case 0x35:
        return opcode(std::string("0x35"), OP_DEC, AM_MR, RT_HL);
    case 0x36:
        return opcode(std::string("0x36"), OP_LD, AM_MR_D8, RT_HL);
    case 0x37:
        return opcode(std::string("0x37"), OP_SCF);
    case 0x38:
        return opcode(std::string("0x38"), OP_JR, AM_D8, RT_NONE, RT_NONE, CT_C);
    case 0x39:
        return opcode(std::string("0x39"), OP_ADD, AM_R_R, RT_HL, RT_SP);
    case 0x3A:
        return opcode(std::string("0x3A"), OP_LD, AM_R_HLD, RT_A, RT_HL);
    case 0x3B:
        return opcode(std::string("0x3B"), OP_DEC, AM_R, RT_SP);
    case 0x3C:
        return opcode(std::string("0x3C"), OP_INC, AM_R, RT_A);
    case 0x3D:
        return opcode(std::string("0x3D"), OP_DEC, AM_R, RT_A);
    case 0x3E:
        return opcode(std::string("0x3E"), OP_LD, AM_R_D8, RT_A);
    case 0x3F:
        return opcode(std::string("0x3F"), OP_CCF);
    case 0x40:
        return opcode(std::string("0x40"), OP_LD, AM_R_R, RT_B, RT_B);
    case 0x41:
        return opcode(std::string("0x41"), OP_LD, AM_R_R, RT_B, RT_C);
    case 0x42:
        return opcode(std::string("0x42"), OP_LD, AM_R_R, RT_B, RT_D);
    case 0x43:
        return opcode(std::string("0x43"), OP_LD, AM_R_R, RT_B, RT_E);
    case 0x44:
        return opcode(std::string("0x44"), OP_LD, AM_R_R, RT_B, RT_H);
    case 0x45:
        return opcode(std::string("0x45"), OP_LD, AM_R_R, RT_B, RT_L);
    case 0x46:
        return opcode(std::string("0x46"), OP_LD, AM_R_MR, RT_B, RT_HL);
    case 0x47:
        return opcode(std::string("0x47"), OP_LD, AM_R_R, RT_B, RT_A);
    case 0x48:
        return opcode(std::string("0x48"), OP_LD, AM_R_R, RT_C, RT_B);
    case 0x49:
        return opcode(std::string("0x49"), OP_LD, AM_R_R, RT_C, RT_C);
    case 0x4A:
        return opcode(std::string("0x4A"), OP_LD, AM_R_R, RT_C, RT_D);
    case 0x4B:
        return opcode(std::string("0x4B"), OP_LD, AM_R_R, RT_C, RT_E);
    case 0x4C:
        return opcode(std::string("0x4C"), OP_LD, AM_R_R, RT_C, RT_H);
    case 0x4D:
        return opcode(std::string("0x4D"), OP_LD, AM_R_R, RT_C, RT_L);
    case 0x4E:
        return opcode(std::string("0x4E"), OP_LD, AM_R_MR, RT_C, RT_HL);
    case 0x4F:
        return opcode(std::string("0x4F"), OP_LD, AM_R_R, RT_C, RT_A);
    case 0x50:
        return opcode(std::string("0x50"), OP_LD, AM_R_R, RT_D, RT_B);
    case 0x51:
        return opcode(std::string("0x51"), OP_LD, AM_R_R, RT_D, RT_C);
    case 0x52:
        return opcode(std::string("0x52"), OP_LD, AM_R_R, RT_D, RT_D);
    case 0x53:
        return opcode(std::string("0x53"), OP_LD, AM_R_R, RT_D, RT_E);
    case 0x54:
        return opcode(std::string("0x54"), OP_LD, AM_R_R, RT_D, RT_H);
    case 0x55:
        return opcode(std::string("0x55"), OP_LD, AM_R_R, RT_D, RT_L);
    case 0x56:
        return opcode(std::string("0x56"), OP_LD, AM_R_MR, RT_D, RT_HL);
    case 0x57:
        return opcode(std::string("0x57"), OP_LD, AM_R_R, RT_D, RT_A);
    case 0x58:
        return opcode(std::string("0x58"), OP_LD, AM_R_R, RT_E, RT_B);
    case 0x59:
        return opcode(std::string("0x59"), OP_LD, AM_R_R, RT_E, RT_C);
    case 0x5A:
        return opcode(std::string("0x5A"), OP_LD, AM_R_R, RT_E, RT_D);
    case 0x5B:
        return opcode(std::string("0x5B"), OP_LD, AM_R_R, RT_E, RT_E);
    case 0x5C:
        return opcode(std::string("0x5C"), OP_LD, AM_R_R, RT_E, RT_H);
    case 0x5D:
        return opcode(std::string("0x5D"), OP_LD, AM_R_R, RT_E, RT_L);
    case 0x5E:
        return opcode(std::string("0x5E"), OP_LD, AM_R_MR, RT_E, RT_HL);
    case 0x5F:
        return opcode(std::string("0x5F"), OP_LD, AM_R_R, RT_E, RT_A);
    case 0x60:
        return opcode(std::string("0x60"), OP_LD, AM_R_R, RT_H, RT_B);
    case 0x61:
        return opcode(std::string("0x61"), OP_LD, AM_R_R, RT_H, RT_C);
    case 0x62:
        return opcode(std::string("0x62"), OP_LD, AM_R_R, RT_H, RT_D);
    case 0x63:
        return opcode(std::string("0x63"), OP_LD, AM_R_R, RT_H, RT_E);
    case 0x64:
        return opcode(std::string("0x64"), OP_LD, AM_R_R, RT_H, RT_H);
    case 0x65:
        return opcode(std::string("0x65"), OP_LD, AM_R_R, RT_H, RT_L);
    case 0x66:
        return opcode(std::string("0x66"), OP_LD, AM_R_MR, RT_H, RT_HL);
    case 0x67:
        return opcode(std::string("0x67"), OP_LD, AM_R_R, RT_H, RT_A);
    case 0x68:
        return opcode(std::string("0x68"), OP_LD, AM_R_R, RT_L, RT_B);
    case 0x69:
        return opcode(std::string("0x69"), OP_LD, AM_R_R, RT_L, RT_C);
    case 0x6A:
        return opcode(std::string("0x6A"), OP_LD, AM_R_R, RT_L, RT_D);
    case 0x6B:
        return opcode(std::string("0x6B"), OP_LD, AM_R_R, RT_L, RT_E);
    case 0x6C:
        return opcode(std::string("0x6C"), OP_LD, AM_R_R, RT_L, RT_H);
    case 0x6D:
        return opcode(std::string("0x6D"), OP_LD, AM_R_R, RT_L, RT_L);
    case 0x6E:
        return opcode(std::string("0x6E"), OP_LD, AM_R_MR, RT_L, RT_HL);
    case 0x6F:
        return opcode(std::string("0x6F"), OP_LD, AM_R_R, RT_L, RT_A);
    case 0x70:
        return opcode(std::string("0x70"), OP_LD, AM_MR_R, RT_HL, RT_B);
    case 0x71:
        return opcode(std::string("0x71"), OP_LD, AM_MR_R, RT_HL, RT_C);
    case 0x72:
        return opcode(std::string("0x72"), OP_LD, AM_MR_R, RT_HL, RT_D);
    case 0x73:
        return opcode(std::string("0x73"), OP_LD, AM_MR_R, RT_HL, RT_E);
    case 0x74:
        return opcode(std::string("0x74"), OP_LD, AM_MR_R, RT_HL, RT_H);
    case 0x75:
        return opcode(std::string("0x75"), OP_LD, AM_MR_R, RT_HL, RT_L);
    case 0x76:
        return opcode(std::string("0x76"), OP_HALT);
    case 0x77:
        return opcode(std::string("0x77"), OP_LD, AM_MR_R, RT_HL, RT_A);
    case 0x78:
        return opcode(std::string("0x78"), OP_LD, AM_R_R, RT_A, RT_B);
    case 0x79:
        return opcode(std::string("0x79"), OP_LD, AM_R_R, RT_A, RT_C);
    case 0x7A:
        return opcode(std::string("0x7A"), OP_LD, AM_R_R, RT_A, RT_D);
    case 0x7B:
        return opcode(std::string("0x7B"), OP_LD, AM_R_R, RT_A, RT_E);
    case 0x7C:
        return opcode(std::string("0x7C"), OP_LD, AM_R_R, RT_A, RT_H);
    case 0x7D:
        return opcode(std::string("0x7D"), OP_LD, AM_R_R, RT_A, RT_L);
    case 0x7E:
        return opcode(std::string("0x7E"), OP_LD, AM_R_MR, RT_A, RT_HL);
    case 0x7F:
        return opcode(std::string("0x7F"), OP_LD, AM_R_R, RT_A, RT_A);
    case 0x80:
        return opcode(std::string("0x80"), OP_ADD, AM_R_R, RT_A, RT_B);
    case 0x81:
        return opcode(std::string("0x81"), OP_ADD, AM_R_R, RT_A, RT_C);
    case 0x82:
        return opcode(std::string("0x82"), OP_ADD, AM_R_R, RT_A, RT_D);
    case 0x83:
        return opcode(std::string("0x83"), OP_ADD, AM_R_R, RT_A, RT_E);
    case 0x84:
        return opcode(std::string("0x84"), OP_ADD, AM_R_R, RT_A, RT_H);
    case 0x85:
        return opcode(std::string("0x85"), OP_ADD, AM_R_R, RT_A, RT_L);
    case 0x86:
        return opcode(std::string("0x86"), OP_ADD, AM_R_MR, RT_A, RT_HL);
    case 0x87:
        return opcode(std::string("0x87"), OP_ADD, AM_R_R, RT_A, RT_A);
    case 0x88:
        return opcode(std::string("0x88"), OP_ADC, AM_R_R, RT_A, RT_B);
    case 0x89:
        return opcode(std::string("0x89"), OP_ADC, AM_R_R, RT_A, RT_C);
    case 0x8A:
        return opcode(std::string("0x8A"), OP_ADC, AM_R_R, RT_A, RT_D);
    case 0x8B:
        return opcode(std::string("0x8B"), OP_ADC, AM_R_R, RT_A, RT_E);
    case 0x8C:
        return opcode(std::string("0x8C"), OP_ADC, AM_R_R, RT_A, RT_H);
    case 0x8D:
        return opcode(std::string("0x8D"), OP_ADC, AM_R_R, RT_A, RT_L);
    case 0x8E:
        return opcode(std::string("0x8E"), OP_ADC, AM_R_MR, RT_A, RT_HL);
    case 0x8F:
        return opcode(std::string("0x8F"), OP_ADC, AM_R_R, RT_A, RT_A);
    case 0x90:
        return opcode(std::string("0x90"), OP_SUB, AM_R_R, RT_A, RT_B);
    case 0x91:
        return opcode(std::string("0x91"), OP_SUB, AM_R_R, RT_A, RT_C);
    case 0x92:
        return opcode(std::string("0x92"), OP_SUB, AM_R_R, RT_A, RT_D);
    case 0x93:
        return opcode(std::string("0x93"), OP_SUB, AM_R_R, RT_A, RT_E);
    case 0x94:
        return opcode(std::string("0x94"), OP_SUB, AM_R_R, RT_A, RT_H);
    case 0x95:
        return opcode(std::string("0x95"), OP_SUB, AM_R_R, RT_A, RT_L);
    case 0x96:
        return opcode(std::string("0x96"), OP_SUB, AM_R_MR, RT_A, RT_HL);
    case 0x97:
        return opcode(std::string("0x97"), OP_SUB, AM_R_R, RT_A, RT_A);
    case 0x98:
        return opcode(std::string("0x98"), OP_SBC, AM_R_R, RT_A, RT_B);
    case 0x99:
        return opcode(std::string("0x99"), OP_SBC, AM_R_R, RT_A, RT_C);
    case 0x9A:
        return opcode(std::string("0x9A"), OP_SBC, AM_R_R, RT_A, RT_D);
    case 0x9B:
        return opcode(std::string("0x9B"), OP_SBC, AM_R_R, RT_A, RT_E);
    case 0x9C:
        return opcode(std::string("0x9C"), OP_SBC, AM_R_R, RT_A, RT_H);
    case 0x9D:
        return opcode(std::string("0x9D"), OP_SBC, AM_R_R, RT_A, RT_L);
    case 0x9E:
        return opcode(std::string("0x9E"), OP_SBC, AM_R_MR, RT_A, RT_HL);
    case 0x9F:
        return opcode(std::string("0x9F"), OP_SBC, AM_R_R, RT_A, RT_A);
    case 0xA0:
        return opcode(std::string("0xA0"), OP_AND, AM_R_R, RT_A, RT_B);
    case 0xA1:
        return opcode(std::string("0xA1"), OP_AND, AM_R_R, RT_A, RT_C);
    case 0xA2:
        return opcode(std::string("0xA2"), OP_AND, AM_R_R, RT_A, RT_D);
    case 0xA3:
        return opcode(std::string("0xA3"), OP_AND, AM_R_R, RT_A, RT_E);
    case 0xA4:
        return opcode(std::string("0xA4"), OP_AND, AM_R_R, RT_A, RT_H);
    case 0xA5:
        return opcode(std::string("0xA5"), OP_AND, AM_R_R, RT_A, RT_L);
    case 0xA6:
        return opcode(std::string("0xA6"), OP_AND, AM_R_MR, RT_A, RT_HL);
    case 0xA7:
        return opcode(std::string("0xA7"), OP_AND, AM_R_R, RT_A, RT_A);
    case 0xA8:
        return opcode(std::string("0xA8"), OP_XOR, AM_R_R, RT_A, RT_B);
    case 0xA9:
        return opcode(std::string("0xA9"), OP_XOR, AM_R_R, RT_A, RT_C);
    case 0xAA:
        return opcode(std::string("0xAA"), OP_XOR, AM_R_R, RT_A, RT_D);
    case 0xAB:
        return opcode(std::string("0xAB"), OP_XOR, AM_R_R, RT_A, RT_E);
    case 0xAC:
        return opcode(std::string("0xAC"), OP_XOR, AM_R_R, RT_A, RT_H);
    case 0xAD:
        return opcode(std::string("0xAD"), OP_XOR, AM_R_R, RT_A, RT_L);
    case 0xAE:
        return opcode(std::string("0xAE"), OP_XOR, AM_R_MR, RT_A, RT_HL);
    case 0xAF:
        return opcode(std::string("0xAF"), OP_XOR, AM_R_R, RT_A, RT_A);
    case 0xB0:
        return opcode(std::string("0xB0"), OP_OR, AM_R_R, RT_A, RT_B);
    case 0xB1:
        return opcode(std::string("0xB1"), OP_OR, AM_R_R, RT_A, RT_C);
    case 0xB2:
        return opcode(std::string("0xB2"), OP_OR, AM_R_R, RT_A, RT_D);
    case 0xB3:
        return opcode(std::string("0xB3"), OP_OR, AM_R_R, RT_A, RT_E);
    case 0xB4:
        return opcode(std::string("0xB4"), OP_OR, AM_R_R, RT_A, RT_H);
    case 0xB5:
        return opcode(std::string("0xB5"), OP_OR, AM_R_R, RT_A, RT_L);
    case 0xB6:
        return opcode(std::string("0xB6"), OP_OR, AM_R_MR, RT_A, RT_HL);
    case 0xB7:
        return opcode(std::string("0xB7"), OP_OR, AM_R_R, RT_A, RT_A);
    case 0xB8:
        return opcode(std::string("0xB8"), OP_CP, AM_R_R, RT_A, RT_B);
    case 0xB9:
        return opcode(std::string("0xB9"), OP_CP, AM_R_R, RT_A, RT_C);
    case 0xBA:
        return opcode(std::string("0xBA"), OP_CP, AM_R_R, RT_A, RT_D);
    case 0xBB:
        return opcode(std::string("0xBB"), OP_CP, AM_R_R, RT_A, RT_E);
    case 0xBC:
        return opcode(std::string("0xBC"), OP_CP, AM_R_R, RT_A, RT_H);
    case 0xBD:
        return opcode(std::string("0xBD"), OP_CP, AM_R_R, RT_A, RT_L);
    case 0xBE:
        return opcode(std::string("0xBE"), OP_CP, AM_R_MR, RT_A, RT_HL);
    case 0xBF:
        return opcode(std::string("0xBF"), OP_CP, AM_R_R, RT_A, RT_A);
    case 0xC0:
        return opcode(std::string("0xC0"), OP_RET, AM_IMP, RT_NONE, RT_NONE, CT_NZ);
    case 0xC1:
        return opcode(std::string("0xC1"), OP_POP, AM_R, RT_BC);
    case 0xC2:
        return opcode(std::string("0xC2"), OP_JP, AM_D16, RT_NONE, RT_NONE, CT_NZ);
    case 0xC3:
        return opcode(std::string("0xC3"), OP_JP, AM_D16);
    case 0xC4:
        return opcode(std::string("0xC4"), OP_CALL, AM_D16, RT_NONE, RT_NONE, CT_NZ);
    case 0xC5:
        return opcode(std::string("0xC5"), OP_PUSH, AM_R, RT_BC);
    case 0xC6:
        return opcode(std::string("0xC6"), OP_ADD, AM_R_D8, RT_A);
    case 0xC7:
        return opcode(std::string("0xC7"), OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x00);
    case 0xC8:
        return opcode(std::string("0xC8"), OP_RET, AM_IMP, RT_NONE, RT_NONE, CT_Z);
    case 0xC9:
        return opcode(std::string("0xC9"), OP_RET);
    case 0xCA:
        return opcode(std::string("0xCA"), OP_JP, AM_D16, RT_NONE, RT_NONE, CT_Z);
    case 0xCB:
        return opcode(std::string("0xCB"), OP_CB);
    case 0xCC:
        return opcode(std::string("0xCC"), OP_CALL, AM_D16, RT_NONE, RT_NONE, CT_Z);
    case 0xCD:
        return opcode(std::string("0xCD"), OP_CALL, AM_D16);
    case 0xCE:
        return opcode(std::string("0xCE"), OP_ADC, AM_R_D8, RT_A);
    case 0xCF:
        return opcode(std::string("0xCF"), OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x08);
    case 0xD0:
        return opcode(std::string("0xD0"), OP_RET, AM_IMP, RT_NONE, RT_NONE, CT_NC);
    case 0xD1:
        return opcode(std::string("0xD1"), OP_POP, AM_R, RT_DE);
    case 0xD2:
        return opcode(std::string("0xD2"), OP_JP, AM_D16, RT_NONE, RT_NONE, CT_NC);
    //case 0xD3:
        //return opcode(std::string("0xD3"), OP_INVALID);
    case 0xD4:
        return opcode(std::string("0xD4"), OP_CALL, AM_D16, RT_NONE, RT_NONE, CT_NC);
    case 0xD5:
        return opcode(std::string("0xD5"), OP_PUSH, AM_R, RT_DE);
    case 0xD6:
        return opcode(std::string("0xD6"), OP_SUB, AM_R_D8, RT_A);
    case 0xD7:
        return opcode(std::string("0xD7"), OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x10);
    case 0xD8:
        return opcode(std::string("0xD8"), OP_RET, AM_IMP, RT_NONE, RT_NONE, CT_C);
    case 0xD9:
        return opcode(std::string("0xD9"), OP_RETI);
    case 0xDA:
        return opcode(std::string("0xDA"), OP_JP, AM_D16, RT_NONE, RT_NONE, CT_C);
    case 0xDC:
        return opcode(std::string("0xDC"), OP_CALL, AM_D16, RT_NONE, RT_NONE, CT_C);
    case 0xDE:
        return opcode(std::string("0xDE"), OP_SBC, AM_R_D8, RT_A);
    case 0xDF:
        return opcode(std::string("0xDF"), OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x18);
    case 0xE0:
        return opcode(std::string("0xE0"), OP_LDH, AM_A8_R, RT_NONE, RT_A);
    case 0xE1:
        return opcode(std::string("0xE1"), OP_POP, AM_R, RT_HL);
    case 0xE2:
        return opcode(std::string("0xE2"), OP_LDH, AM_MR_R, RT_C, RT_A);
    case 0xE5:
        return opcode(std::string("0xE5"), OP_PUSH, AM_R, RT_HL);
    case 0xE6:
        return opcode(std::string("0xE6"), OP_AND, AM_R_D8, RT_A);
    case 0xE7:
        return opcode(std::string("0xE7"), OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x20);
    case 0xE8:
        return opcode(std::string("0xE8"), OP_ADD, AM_R_D8, RT_SP);
    case 0xE9:
        return opcode(std::string("0xE9"), OP_JP, AM_R, RT_HL);
    case 0xEA:
        return opcode(std::string("0xEA"), OP_LD, AM_MR_R, RT_NONE, RT_A);
    case 0xEE:
        return opcode(std::string("0xEE"), OP_XOR, AM_R_D8, RT_A);
    case 0xEF:
        return opcode(std::string("0xEF"), OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x28);
    case 0xF0:
        return opcode(std::string("0xF0"), OP_LDH, AM_R_MR, RT_A);
    case 0xF1:
        return opcode(std::string("0xF1"), OP_POP, AM_R, RT_AF);
    case 0xF2:
        return opcode(std::string("0xF2"), OP_LDH, AM_R_MR, RT_A, RT_C);
    case 0xF3:
        return opcode(std::string("0xF3"), OP_DI);
    case 0xF5:
        return opcode(std::string("0xF5"), OP_PUSH, AM_R, RT_AF);
    case 0xF6:
        return opcode(std::string("0xF6"), OP_OR, AM_R_D8, RT_A);
    case 0xF7:
        return opcode(std::string("0xF7"), OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x30);
    case 0xF8:
        return opcode(std::string("0xF8"), OP_LD, AM_HL_SPR, RT_HL, RT_SP);
    case 0xF9:
        return opcode(std::string("0xF9"), OP_LD, AM_R_R, RT_SP, RT_HL);
    case 0xFA:
        return opcode(std::string("0xFA"), OP_LD, AM_R_MR, RT_A, RT_A);
    case 0xFB:
        return opcode(std::string("0xFB"), OP_EI);
    case 0xFE:
        return opcode(std::string("0xFE"), OP_CP, AM_R_D8, RT_A);
    case 0xFF:
        return opcode(std::string("0xFF"), OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x38);
    default:
        return opcode(std::string("0x00"), OP_NOP);
    }
}