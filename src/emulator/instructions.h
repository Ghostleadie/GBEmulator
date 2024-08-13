#pragma once

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

enum instructionType 
{
    IN_NONE,
    IN_NOP,
    IN_LD,
    IN_INC,
    IN_DEC,
    IN_RLCA,
    IN_ADD,
    IN_RRCA,
    IN_STOP,
    IN_RLA,
    IN_JR,
    IN_RRA,
    IN_DAA,
    IN_CPL,
    IN_SCF,
    IN_CCF,
    IN_HALT,
    IN_ADC,
    IN_SUB,
    IN_SBC,
    IN_AND,
    IN_XOR,
    IN_OR,
    IN_CP,
    IN_POP,
    IN_JP,
    IN_PUSH,
    IN_RET,
    IN_CB,
    IN_CALL,
    IN_RETI,
    IN_LDH,
    IN_JPHL,
    IN_DI,
    IN_EI,
    IN_RST,
    IN_ERR,
    IN_RLC,
    IN_RRC,
    IN_RL,
    IN_RR,
    IN_SLA,
    IN_SRA,
    IN_SWAP,
    IN_SRL,
    IN_BIT,
    IN_RES,
    IN_SET
};

enum conditionType
{
    CT_NONE,
    CT_NZ,
    CT_Z,
    CT_NC,
    CT_C
};

struct instruction 
{
    std::string name;
    instructionType type;
    addressMode mode;
    registryType reg1;
    registryType reg2;
    conditionType cond;
    uint8_t param;

    instruction();
    instruction(std::string m_name, instructionType m_type, addressMode m_mode = addressMode::AM_IMP, registryType m_reg1 = registryType::RT_NONE, registryType m_reg2 = registryType::RT_NONE, conditionType m_cond = conditionType::CT_NONE, uint8_t m_param = 0);
};

class instructions
{
public:
    instructions() {}
    instruction instructionByOpcode(uint8_t opcode);

    char* instructionName(instructionType t);

    static registryType registryLookup(uint8_t reg);
private:
    
};