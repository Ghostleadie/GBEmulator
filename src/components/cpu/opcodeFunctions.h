//
// Created by Jack_ on 08/09/2025.
//

#ifndef GAMEBOYEMULATOR_OPCODEFUNCTIONS_H
#define GAMEBOYEMULATOR_OPCODEFUNCTIONS_H
#include <memory>
#include "opcodes.h"


class cpu;

class OpcodeCommand {
public:
	virtual ~OpcodeCommand() = default;
	/** Executes this opcode command, mutating the given CPU's registers, memory, and flags.
	 * @param m_cpu CPU the instruction operates on. */
	virtual void execute(cpu& m_cpu) = 0;
};

class AdcCommand : public OpcodeCommand {
public:
    /** Executes ADC A,n: adds the fetched operand plus the carry flag to A. Sets Z if zero, clears N, sets H/C on carry. */
    void execute(cpu& m_cpu) override;
};

class Add8BitCommand : public OpcodeCommand {
public:
	/** Executes 8-bit ADD: adds the fetched operand to A. Sets Z if zero, clears N, sets H/C on carry. */
	void execute(cpu& m_cpu) override;
};

class Add16BitCommand : public OpcodeCommand {
public:
	/** Executes 16-bit ADD (ADD HL,rr): adds the fetched 16-bit operand to HL. Leaves Z, clears N, sets H/C on 12/16-bit carry. */
	void execute(cpu& m_cpu) override;
};

class AddSPCommand : public OpcodeCommand {
public:
	/** Executes ADD SP,e8: adds a signed 8-bit offset to SP. Clears Z and N, sets H/C from the low-byte addition. */
	void execute(cpu& m_cpu) override;
};

class AndCommand : public OpcodeCommand {
public:
    /** Executes AND A,n: ANDs the fetched operand into A. Sets Z if zero, clears N and C, sets H. */
    void execute(cpu& m_cpu) override;
};

class CallCommand : public OpcodeCommand {
public:
    /** Executes CALL: if the condition holds, pushes PC then jumps to the fetched address. Affects no flags. */
    void execute(cpu& m_cpu) override;
};

class CcfCommand : public OpcodeCommand {
public:
    /** Executes CCF: complements the carry flag. Leaves Z, clears N and H. */
    void execute(cpu& m_cpu) override;
};

class CbCommand : public OpcodeCommand
{
public:
	/** Executes a CB-prefixed instruction: decodes and performs the bit/rotate/shift op (BIT, RES, SET, RLC, RRC, RL, RR, SLA, SRA, SWAP, SRL) on a register or (HL), setting Z/N/H/C per that op. */
	void execute(cpu& m_cpu) override;
};

class CplCommand : public OpcodeCommand {
public:
    /** Executes CPL: bitwise-complements A (A = ~A). Leaves Z and C, sets N and H. */
    void execute(cpu& m_cpu) override;
};

class CpCommand : public OpcodeCommand {
public:
    /** Executes CP A,n: compares A with the fetched operand (A - n) without storing the result. Sets Z if equal, sets N, sets H/C on borrow. */
    void execute(cpu& m_cpu) override;
};

class DaaCommand : public OpcodeCommand {
public:
    /** Executes DAA: decimal-adjusts A into valid BCD after an add/subtract. Sets Z if zero, leaves N, clears H, sets C on carry. */
    void execute(cpu& m_cpu) override;
};

class Dec8BitCommand : public OpcodeCommand {
public:
    /** Executes 8-bit DEC: decrements reg1 or the byte at (HL). Sets Z if zero, sets N, sets H on nibble borrow, leaves C. */
    void execute(cpu& m_cpu) override;
};

class Dec16BitCommand : public OpcodeCommand {
public:
	/** Executes 16-bit DEC: decrements the 16-bit register reg1. Affects no flags. */
	void execute(cpu& m_cpu) override;
};

class DiCommand : public OpcodeCommand {
public:
    /** Executes DI: disables interrupts by clearing the master interrupt-enable (IME) flag. */
    void execute(cpu& m_cpu) override;
};

class EiCommand : public OpcodeCommand {
public:
    /** Executes EI: schedules enabling of interrupts; IME becomes set after the following instruction. */
    void execute(cpu& m_cpu) override;
};

class HaltCommand : public OpcodeCommand {
public:
    /** Executes HALT: halts the CPU until an interrupt becomes pending. */
    void execute(cpu& m_cpu) override;
};

class Inc8BitCommand : public OpcodeCommand {
public:
    /** Executes 8-bit INC: increments reg1 or the byte at (HL). Sets Z if zero, clears N, sets H on nibble carry, leaves C. */
    void execute(cpu& m_cpu) override;
};

class Inc16BitCommand : public OpcodeCommand {
public:
	/** Executes 16-bit INC: increments the 16-bit register reg1 with no flag effects; for the (HL) memory case increments the byte and sets Z, clears N, sets H. */
	void execute(cpu& m_cpu) override;
};

class JpCommand : public OpcodeCommand {
public:
    /** Executes JP: if the condition holds, jumps to the fetched 16-bit address. Affects no flags. */
    void execute(cpu& m_cpu) override;
};

class JphlCommand : public OpcodeCommand {
public:
    /** Executes JP (HL): jumps to the address held in HL. Affects no flags. */
    void execute(cpu& m_cpu) override;
};

class JrCommand : public OpcodeCommand {
public:
    /** Executes JR: if the condition holds, performs a relative jump by the signed 8-bit offset. Affects no flags. */
    void execute(cpu& m_cpu) override;
};

class Ld8BitCommand : public OpcodeCommand {
public:
    /** Executes 8-bit LD: stores the fetched byte into reg1 or the destination memory address. Affects no flags. */
    void execute(cpu& m_cpu) override;
};

class Ld16BitCommand : public OpcodeCommand {
public:
	/** Executes 16-bit LD: stores the fetched 16-bit value into reg1 or the destination memory address. Affects no flags. */
	void execute(cpu& m_cpu) override;
};

class LdSpecialCommand : public OpcodeCommand {
public:
	/** Executes LD HL,SP+e8: loads SP plus a signed 8-bit offset into HL. Clears Z and N, sets H/C from the low-byte addition. */
	void execute(cpu& m_cpu) override;
};

class LdhCommand : public OpcodeCommand {
public:
    /** Executes LDH: loads A from, or stores A to, high memory at 0xFF00 + offset. Affects no flags. */
    void execute(cpu& m_cpu) override;
};


class NopCommand : public OpcodeCommand {
public:
    /** Executes NOP: no operation. Affects no flags. */
    void execute(cpu& m_cpu) override;
};


class OrCommand : public OpcodeCommand {
public:
    /** Executes OR A,n: ORs the fetched operand into A. Sets Z if zero, clears N, H, and C. */
    void execute(cpu& m_cpu) override;
};


class PopCommand : public OpcodeCommand {
public:
    /** Executes POP: pops a 16-bit value off the stack into reg1. Affects no flags. */
    void execute(cpu& m_cpu) override;
};

class PopSpecialCommand : public OpcodeCommand {
public:
	/** Executes POP AF: pops a 16-bit value into AF, masking off the unused low nibble of F; the flags are loaded from the popped low byte. */
	void execute(cpu& m_cpu) override;
};

class PushCommand : public OpcodeCommand {
public:
    /** Executes PUSH: pushes the 16-bit register reg1 onto the stack. Affects no flags. */
    void execute(cpu& m_cpu) override;
};


class RetCommand : public OpcodeCommand {
public:
    /** Executes RET: if the condition holds, pops the return address off the stack into PC. Affects no flags. */
    void execute(cpu& m_cpu) override;
};


class RetiCommand : public OpcodeCommand {
public:
    /** Executes RETI: re-enables interrupts (IME) and returns by popping PC off the stack. Affects no flags. */
    void execute(cpu& m_cpu) override;
};


class RlaCommand : public OpcodeCommand {
public:
    /** Executes RLA: rotates A left through the carry flag. Clears Z, N, and H; sets C to the old bit 7. */
    void execute(cpu& m_cpu) override;
};


class RlcaCommand : public OpcodeCommand {
public:
    /** Executes RLCA: rotates A left circularly (old bit 7 into carry and bit 0). Clears Z, N, and H; sets C. */
    void execute(cpu& m_cpu) override;
};


class RraCommand : public OpcodeCommand {
public:
    /** Executes RRA: rotates A right through the carry flag. Clears Z, N, and H; sets C to the old bit 0. */
    void execute(cpu& m_cpu) override;
};


class RrcaCommand : public OpcodeCommand {
public:
    /** Executes RRCA: rotates A right circularly (old bit 0 into carry and bit 7). Clears Z, N, and H; sets C. */
    void execute(cpu& m_cpu) override;
};


class RstCommand : public OpcodeCommand {
public:
    /** Executes RST: pushes PC and jumps to the fixed restart vector given by the opcode's param. Affects no flags. */
    void execute(cpu& m_cpu) override;
};


class SbcCommand : public OpcodeCommand {
public:
    /** Executes SBC A,n: subtracts the fetched operand and the carry flag from A. Sets Z if zero, sets N, sets H/C on borrow. */
    void execute(cpu& m_cpu) override;
};


class ScfCommand : public OpcodeCommand {
public:
    /** Executes SCF: sets the carry flag. Leaves Z, clears N and H. */
    void execute(cpu& m_cpu) override;
};


class StopCommand : public OpcodeCommand {
public:
    /** Executes STOP: consumes the following byte and halts the CPU. */
    void execute(cpu& m_cpu) override;
};


class SubCommand : public OpcodeCommand {
public:
    /** Executes SUB A,n: subtracts the fetched operand from A. Sets Z if zero, sets N, sets H/C on borrow. */
    void execute(cpu& m_cpu) override;
};


class XorCommand : public OpcodeCommand {
public:
    /** Executes XOR A,n: XORs the fetched operand into A. Sets Z if zero, clears N, H, and C. */
    void execute(cpu& m_cpu) override;
};

class OpcodeCommandFactory {
public:
	/** Creates the OpcodeCommand handler for a decoded opcode, selecting the correct variant (8/16-bit, SP, special, or memory form); integrates with the existing opcodeFactory.
	 * @param opcode Decoded opcode to dispatch on.
	 * @return Owning pointer to the matching command, or nullptr if the opcode is unimplemented. */
	static std::unique_ptr<OpcodeCommand> createCommand(const opcode& opcode);
};


#endif //GAMEBOYEMULATOR_OPCODEFUNCTIONS_H