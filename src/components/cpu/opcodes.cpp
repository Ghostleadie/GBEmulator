//
// Created by Jack_ on 01/09/2025.
//

#include "opcodes.h"
#include "opcodeFunctions.h"
#include "cpu.h"

 const std::unordered_map<uint8_t, opcode> opcodeTable =
	{
	    // 0x0X
	    {0x00, {"0x00", OP_NOP, AM_IMP}},
	    {0x01, {"0x01", OP_LD, AM_R_D16, RT_BC}},
	    {0x02, {"0x02", OP_LD, AM_MR_R, RT_BC, RT_A}},
	    {0x03, {"0x03", OP_INC, AM_R, RT_BC}},
	    {0x04, {"0x04", OP_INC, AM_R, RT_B}},
	    {0x05, {"0x05", OP_DEC, AM_R, RT_B}},
	    {0x06, {"0x06", OP_LD, AM_R_D8, RT_B}},
	    {0x07, {"0x07", OP_RLCA, AM_IMP}},
	    {0x08, {"0x08", OP_LD, AM_A16_R, RT_NONE, RT_SP}},
	    {0x09, {"0x09", OP_ADD, AM_R_R, RT_HL, RT_BC}},
	    {0x0A, {"0x0A", OP_LD, AM_R_MR, RT_A, RT_BC}},
	    {0x0B, {"0x0B", OP_DEC, AM_R, RT_BC}},
	    {0x0C, {"0x0C", OP_INC, AM_R, RT_C}},
	    {0x0D, {"0x0D", OP_DEC, AM_R, RT_C}},
	    {0x0E, {"0x0E", OP_LD, AM_R_D8, RT_C}},
	    {0x0F, {"0x0F", OP_RRCA, AM_IMP}},

	    // 0x1X
	    {0x10, {"0x10", OP_STOP, AM_D8}},
	    {0x11, {"0x11", OP_LD, AM_R_D16, RT_DE}},
	    {0x12, {"0x12", OP_LD, AM_MR_R, RT_DE, RT_A}},
	    {0x13, {"0x13", OP_INC, AM_R, RT_DE}},
	    {0x14, {"0x14", OP_INC, AM_R, RT_D}},
	    {0x15, {"0x15", OP_DEC, AM_R, RT_D}},
	    {0x16, {"0x16", OP_LD, AM_R_D8, RT_D}},
	    {0x17, {"0x17", OP_RLA, AM_IMP}},
	    {0x18, {"0x18", OP_JR, AM_D8}},
	    {0x19, {"0x19", OP_ADD, AM_R_R, RT_HL, RT_DE}},
	    {0x1A, {"0x1A", OP_LD, AM_R_MR, RT_A, RT_DE}},
	    {0x1B, {"0x1B", OP_DEC, AM_R, RT_DE}},
	    {0x1C, {"0x1C", OP_INC, AM_R, RT_E}},
	    {0x1D, {"0x1D", OP_DEC, AM_R, RT_E}},
	    {0x1E, {"0x1E", OP_LD, AM_R_D8, RT_E}},
	    {0x1F, {"0x1F", OP_RRA, AM_IMP}},

	    // 0x2X
	    {0x20, {"0x20", OP_JR, AM_D8, RT_NONE, RT_NONE, CT_NZ}},
	    {0x21, {"0x21", OP_LD, AM_R_D16, RT_HL}},
	    {0x22, {"0x22", OP_LD, AM_HLI_R, RT_HL, RT_A}},
	    {0x23, {"0x23", OP_INC, AM_R, RT_HL}},
	    {0x24, {"0x24", OP_INC, AM_R, RT_H}},
	    {0x25, {"0x25", OP_DEC, AM_R, RT_H}},
	    {0x26, {"0x26", OP_LD, AM_R_D8, RT_H}},
	    {0x27, {"0x27", OP_DAA, AM_IMP}},
	    {0x28, {"0x28", OP_JR, AM_D8, RT_NONE, RT_NONE, CT_Z}},
	    {0x29, {"0x29", OP_ADD, AM_R_R, RT_HL, RT_HL}},
	    {0x2A, {"0x2A", OP_LD, AM_R_HLI, RT_A, RT_HL}},
	    {0x2B, {"0x2B", OP_DEC, AM_R, RT_HL}},
	    {0x2C, {"0x2C", OP_INC, AM_R, RT_L}},
	    {0x2D, {"0x2D", OP_DEC, AM_R, RT_L}},
	    {0x2E, {"0x2E", OP_LD, AM_R_D8, RT_L}},
	    {0x2F, {"0x2F", OP_CPL, AM_IMP}},

	    // 0x3X
	    {0x30, {"0x30", OP_JR, AM_D8, RT_NONE, RT_NONE, CT_NC}},
	    {0x31, {"0x31", OP_LD, AM_R_D16, RT_SP}},
	    {0x32, {"0x32", OP_LD, AM_HLD_R, RT_HL, RT_A}},
	    {0x33, {"0x33", OP_INC, AM_R, RT_SP}},
	    {0x34, {"0x34", OP_INC, AM_MR, RT_HL}},
	    {0x35, {"0x35", OP_DEC, AM_MR, RT_HL}},
	    {0x36, {"0x36", OP_LD, AM_MR_D8, RT_HL}},
	    {0x37, {"0x37", OP_SCF, AM_IMP}},
	    {0x38, {"0x38", OP_JR, AM_D8, RT_NONE, RT_NONE, CT_C}},
	    {0x39, {"0x39", OP_ADD, AM_R_R, RT_HL, RT_SP}},
	    {0x3A, {"0x3A", OP_LD, AM_R_HLD, RT_A, RT_HL}},
	    {0x3B, {"0x3B", OP_DEC, AM_R, RT_SP}},
	    {0x3C, {"0x3C", OP_INC, AM_R, RT_A}},
	    {0x3D, {"0x3D", OP_DEC, AM_R, RT_A}},
	    {0x3E, {"0x3E", OP_LD, AM_R_D8, RT_A}},
	    {0x3F, {"0x3F", OP_CCF, AM_IMP}},

	    // 0x4X - LD B,r
	    {0x40, {"0x40", OP_LD, AM_R_R, RT_B, RT_B}},
	    {0x41, {"0x41", OP_LD, AM_R_R, RT_B, RT_C}},
	    {0x42, {"0x42", OP_LD, AM_R_R, RT_B, RT_D}},
	    {0x43, {"0x43", OP_LD, AM_R_R, RT_B, RT_E}},
	    {0x44, {"0x44", OP_LD, AM_R_R, RT_B, RT_H}},
	    {0x45, {"0x45", OP_LD, AM_R_R, RT_B, RT_L}},
	    {0x46, {"0x46", OP_LD, AM_R_MR, RT_B, RT_HL}},
	    {0x47, {"0x47", OP_LD, AM_R_R, RT_B, RT_A}},
	    {0x48, {"0x48", OP_LD, AM_R_R, RT_C, RT_B}},
	    {0x49, {"0x49", OP_LD, AM_R_R, RT_C, RT_C}},
	    {0x4A, {"0x4A", OP_LD, AM_R_R, RT_C, RT_D}},
	    {0x4B, {"0x4B", OP_LD, AM_R_R, RT_C, RT_E}},
	    {0x4C, {"0x4C", OP_LD, AM_R_R, RT_C, RT_H}},
	    {0x4D, {"0x4D", OP_LD, AM_R_R, RT_C, RT_L}},
	    {0x4E, {"0x4E", OP_LD, AM_R_MR, RT_C, RT_HL}},
	    {0x4F, {"0x4F", OP_LD, AM_R_R, RT_C, RT_A}},

	    // 0x5X - LD D,r
	    {0x50, {"0x50", OP_LD, AM_R_R, RT_D, RT_B}},
	    {0x51, {"0x51", OP_LD, AM_R_R, RT_D, RT_C}},
	    {0x52, {"0x52", OP_LD, AM_R_R, RT_D, RT_D}},
	    {0x53, {"0x53", OP_LD, AM_R_R, RT_D, RT_E}},
	    {0x54, {"0x54", OP_LD, AM_R_R, RT_D, RT_H}},
	    {0x55, {"0x55", OP_LD, AM_R_R, RT_D, RT_L}},
	    {0x56, {"0x56", OP_LD, AM_R_MR, RT_D, RT_HL}},
	    {0x57, {"0x57", OP_LD, AM_R_R, RT_D, RT_A}},
	    {0x58, {"0x58", OP_LD, AM_R_R, RT_E, RT_B}},
	    {0x59, {"0x59", OP_LD, AM_R_R, RT_E, RT_C}},
	    {0x5A, {"0x5A", OP_LD, AM_R_R, RT_E, RT_D}},
	    {0x5B, {"0x5B", OP_LD, AM_R_R, RT_E, RT_E}},
	    {0x5C, {"0x5C", OP_LD, AM_R_R, RT_E, RT_H}},
	    {0x5D, {"0x5D", OP_LD, AM_R_R, RT_E, RT_L}},
	    {0x5E, {"0x5E", OP_LD, AM_R_MR, RT_E, RT_HL}},
	    {0x5F, {"0x5F", OP_LD, AM_R_R, RT_E, RT_A}},

	    // 0x6X - LD H,r
	    {0x60, {"0x60", OP_LD, AM_R_R, RT_H, RT_B}},
	    {0x61, {"0x61", OP_LD, AM_R_R, RT_H, RT_C}},
	    {0x62, {"0x62", OP_LD, AM_R_R, RT_H, RT_D}},
	    {0x63, {"0x63", OP_LD, AM_R_R, RT_H, RT_E}},
	    {0x64, {"0x64", OP_LD, AM_R_R, RT_H, RT_H}},
	    {0x65, {"0x65", OP_LD, AM_R_R, RT_H, RT_L}},
	    {0x66, {"0x66", OP_LD, AM_R_MR, RT_H, RT_HL}},
	    {0x67, {"0x67", OP_LD, AM_R_R, RT_H, RT_A}},
	    {0x68, {"0x68", OP_LD, AM_R_R, RT_L, RT_B}},
	    {0x69, {"0x69", OP_LD, AM_R_R, RT_L, RT_C}},
	    {0x6A, {"0x6A", OP_LD, AM_R_R, RT_L, RT_D}},
	    {0x6B, {"0x6B", OP_LD, AM_R_R, RT_L, RT_E}},
	    {0x6C, {"0x6C", OP_LD, AM_R_R, RT_L, RT_H}},
	    {0x6D, {"0x6D", OP_LD, AM_R_R, RT_L, RT_L}},
	    {0x6E, {"0x6E", OP_LD, AM_R_MR, RT_L, RT_HL}},
	    {0x6F, {"0x6F", OP_LD, AM_R_R, RT_L, RT_A}},

	    // 0x7X - LD (HL),r and LD A,r
	    {0x70, {"0x70", OP_LD, AM_MR_R, RT_HL, RT_B}},
	    {0x71, {"0x71", OP_LD, AM_MR_R, RT_HL, RT_C}},
	    {0x72, {"0x72", OP_LD, AM_MR_R, RT_HL, RT_D}},
	    {0x73, {"0x73", OP_LD, AM_MR_R, RT_HL, RT_E}},
	    {0x74, {"0x74", OP_LD, AM_MR_R, RT_HL, RT_H}},
	    {0x75, {"0x75", OP_LD, AM_MR_R, RT_HL, RT_L}},
	    {0x76, {"0x76", OP_HALT, AM_IMP}},
	    {0x77, {"0x77", OP_LD, AM_MR_R, RT_HL, RT_A}},
	    {0x78, {"0x78", OP_LD, AM_R_R, RT_A, RT_B}},
	    {0x79, {"0x79", OP_LD, AM_R_R, RT_A, RT_C}},
	    {0x7A, {"0x7A", OP_LD, AM_R_R, RT_A, RT_D}},
	    {0x7B, {"0x7B", OP_LD, AM_R_R, RT_A, RT_E}},
	    {0x7C, {"0x7C", OP_LD, AM_R_R, RT_A, RT_H}},
	    {0x7D, {"0x7D", OP_LD, AM_R_R, RT_A, RT_L}},
	    {0x7E, {"0x7E", OP_LD, AM_R_MR, RT_A, RT_HL}},
	    {0x7F, {"0x7F", OP_LD, AM_R_R, RT_A, RT_A}},

	    // 0x8X - ADD A,r
	    {0x80, {"0x80", OP_ADD, AM_R_R, RT_A, RT_B}},
	    {0x81, {"0x81", OP_ADD, AM_R_R, RT_A, RT_C}},
	    {0x82, {"0x82", OP_ADD, AM_R_R, RT_A, RT_D}},
	    {0x83, {"0x83", OP_ADD, AM_R_R, RT_A, RT_E}},
	    {0x84, {"0x84", OP_ADD, AM_R_R, RT_A, RT_H}},
	    {0x85, {"0x85", OP_ADD, AM_R_R, RT_A, RT_L}},
	    {0x86, {"0x86", OP_ADD, AM_R_MR, RT_A, RT_HL}},
	    {0x87, {"0x87", OP_ADD, AM_R_R, RT_A, RT_A}},
	    {0x88, {"0x88", OP_ADC, AM_R_R, RT_A, RT_B}},
	    {0x89, {"0x89", OP_ADC, AM_R_R, RT_A, RT_C}},
	    {0x8A, {"0x8A", OP_ADC, AM_R_R, RT_A, RT_D}},
	    {0x8B, {"0x8B", OP_ADC, AM_R_R, RT_A, RT_E}},
	    {0x8C, {"0x8C", OP_ADC, AM_R_R, RT_A, RT_H}},
	    {0x8D, {"0x8D", OP_ADC, AM_R_R, RT_A, RT_L}},
	    {0x8E, {"0x8E", OP_ADC, AM_R_MR, RT_A, RT_HL}},
	    {0x8F, {"0x8F", OP_ADC, AM_R_R, RT_A, RT_A}},

	    // 0x9X - SUB r
	    {0x90, {"0x90", OP_SUB, AM_R_R, RT_A, RT_B}},
	    {0x91, {"0x91", OP_SUB, AM_R_R, RT_A, RT_C}},
	    {0x92, {"0x92", OP_SUB, AM_R_R, RT_A, RT_D}},
	    {0x93, {"0x93", OP_SUB, AM_R_R, RT_A, RT_E}},
	    {0x94, {"0x94", OP_SUB, AM_R_R, RT_A, RT_H}},
	    {0x95, {"0x95", OP_SUB, AM_R_R, RT_A, RT_L}},
	    {0x96, {"0x96", OP_SUB, AM_R_MR, RT_A, RT_HL}},
	    {0x97, {"0x97", OP_SUB, AM_R_R, RT_A, RT_A}},
	    {0x98, {"0x98", OP_SBC, AM_R_R, RT_A, RT_B}},
	    {0x99, {"0x99", OP_SBC, AM_R_R, RT_A, RT_C}},
	    {0x9A, {"0x9A", OP_SBC, AM_R_R, RT_A, RT_D}},
	    {0x9B, {"0x9B", OP_SBC, AM_R_R, RT_A, RT_E}},
	    {0x9C, {"0x9C", OP_SBC, AM_R_R, RT_A, RT_H}},
	    {0x9D, {"0x9D", OP_SBC, AM_R_R, RT_A, RT_L}},
	    {0x9E, {"0x9E", OP_SBC, AM_R_MR, RT_A, RT_HL}},
	    {0x9F, {"0x9F", OP_SBC, AM_R_R, RT_A, RT_A}},

	    // 0xAX - AND r
	    {0xA0, {"0xA0", OP_AND, AM_R_R, RT_A, RT_B}},
	    {0xA1, {"0xA1", OP_AND, AM_R_R, RT_A, RT_C}},
	    {0xA2, {"0xA2", OP_AND, AM_R_R, RT_A, RT_D}},
	    {0xA3, {"0xA3", OP_AND, AM_R_R, RT_A, RT_E}},
	    {0xA4, {"0xA4", OP_AND, AM_R_R, RT_A, RT_H}},
	    {0xA5, {"0xA5", OP_AND, AM_R_R, RT_A, RT_L}},
	    {0xA6, {"0xA6", OP_AND, AM_R_MR, RT_A, RT_HL}},
	    {0xA7, {"0xA7", OP_AND, AM_R_R, RT_A, RT_A}},
	    {0xA8, {"0xA8", OP_XOR, AM_R_R, RT_A, RT_B}},
	    {0xA9, {"0xA9", OP_XOR, AM_R_R, RT_A, RT_C}},
	    {0xAA, {"0xAA", OP_XOR, AM_R_R, RT_A, RT_D}},
	    {0xAB, {"0xAB", OP_XOR, AM_R_R, RT_A, RT_E}},
	    {0xAC, {"0xAC", OP_XOR, AM_R_R, RT_A, RT_H}},
	    {0xAD, {"0xAD", OP_XOR, AM_R_R, RT_A, RT_L}},
	    {0xAE, {"0xAE", OP_XOR, AM_R_MR, RT_A, RT_HL}},
	    {0xAF, {"0xAF", OP_XOR, AM_R_R, RT_A, RT_A}},

	    // 0xBX - OR r and CP r
	    {0xB0, {"0xB0", OP_OR, AM_R_R, RT_A, RT_B}},
	    {0xB1, {"0xB1", OP_OR, AM_R_R, RT_A, RT_C}},
	    {0xB2, {"0xB2", OP_OR, AM_R_R, RT_A, RT_D}},
	    {0xB3, {"0xB3", OP_OR, AM_R_R, RT_A, RT_E}},
	    {0xB4, {"0xB4", OP_OR, AM_R_R, RT_A, RT_H}},
	    {0xB5, {"0xB5", OP_OR, AM_R_R, RT_A, RT_L}},
	    {0xB6, {"0xB6", OP_OR, AM_R_MR, RT_A, RT_HL}},
	    {0xB7, {"0xB7", OP_OR, AM_R_R, RT_A, RT_A}},
	    {0xB8, {"0xB8", OP_CP, AM_R_R, RT_A, RT_B}},
	    {0xB9, {"0xB9", OP_CP, AM_R_R, RT_A, RT_C}},
	    {0xBA, {"0xBA", OP_CP, AM_R_R, RT_A, RT_D}},
	    {0xBB, {"0xBB", OP_CP, AM_R_R, RT_A, RT_E}},
	    {0xBC, {"0xBC", OP_CP, AM_R_R, RT_A, RT_H}},
	    {0xBD, {"0xBD", OP_CP, AM_R_R, RT_A, RT_L}},
	    {0xBE, {"0xBE", OP_CP, AM_R_MR, RT_A, RT_HL}},
	    {0xBF, {"0xBF", OP_CP, AM_R_R, RT_A, RT_A}},

	    // 0xCX
	    {0xC0, {"0xC0", OP_RET, AM_IMP, RT_NONE, RT_NONE, CT_NZ}},
	    {0xC1, {"0xC1", OP_POP, AM_R, RT_BC}},
	    {0xC2, {"0xC2", OP_JP, AM_D16, RT_NONE, RT_NONE, CT_NZ}},
	    {0xC3, {"0xC3", OP_JP, AM_D16}},
	    {0xC4, {"0xC4", OP_CALL, AM_D16, RT_NONE, RT_NONE, CT_NZ}},
	    {0xC5, {"0xC5", OP_PUSH, AM_R, RT_BC}},
	    {0xC6, {"0xC6", OP_ADD, AM_R_D8, RT_A}},
	    {0xC7, {"0xC7", OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x00}},
	    {0xC8, {"0xC8", OP_RET, AM_IMP, RT_NONE, RT_NONE, CT_Z}},
	    {0xC9, {"0xC9", OP_RET, AM_IMP}},
	    {0xCA, {"0xCA", OP_JP, AM_D16, RT_NONE, RT_NONE, CT_Z}},
	    {0xCB, {"0xCB", OP_CB, AM_D8}},
	    {0xCC, {"0xCC", OP_CALL, AM_D16, RT_NONE, RT_NONE, CT_Z}},
	    {0xCD, {"0xCD", OP_CALL, AM_D16}},
	    {0xCE, {"0xCE", OP_ADC, AM_R_D8, RT_A}},
	    {0xCF, {"0xCF", OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x08}},

	    // 0xDX
	    {0xD0, {"0xD0", OP_RET, AM_IMP, RT_NONE, RT_NONE, CT_NC}},
	    {0xD1, {"0xD1", OP_POP, AM_R, RT_DE}},
	    {0xD2, {"0xD2", OP_JP, AM_D16, RT_NONE, RT_NONE, CT_NC}},
	    {0xD4, {"0xD4", OP_CALL, AM_D16, RT_NONE, RT_NONE, CT_NC}},
	    {0xD5, {"0xD5", OP_PUSH, AM_R, RT_DE}},
	    {0xD6, {"0xD6", OP_SUB, AM_R_D8, RT_A}},
	    {0xD7, {"0xD7", OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x10}},
	    {0xD8, {"0xD8", OP_RET, AM_IMP, RT_NONE, RT_NONE, CT_C}},
	    {0xD9, {"0xD9", OP_RETI, AM_IMP}},
	    {0xDA, {"0xDA", OP_JP, AM_D16, RT_NONE, RT_NONE, CT_C}},
	    {0xDC, {"0xDC", OP_CALL, AM_D16, RT_NONE, RT_NONE, CT_C}},
	    {0xDE, {"0xDE", OP_SBC, AM_R_D8, RT_A}},
	    {0xDF, {"0xDF", OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x18}},

	    // 0xEX
	    {0xE0, {"0xE0", OP_LDH, AM_A8_R, RT_NONE, RT_A}},
	    {0xE1, {"0xE1", OP_POP, AM_R, RT_HL}},
	    {0xE2, {"0xE2", OP_LD, AM_MR_R, RT_C, RT_A}},
	    {0xE5, {"0xE5", OP_PUSH, AM_R, RT_HL}},
	    {0xE6, {"0xE6", OP_AND, AM_R_D8, RT_A}},
	    {0xE7, {"0xE7", OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x20}},
	    {0xE8, {"0xE8", OP_ADD, AM_R_D8, RT_SP}},
	    {0xE9, {"0xE9", OP_JPHL, AM_IMP}},
	    {0xEA, {"0xEA", OP_LD, AM_A16_R, RT_NONE, RT_A}},
	    {0xEE, {"0xEE", OP_XOR, AM_R_D8, RT_A}},
	    {0xEF, {"0xEF", OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x28}},

	    // 0xFX
	    {0xF0, {"0xF0", OP_LDH, AM_R_A8, RT_A}},
	    {0xF1, {"0xF1", OP_POP, AM_R, RT_AF}},
	    {0xF2, {"0xF2", OP_LD, AM_R_MR, RT_A, RT_C}},
	    {0xF3, {"0xF3", OP_DI, AM_IMP}},
	    {0xF5, {"0xF5", OP_PUSH, AM_R, RT_AF}},
	    {0xF6, {"0xF6", OP_OR, AM_R_D8, RT_A}},
	    {0xF7, {"0xF7", OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x30}},
	    {0xF8, {"0xF8", OP_LD, AM_HL_SPR, RT_HL, RT_SP}},
	    {0xF9, {"0xF9", OP_LD, AM_R_R, RT_SP, RT_HL}},
	    {0xFA, {"0xFA", OP_LD, AM_R_A16, RT_A}},
	    {0xFB, {"0xFB", OP_EI, AM_IMP}},
	    {0xFE, {"0xFE", OP_CP, AM_R_D8, RT_A}},
	    {0xFF, {"0xFF", OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x38}}
	};

// Initialize execute functions after map creation
void initializeOpcodeExecution() {
	static bool initialized = false;
	if (initialized) return;

	// Cast away const for initialization - done once at startup
	auto& mutable_opcodes = const_cast<std::unordered_map<uint8_t, opcode>&>(opcodeTable);

	for (auto& [opcode_value, op] : mutable_opcodes) {
		op.execute = [opcode_value](cpu& c) {
			if (auto cmd = OpcodeCommandFactory::createCommand(opcode_value)) {
				cmd->execute(c);
			}
		};
	}
	initialized = true;
}

opcode getOpcode(uint8_t opcode_value) {
	initializeOpcodeExecution();
	const auto it = opcodeTable.find(opcode_value);
	return (it != opcodeTable.end()) ? it->second : opcodeTable.at(0x00); // fallback to NOP
}
