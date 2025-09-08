//
// Created by Jack_ on 01/09/2025.
//

#ifndef GAMEBOYEMULATOR_OPCODES_H
#define GAMEBOYEMULATOR_OPCODES_H

enum addressMode
{
	AM_IMP,
	AM_R_D16,
	AM_R_R,
	AM_MR_R,
	AM_R,
	AM_R_D8,
	AM_R_MR,
	AM_R_HLI,
	AM_R_HLD,
	AM_HLI_R,
	AM_HLD_R,
	AM_R_A8,
	AM_A8_R,
	AM_HL_SPR,
	AM_D16,
	AM_D8,
	AM_D16_R,
	AM_MR_D8,
	AM_MR,
	AM_A16_R,
	AM_R_A16
};

enum registryType
{
	RT_NONE,
	RT_A,
	RT_F,
	RT_B,
	RT_C,
	RT_D,
	RT_E,
	RT_H,
	RT_L,
	RT_AF,
	RT_BC,
	RT_DE,
	RT_HL,
	RT_SP,
	RT_PC
};

enum opcodeType
{
	OP_NONE,
	OP_NOP,
	OP_LD,
	OP_INC,
	OP_DEC,
	OP_RLCA,
	OP_ADD,
	OP_RRCA,
	OP_STOP,
	OP_RLA,
	OP_JR,
	OP_RRA,
	OP_DAA,
	OP_CPL,
	OP_SCF,
	OP_CCF,
	OP_HALT,
	OP_ADC,
	OP_SUB,
	OP_SBC,
	OP_AND,
	OP_XOR,
	OP_OR,
	OP_CP,
	OP_POP,
	OP_JP,
	OP_PUSH,
	OP_RET,
	OP_CB,
	OP_CALL,
	OP_RETI,
	OP_LDH,
	OP_JPHL,
	OP_DI,
	OP_EI,
	OP_RST,
	OP_ERR,
	OP_RLC,
	OP_RRC,
	OP_RL,
	OP_RR,
	OP_SLA,
	OP_SRA,
	OP_SWAP,
	OP_SRL,
	OP_BIT,
	OP_RES,
	OP_SET
};

enum conditionType
{
	CT_NONE,
	CT_NZ,
	CT_Z,
	CT_NC,
	CT_C
};

struct opcode
{
	std::string name;
	opcodeType type;
	addressMode mode;
	registryType reg1;
	registryType reg2;
	conditionType cond;
	uint8_t param;

	opcode();

	opcode(const std::string& m_name, opcodeType m_type, addressMode m_mode = AM_IMP, registryType m_reg1 = RT_NONE, registryType m_reg2 = RT_NONE, conditionType m_cond = CT_NONE, uint8_t m_param = 0);
};

/*class opcode {
public:
	virtual ~opcode() = default;
	virtual void execute() const = 0;

	opcode() : name(""), type(OP_NONE), mode(AM_IMP), reg1(RT_NONE), reg2(RT_NONE), cond(CT_NONE), param(0) {}
public:
	std::string name;
	opcodeType type;
	addressMode mode;
	registryType reg1;
	registryType reg2;
	conditionType cond;
	uint8_t param;
};

class nopOpcode : public opcode
{
public:
	nopOpcode() = default;
	nopOpcode(const std::string& m_name, opcodeType m_type, addressMode m_mode = AM_IMP, registryType m_reg1 = RT_NONE, registryType m_reg2 = RT_NONE, conditionType m_cond = CT_NONE, uint8_t m_param = 0)
	{
		name = m_name;
		type = m_type;
		mode = m_mode;
		reg1 = m_reg1;
		reg2 = m_reg2;
		cond = m_cond;
		param = m_param;
	}
	void execute() const override {
		// NOP does nothing
	}
};

// Factory interface for creating opcode instances

class opcodeFactory
{
public:
	virtual ~opcodeFactory() = default;
	virtual std::shared_ptr<opcode> createOpcode(const std::string& m_name, opcodeType m_type, addressMode m_mode = AM_IMP, registryType m_reg1 = RT_NONE, registryType m_reg2 = RT_NONE, conditionType m_cond = CT_NONE, uint8_t m_param = 0) const = 0;
};*/


#endif //GAMEBOYEMULATOR_OPCODES_H