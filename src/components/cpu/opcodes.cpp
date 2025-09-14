//
// Created by Jack_ on 01/09/2025.
//

#include "opcodes.h"
#include "opcodeFunctions.h"
#include "cpu.h"

 const std::unordered_map<uint8_t, opcode> opcodeTable =
	{
	    // 0x0X
	    {0x00, {"NOP", OP_NOP, AM_IMP}},
	    {0x01, {"LD", OP_LD, AM_R_D16, RT_BC}},
	    {0x02, {"LD", OP_LD, AM_MR_R, RT_BC, RT_A}},
	    {0x03, {"INC", OP_INC, AM_R, RT_BC}},
	    {0x04, {"INC", OP_INC, AM_R, RT_B}},
	    {0x05, {"DEC", OP_DEC, AM_R, RT_B}},
	    {0x06, {"LD", OP_LD, AM_R_D8, RT_B}},
	    {0x07, {"RLCA", OP_RLCA, AM_IMP}},
	    {0x08, {"LD", OP_LD, AM_A16_R, RT_NONE, RT_SP}},
	    {0x09, {"ADD", OP_ADD, AM_R_R, RT_HL, RT_BC}},
	    {0x0A, {"LD", OP_LD, AM_R_MR, RT_A, RT_BC}},
	    {0x0B, {"DEC", OP_DEC, AM_R, RT_BC}},
	    {0x0C, {"INC", OP_INC, AM_R, RT_C}},
	    {0x0D, {"DEC", OP_DEC, AM_R, RT_C}},
	    {0x0E, {"LD", OP_LD, AM_R_D8, RT_C}},
	    {0x0F, {"RRCA", OP_RRCA, AM_IMP}},

	    // 0x1X
	    {0x10, {"STOP", OP_STOP, AM_D8}},
	    {0x11, {"LD", OP_LD, AM_R_D16, RT_DE}},
	    {0x12, {"LD", OP_LD, AM_MR_R, RT_DE, RT_A}},
	    {0x13, {"INC", OP_INC, AM_R, RT_DE}},
	    {0x14, {"INC", OP_INC, AM_R, RT_D}},
	    {0x15, {"DEC", OP_DEC, AM_R, RT_D}},
	    {0x16, {"LD", OP_LD, AM_R_D8, RT_D}},
	    {0x17, {"RLA", OP_RLA, AM_IMP}},
	    {0x18, {"JR", OP_JR, AM_D8}},
	    {0x19, {"ADD", OP_ADD, AM_R_R, RT_HL, RT_DE}},
	    {0x1A, {"LD", OP_LD, AM_R_MR, RT_A, RT_DE}},
	    {0x1B, {"DEC", OP_DEC, AM_R, RT_DE}},
	    {0x1C, {"INC", OP_INC, AM_R, RT_E}},
	    {0x1D, {"DEC", OP_DEC, AM_R, RT_E}},
	    {0x1E, {"LD", OP_LD, AM_R_D8, RT_E}},
	    {0x1F, {"RRA", OP_RRA, AM_IMP}},

	    // 0x2X
	    {0x20, {"JR", OP_JR, AM_D8, RT_NONE, RT_NONE, CT_NZ}},
	    {0x21, {"LD", OP_LD, AM_R_D16, RT_HL}},
	    {0x22, {"LD", OP_LD, AM_HLI_R, RT_HL, RT_A}},
	    {0x23, {"INC", OP_INC, AM_R, RT_HL}},
	    {0x24, {"INC", OP_INC, AM_R, RT_H}},
	    {0x25, {"DEC", OP_DEC, AM_R, RT_H}},
	    {0x26, {"LD", OP_LD, AM_R_D8, RT_H}},
	    {0x27, {"DAA", OP_DAA, AM_IMP}},
	    {0x28, {"JR", OP_JR, AM_D8, RT_NONE, RT_NONE, CT_Z}},
	    {0x29, {"ADD", OP_ADD, AM_R_R, RT_HL, RT_HL}},
	    {0x2A, {"LD", OP_LD, AM_R_HLI, RT_A, RT_HL}},
	    {0x2B, {"DEC", OP_DEC, AM_R, RT_HL}},
	    {0x2C, {"INC", OP_INC, AM_R, RT_L}},
	    {0x2D, {"DEC", OP_DEC, AM_R, RT_L}},
	    {0x2E, {"LD", OP_LD, AM_R_D8, RT_L}},
	    {0x2F, {"CPL", OP_CPL, AM_IMP}},

	    // 0x3X
	    {0x30, {"JR", OP_JR, AM_D8, RT_NONE, RT_NONE, CT_NC}},
	    {0x31, {"LD", OP_LD, AM_R_D16, RT_SP}},
	    {0x32, {"LD", OP_LD, AM_HLD_R, RT_HL, RT_A}},
	    {0x33, {"INC", OP_INC, AM_R, RT_SP}},
	    {0x34, {"INC", OP_INC, AM_MR, RT_HL}},
	    {0x35, {"DEC", OP_DEC, AM_MR, RT_HL}},
	    {0x36, {"LD", OP_LD, AM_MR_D8, RT_HL}},
	    {0x37, {"SCF", OP_SCF, AM_IMP}},
	    {0x38, {"JR", OP_JR, AM_D8, RT_NONE, RT_NONE, CT_C}},
	    {0x39, {"ADD", OP_ADD, AM_R_R, RT_HL, RT_SP}},
	    {0x3A, {"LD", OP_LD, AM_R_HLD, RT_A, RT_HL}},
	    {0x3B, {"DEC", OP_DEC, AM_R, RT_SP}},
	    {0x3C, {"INC", OP_INC, AM_R, RT_A}},
	    {0x3D, {"DEC", OP_DEC, AM_R, RT_A}},
	    {0x3E, {"LD", OP_LD, AM_R_D8, RT_A}},
	    {0x3F, {"CCF", OP_CCF, AM_IMP}},

	    // 0x4X - LD B,r
	    {0x40, {"LD", OP_LD, AM_R_R, RT_B, RT_B}},
	    {0x41, {"LD", OP_LD, AM_R_R, RT_B, RT_C}},
	    {0x42, {"LD", OP_LD, AM_R_R, RT_B, RT_D}},
	    {0x43, {"LD", OP_LD, AM_R_R, RT_B, RT_E}},
	    {0x44, {"LD", OP_LD, AM_R_R, RT_B, RT_H}},
	    {0x45, {"LD", OP_LD, AM_R_R, RT_B, RT_L}},
	    {0x46, {"LD", OP_LD, AM_R_MR, RT_B, RT_HL}},
	    {0x47, {"LD", OP_LD, AM_R_R, RT_B, RT_A}},
	    {0x48, {"LD", OP_LD, AM_R_R, RT_C, RT_B}},
	    {0x49, {"LD", OP_LD, AM_R_R, RT_C, RT_C}},
	    {0x4A, {"LD", OP_LD, AM_R_R, RT_C, RT_D}},
	    {0x4B, {"LD", OP_LD, AM_R_R, RT_C, RT_E}},
	    {0x4C, {"LD", OP_LD, AM_R_R, RT_C, RT_H}},
	    {0x4D, {"LD", OP_LD, AM_R_R, RT_C, RT_L}},
	    {0x4E, {"LD", OP_LD, AM_R_MR, RT_C, RT_HL}},
	    {0x4F, {"LD", OP_LD, AM_R_R, RT_C, RT_A}},

	    // 0x5X - LD D,r
	    {0x50, {"LD", OP_LD, AM_R_R, RT_D, RT_B}},
	    {0x51, {"LD", OP_LD, AM_R_R, RT_D, RT_C}},
	    {0x52, {"LD", OP_LD, AM_R_R, RT_D, RT_D}},
	    {0x53, {"LD", OP_LD, AM_R_R, RT_D, RT_E}},
	    {0x54, {"LD", OP_LD, AM_R_R, RT_D, RT_H}},
	    {0x55, {"LD", OP_LD, AM_R_R, RT_D, RT_L}},
	    {0x56, {"LD", OP_LD, AM_R_MR, RT_D, RT_HL}},
	    {0x57, {"LD", OP_LD, AM_R_R, RT_D, RT_A}},
	    {0x58, {"LD", OP_LD, AM_R_R, RT_E, RT_B}},
	    {0x59, {"LD", OP_LD, AM_R_R, RT_E, RT_C}},
	    {0x5A, {"LD", OP_LD, AM_R_R, RT_E, RT_D}},
	    {0x5B, {"LD", OP_LD, AM_R_R, RT_E, RT_E}},
	    {0x5C, {"LD", OP_LD, AM_R_R, RT_E, RT_H}},
	    {0x5D, {"LD", OP_LD, AM_R_R, RT_E, RT_L}},
	    {0x5E, {"LD", OP_LD, AM_R_MR, RT_E, RT_HL}},
	    {0x5F, {"LD", OP_LD, AM_R_R, RT_E, RT_A}},

	    // 0x6X - LD H,r
	    {0x60, {"LD", OP_LD, AM_R_R, RT_H, RT_B}},
	    {0x61, {"LD", OP_LD, AM_R_R, RT_H, RT_C}},
	    {0x62, {"LD", OP_LD, AM_R_R, RT_H, RT_D}},
	    {0x63, {"LD", OP_LD, AM_R_R, RT_H, RT_E}},
	    {0x64, {"LD", OP_LD, AM_R_R, RT_H, RT_H}},
	    {0x65, {"LD", OP_LD, AM_R_R, RT_H, RT_L}},
	    {0x66, {"LD", OP_LD, AM_R_MR, RT_H, RT_HL}},
	    {0x67, {"LD", OP_LD, AM_R_R, RT_H, RT_A}},
	    {0x68, {"LD", OP_LD, AM_R_R, RT_L, RT_B}},
	    {0x69, {"LD", OP_LD, AM_R_R, RT_L, RT_C}},
	    {0x6A, {"LD", OP_LD, AM_R_R, RT_L, RT_D}},
	    {0x6B, {"LD", OP_LD, AM_R_R, RT_L, RT_E}},
	    {0x6C, {"LD", OP_LD, AM_R_R, RT_L, RT_H}},
	    {0x6D, {"LD", OP_LD, AM_R_R, RT_L, RT_L}},
	    {0x6E, {"LD", OP_LD, AM_R_MR, RT_L, RT_HL}},
	    {0x6F, {"LD", OP_LD, AM_R_R, RT_L, RT_A}},

	    // 0x7X - LD (HL),r and LD A,r
	    {0x70, {"LD", OP_LD, AM_MR_R, RT_HL, RT_B}},
	    {0x71, {"LD", OP_LD, AM_MR_R, RT_HL, RT_C}},
	    {0x72, {"LD", OP_LD, AM_MR_R, RT_HL, RT_D}},
	    {0x73, {"LD", OP_LD, AM_MR_R, RT_HL, RT_E}},
	    {0x74, {"LD", OP_LD, AM_MR_R, RT_HL, RT_H}},
	    {0x75, {"LD", OP_LD, AM_MR_R, RT_HL, RT_L}},
	    {0x76, {"HALT", OP_HALT, AM_IMP}},
	    {0x77, {"LD", OP_LD, AM_MR_R, RT_HL, RT_A}},
	    {0x78, {"LD", OP_LD, AM_R_R, RT_A, RT_B}},
	    {0x79, {"LD", OP_LD, AM_R_R, RT_A, RT_C}},
	    {0x7A, {"LD", OP_LD, AM_R_R, RT_A, RT_D}},
	    {0x7B, {"LD", OP_LD, AM_R_R, RT_A, RT_E}},
	    {0x7C, {"LD", OP_LD, AM_R_R, RT_A, RT_H}},
	    {0x7D, {"LD", OP_LD, AM_R_R, RT_A, RT_L}},
	    {0x7E, {"LD", OP_LD, AM_R_MR, RT_A, RT_HL}},
	    {0x7F, {"LD", OP_LD, AM_R_R, RT_A, RT_A}},

	    // 0x8X - ADD A,r
	    {0x80, {"ADD", OP_ADD, AM_R_R, RT_A, RT_B}},
	    {0x81, {"ADD", OP_ADD, AM_R_R, RT_A, RT_C}},
	    {0x82, {"ADD", OP_ADD, AM_R_R, RT_A, RT_D}},
	    {0x83, {"ADD", OP_ADD, AM_R_R, RT_A, RT_E}},
	    {0x84, {"ADD", OP_ADD, AM_R_R, RT_A, RT_H}},
	    {0x85, {"ADD", OP_ADD, AM_R_R, RT_A, RT_L}},
	    {0x86, {"ADD", OP_ADD, AM_R_MR, RT_A, RT_HL}},
	    {0x87, {"ADD", OP_ADD, AM_R_R, RT_A, RT_A}},
	    {0x88, {"ADC", OP_ADC, AM_R_R, RT_A, RT_B}},
	    {0x89, {"ADC", OP_ADC, AM_R_R, RT_A, RT_C}},
	    {0x8A, {"ADC", OP_ADC, AM_R_R, RT_A, RT_D}},
	    {0x8B, {"ADC", OP_ADC, AM_R_R, RT_A, RT_E}},
	    {0x8C, {"ADC", OP_ADC, AM_R_R, RT_A, RT_H}},
	    {0x8D, {"ADC", OP_ADC, AM_R_R, RT_A, RT_L}},
	    {0x8E, {"ADC", OP_ADC, AM_R_MR, RT_A, RT_HL}},
	    {0x8F, {"ADC", OP_ADC, AM_R_R, RT_A, RT_A}},

	    // 0x9X - SUB r
	    {0x90, {"SUB", OP_SUB, AM_R_R, RT_A, RT_B}},
	    {0x91, {"SUB", OP_SUB, AM_R_R, RT_A, RT_C}},
	    {0x92, {"SUB", OP_SUB, AM_R_R, RT_A, RT_D}},
	    {0x93, {"SUB", OP_SUB, AM_R_R, RT_A, RT_E}},
	    {0x94, {"SUB", OP_SUB, AM_R_R, RT_A, RT_H}},
	    {0x95, {"SUB", OP_SUB, AM_R_R, RT_A, RT_L}},
	    {0x96, {"SUB", OP_SUB, AM_R_MR, RT_A, RT_HL}},
	    {0x97, {"SUB", OP_SUB, AM_R_R, RT_A, RT_A}},
	    {0x98, {"SBC", OP_SBC, AM_R_R, RT_A, RT_B}},
	    {0x99, {"SBC", OP_SBC, AM_R_R, RT_A, RT_C}},
	    {0x9A, {"SBC", OP_SBC, AM_R_R, RT_A, RT_D}},
	    {0x9B, {"SBC", OP_SBC, AM_R_R, RT_A, RT_E}},
	    {0x9C, {"SBC", OP_SBC, AM_R_R, RT_A, RT_H}},
	    {0x9D, {"SBC", OP_SBC, AM_R_R, RT_A, RT_L}},
	    {0x9E, {"SBC", OP_SBC, AM_R_MR, RT_A, RT_HL}},
	    {0x9F, {"SBC", OP_SBC, AM_R_R, RT_A, RT_A}},

	    // 0xAX - AND r
	    {0xA0, {"AND", OP_AND, AM_R_R, RT_A, RT_B}},
	    {0xA1, {"AND", OP_AND, AM_R_R, RT_A, RT_C}},
	    {0xA2, {"AND", OP_AND, AM_R_R, RT_A, RT_D}},
	    {0xA3, {"AND", OP_AND, AM_R_R, RT_A, RT_E}},
	    {0xA4, {"AND", OP_AND, AM_R_R, RT_A, RT_H}},
	    {0xA5, {"AND", OP_AND, AM_R_R, RT_A, RT_L}},
	    {0xA6, {"AND", OP_AND, AM_R_MR, RT_A, RT_HL}},
	    {0xA7, {"AND", OP_AND, AM_R_R, RT_A, RT_A}},
	    {0xA8, {"XOR", OP_XOR, AM_R_R, RT_A, RT_B}},
	    {0xA9, {"XOR", OP_XOR, AM_R_R, RT_A, RT_C}},
	    {0xAA, {"XOR", OP_XOR, AM_R_R, RT_A, RT_D}},
	    {0xAB, {"XOR", OP_XOR, AM_R_R, RT_A, RT_E}},
	    {0xAC, {"XOR", OP_XOR, AM_R_R, RT_A, RT_H}},
	    {0xAD, {"XOR", OP_XOR, AM_R_R, RT_A, RT_L}},
	    {0xAE, {"XOR", OP_XOR, AM_R_MR, RT_A, RT_HL}},
	    {0xAF, {"XOR", OP_XOR, AM_R_R, RT_A, RT_A}},

	    // 0xBX - OR r and CP r
	    {0xB0, {"OR", OP_OR, AM_R_R, RT_A, RT_B}},
	    {0xB1, {"OR", OP_OR, AM_R_R, RT_A, RT_C}},
	    {0xB2, {"OR", OP_OR, AM_R_R, RT_A, RT_D}},
	    {0xB3, {"OR", OP_OR, AM_R_R, RT_A, RT_E}},
	    {0xB4, {"OR", OP_OR, AM_R_R, RT_A, RT_H}},
	    {0xB5, {"OR", OP_OR, AM_R_R, RT_A, RT_L}},
	    {0xB6, {"OR", OP_OR, AM_R_MR, RT_A, RT_HL}},
	    {0xB7, {"OR", OP_OR, AM_R_R, RT_A, RT_A}},
	    {0xB8, {"CP", OP_CP, AM_R_R, RT_A, RT_B}},
	    {0xB9, {"CP", OP_CP, AM_R_R, RT_A, RT_C}},
	    {0xBA, {"CP", OP_CP, AM_R_R, RT_A, RT_D}},
	    {0xBB, {"CP", OP_CP, AM_R_R, RT_A, RT_E}},
	    {0xBC, {"CP", OP_CP, AM_R_R, RT_A, RT_H}},
	    {0xBD, {"CP", OP_CP, AM_R_R, RT_A, RT_L}},
	    {0xBE, {"CP", OP_CP, AM_R_MR, RT_A, RT_HL}},
	    {0xBF, {"CP", OP_CP, AM_R_R, RT_A, RT_A}},

	    // 0xCX
	    {0xC0, {"RET", OP_RET, AM_IMP, RT_NONE, RT_NONE, CT_NZ}},
	    {0xC1, {"POP", OP_POP, AM_R, RT_BC}},
	    {0xC2, {"JP", OP_JP, AM_D16, RT_NONE, RT_NONE, CT_NZ}},
	    {0xC3, {"JP", OP_JP, AM_D16}},
	    {0xC4, {"CALL", OP_CALL, AM_D16, RT_NONE, RT_NONE, CT_NZ}},
	    {0xC5, {"PUSH", OP_PUSH, AM_R, RT_BC}},
	    {0xC6, {"ADD", OP_ADD, AM_R_D8, RT_A}},
	    {0xC7, {"RST", OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x00}},
	    {0xC8, {"RET", OP_RET, AM_IMP, RT_NONE, RT_NONE, CT_Z}},
	    {0xC9, {"RET", OP_RET, AM_IMP}},
	    {0xCA, {"JP", OP_JP, AM_D16, RT_NONE, RT_NONE, CT_Z}},
	    {0xCB, {"CB", OP_CB, AM_D8}},
	    {0xCC, {"CALL", OP_CALL, AM_D16, RT_NONE, RT_NONE, CT_Z}},
	    {0xCD, {"CALL", OP_CALL, AM_D16}},
	    {0xCE, {"ADC", OP_ADC, AM_R_D8, RT_A}},
	    {0xCF, {"RST", OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x08}},

	    // 0xDX
	    {0xD0, {"RET", OP_RET, AM_IMP, RT_NONE, RT_NONE, CT_NC}},
	    {0xD1, {"POP", OP_POP, AM_R, RT_DE}},
	    {0xD2, {"JP", OP_JP, AM_D16, RT_NONE, RT_NONE, CT_NC}},
	    {0xD4, {"CALL", OP_CALL, AM_D16, RT_NONE, RT_NONE, CT_NC}},
	    {0xD5, {"PUSH", OP_PUSH, AM_R, RT_DE}},
	    {0xD6, {"SUB", OP_SUB, AM_R_D8, RT_A}},
	    {0xD7, {"RST", OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x10}},
	    {0xD8, {"RET", OP_RET, AM_IMP, RT_NONE, RT_NONE, CT_C}},
	    {0xD9, {"RETI", OP_RETI, AM_IMP}},
	    {0xDA, {"JP", OP_JP, AM_D16, RT_NONE, RT_NONE, CT_C}},
	    {0xDC, {"CALL", OP_CALL, AM_D16, RT_NONE, RT_NONE, CT_C}},
	    {0xDE, {"SBC", OP_SBC, AM_R_D8, RT_A}},
	    {0xDF, {"RST", OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x18}},

	    // 0xEX
	    {0xE0, {"LDH", OP_LDH, AM_A8_R, RT_NONE, RT_A}},
	    {0xE1, {"POP", OP_POP, AM_R, RT_HL}},
	    {0xE2, {"LD", OP_LD, AM_MR_R, RT_C, RT_A}},
	    {0xE5, {"PUSH", OP_PUSH, AM_R, RT_HL}},
	    {0xE6, {"AND", OP_AND, AM_R_D8, RT_A}},
	    {0xE7, {"RST", OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x20}},
	    {0xE8, {"ADD", OP_ADD, AM_R_D8, RT_SP}},
	    {0xE9, {"JPHL", OP_JPHL, AM_IMP}},
	    {0xEA, {"LD", OP_LD, AM_A16_R, RT_NONE, RT_A}},
	    {0xEE, {"XOR", OP_XOR, AM_R_D8, RT_A}},
	    {0xEF, {"RST", OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x28}},

	    // 0xFX
	    {0xF0, {"LDH", OP_LDH, AM_R_A8, RT_A}},
	    {0xF1, {"POP", OP_POP, AM_R, RT_AF}},
	    {0xF2, {"LD", OP_LD, AM_R_MR, RT_A, RT_C}},
	    {0xF3, {"DI", OP_DI, AM_IMP}},
	    {0xF5, {"PUSH", OP_PUSH, AM_R, RT_AF}},
	    {0xF6, {"OR", OP_OR, AM_R_D8, RT_A}},
	    {0xF7, {"RST", OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x30}},
	    {0xF8, {"LD", OP_LD, AM_HL_SPR, RT_HL, RT_SP}},
	    {0xF9, {"LD", OP_LD, AM_R_R, RT_SP, RT_HL}},
	    {0xFA, {"LD", OP_LD, AM_R_A16, RT_A}},
	    {0xFB, {"EI", OP_EI, AM_IMP}},
	    {0xFE, {"CP", OP_CP, AM_R_D8, RT_A}},
	    {0xFF, {"RST", OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x38}}
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
