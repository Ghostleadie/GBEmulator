//
// Created by Jack_ on 01/09/2025.
//

#ifndef GAMEBOYEMULATOR_OPCODES_H
#define GAMEBOYEMULATOR_OPCODES_H
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

class cpu;

enum class addressMode
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
	AM_I16,
	AM_I8,
	AM_I16_R,
	AM_MR_D8,
	AM_MR,
	AM_A16_R,
	AM_R_A16
};

enum class registryType
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

enum class opcodeType
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

enum class conditionType
{
	CT_NONE,
	CT_NZ,
	CT_Z,
	CT_NC,
	CT_C
};

/** Decoded metadata for a 0xCB-prefixed instruction: mnemonic, operation type, and target register. */
struct extendedCBOpcode
{
	std::string name;
	opcodeType type;
	registryType reg;
};

/** Decoded metadata for a single unprefixed SM83 instruction, including a closure that executes it. */
struct opcode
{
	//meta data
	std::string name;
	opcodeType type;
	addressMode mode;
	registryType reg1;
	registryType reg2;
	conditionType cond;
	uint8_t param;
	// Function pointer for execution - set by factory
	std::function<void(cpu&)> execute;

	opcode() = default;

	/**
	 * Builds an opcode from its decode metadata; execute is left null until bound by initializeOpcodeExecution().
	 * @param name Instruction mnemonic (e.g. "LD").
	 * @param type Operation category.
	 * @param mode Addressing mode describing the operands.
	 * @param reg1 First register operand, or RT_NONE.
	 * @param reg2 Second register operand, or RT_NONE.
	 * @param cond Branch condition, or CT_NONE.
	 * @param par Embedded parameter such as an RST target address.
	 */
	opcode(const std::string& name, const opcodeType type = opcodeType::OP_NOP, const addressMode mode = addressMode::AM_IMP, const registryType reg1 = registryType::RT_NONE, const registryType reg2 = registryType::RT_NONE, const conditionType cond = conditionType::CT_NONE, const uint8_t par = 0)
		: name(name), type(type), mode(mode), reg1(reg1), reg2(reg2), cond(cond), param(par) {};
};

/** Primary lookup table mapping each unprefixed opcode byte to its decoded metadata. */
extern const std::unordered_map<uint8_t, opcode> opcodeTable;
/** Lookup table mapping each 0xCB-prefixed opcode byte to its decoded metadata. */
extern const std::unordered_map<uint8_t, extendedCBOpcode> cbOpcodeTable;

/**
 * Binds the execute closure of every entry in opcodeTable, wiring each opcode to its command implementation.
 * Idempotent: the table is populated only on the first call; later calls are no-ops.
 */
void initializeOpcodeExecution();

/**
 * Looks up the decoded metadata for an unprefixed opcode byte, initializing the execute closures on first use.
 * @param opcode_value The opcode byte to decode.
 * @return The matching opcode entry, or the NOP entry (0x00) as a fallback when the byte is unmapped.
 */
opcode getOpcode(uint8_t opcode_value);

/**
 * Looks up the decoded metadata for a 0xCB-prefixed opcode byte.
 * @param opcode_value The second byte of a CB-prefixed instruction.
 * @return The matching extendedCBOpcode entry, or a default-constructed entry when the byte is unmapped.
 */
extendedCBOpcode getCBOpcode(uint8_t opcode_value);

/**
 * Returns the mnemonic for an unprefixed opcode byte without copying the table entry.
 * Cheap enough to call per-row when rendering a debug view; reads only the static table.
 * @param opcode_value The opcode byte to name.
 * @return Reference to the mnemonic held in the static opcode table, or the NOP mnemonic when unmapped.
 */
const std::string& getOpcodeName(uint8_t opcode_value);


#endif //GAMEBOYEMULATOR_OPCODES_H