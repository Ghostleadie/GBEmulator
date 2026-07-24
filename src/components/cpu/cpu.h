//
// Created by Jack_ on 31/08/2025.
//

#ifndef GAMEBOYEMULATOR_CPU_H
#define GAMEBOYEMULATOR_CPU_H
#include "../../interfaces/IComponentMessanger.h"
#include "../../interfaces/IInterruptSink.h"
#include <vector>
#include <array>
#include <atomic>
#include <cstdint>
#include <cstddef>
#include "opcodes.h"
#include "../../Utility/SerialPortDebugger.h"
#include "../emulatorClock.h"


class bus;

struct registers
{

	union
	{
		struct
		{
			uint8_t f;
			uint8_t a;
		};

		uint16_t af;
	};

	union
	{
		struct
		{
			uint8_t c;
			uint8_t b;
		};

		uint16_t bc;
	};

	union
	{
		struct
		{
			uint8_t e;
			uint8_t d;
		};

		uint16_t de;
	};

	union
	{
		struct
		{
			uint8_t l;
			uint8_t h;
		};

		uint16_t hl;
	};

	uint16_t sp;
	uint16_t pc;
};

struct flags
{
	bool zero;
	bool subtraction;
	bool halfCarry;
	bool carry;
};

class cpu
{
public:
	/**
	 * Constructs the CPU, wiring it to the message bus and the emulator clock.
	 * @param bus shared component-messenger bus used for all memory access; stored as a non-owning raw pointer.
	 * @param clock shared emulator clock advanced as M-cycles are consumed.
	 */
	cpu(const std::shared_ptr<IComponentMessanger>& bus, const std::shared_ptr<emulatorClock>& clock)
		: m_bus(bus.get()), m_clock(clock) {};

	/**
	 * Initialises CPU state to the DMG post-boot-ROM values (PC=0x100, AF=0x01B0, BC=0x0013, DE=0x00D8, HL=0x014D, SP=0xFFFE).
	 * Also clears the IME master interrupt flag and the delayed IME-enable latch.
	 */
	void init();

	/**
	 * Advances the CPU by one instruction: runs the fetch/decode/execute cycle, or idles one M-cycle while halted.
	 * When halted, HALT is exited only once an enabled interrupt is pending (IE & IF & 0x1F).
	 * After executing, services pending interrupts when IME is set and applies the one-instruction-delayed IME enable from EI.
	 * Honours stepping mode and optionally emits a Gameboy Doctor trace line before each fetch.
	 */
	void emulateCycle();

	/**
	 * Reads the opcode byte at PC, advances PC, and decodes it into the current opcode data.
	 * The fetch itself costs one M-cycle (a memory read).
	 */
	void fetchOpcode();

	/**
	 * Fetches the current instruction's operand(s) according to its addressing mode.
	 * Populates fetchedData and, for memory-destination modes, memoryDestination/destinationIsMemory, consuming one M-cycle per memory access.
	 * Handles HL post-increment/decrement and the 0xFF00 high-memory offset for the C register and 8-bit-address modes.
	 */
	void fetchData();

	// register functions
	/**
	 * Reads an 8- or 16-bit CPU register by type.
	 * @param reg the register to read (A, F, B, C, D, E, H, L, or the AF/BC/DE/HL/PC/SP pairs).
	 * @return the register's current value, or 0 for an unknown register.
	 */
	uint16_t readRegister(registryType reg) const;

	/**
	 * Writes an 8- or 16-bit CPU register by type; 8-bit writes are masked to the low byte.
	 * @param reg the register to write (A, F, B, C, D, E, H, L, or the AF/BC/DE/HL/PC/SP pairs).
	 * @param value the value to store.
	 */
	void writeRegister(registryType reg, const uint16_t& value);

	// interrupt functions

	/**
	 * Services the highest-priority pending-and-enabled interrupt, dispatching to its handler vector.
	 * Checks sources in priority order: VBlank (0x40), LCD STAT (0x48), Timer (0x50), Serial (0x58), Joypad (0x60).
	 */
	void handleInterrupts();

	/**
	 * Enters an interrupt service routine: pushes the current PC onto the stack and jumps to the handler address.
	 * @param address the interrupt vector to jump to.
	 */
	void handleInterrupt(uint16_t address);

	/**
	 * Dispatches the given interrupt if its bit is both requested (IF, 0xFF0F) and enabled (IE, 0xFFFF).
	 * On dispatch it calls the handler, clears the source's IF bit, exits HALT, and disables IME.
	 * @param address the interrupt vector to jump to when serviced.
	 * @param type the interrupt source bit to test.
	 * @return true if the interrupt was serviced, false otherwise.
	 */
	bool interruptCheck(uint16_t address, interruptTypes type);

	/**
	 * Requests an interrupt by setting its bit in the interrupt flag register (IF, 0xFF0F).
	 * @param type the interrupt source to flag.
	 */
	void requestInterrupt(interruptTypes type);

	// flag functions
	/**
	 * Evaluates the current instruction's condition code against the Zero and Carry flags.
	 * @return true if the condition (none/C/NC/Z/NZ) is satisfied and the conditional operation should proceed.
	 */
	bool checkConditionFlags();

	/**
	 * Sets the four F-register flags at once; a value of -1 leaves the corresponding flag unchanged.
	 * @param z Zero flag (bit 7): 0 clears, 1 sets, -1 preserves.
	 * @param n Subtract flag (bit 6): 0 clears, 1 sets, -1 preserves.
	 * @param h Half-Carry flag (bit 5): 0 clears, 1 sets, -1 preserves.
	 * @param c Carry flag (bit 4): 0 clears, 1 sets, -1 preserves.
	 */
	void setFlags(int8_t z, int8_t n, int8_t h, int8_t c);

	/**
	 * Sets or clears the Zero flag (F register bit 7).
	 * @param z 0 clears, 1 sets, -1 leaves the flag unchanged.
	 */
	void setZeroFlag(int8_t z);

	/**
	 * Sets or clears the Subtract flag (F register bit 6).
	 * @param n 0 clears, 1 sets, -1 leaves the flag unchanged.
	 */
	void setSubtractFlag(int8_t n);

	/**
	 * Sets or clears the Half-Carry flag (F register bit 5).
	 * @param h 0 clears, 1 sets, -1 leaves the flag unchanged.
	 */
	void setHalfCarryFlag(int8_t h);

	/**
	 * Sets or clears the Carry flag (F register bit 4).
	 * @param c 0 clears, 1 sets, -1 leaves the flag unchanged.
	 */
	void setCarryFlag(int8_t c);

	/** @return true if the Zero flag (F register bit 7) is set. */
	bool isZeroFlagSet() const;

	/** @return true if the Subtract flag (F register bit 6) is set. */
	bool isSubtractFlagSet() const;

	/** @return true if the Half-Carry flag (F register bit 5) is set. */
	bool isHalfCarryFlagSet() const;

	/** @return true if the Carry flag (F register bit 4) is set. */
	bool isCarryFlagSet() const;


	// stack functions
	/**
	 * Pushes one byte onto the stack: pre-decrements SP, then writes the value at SP.
	 * @param value the byte to push.
	 */
	void pushStack(const uint8_t value);

	/**
	 * Pops one byte off the stack: reads the value at SP, then post-increments SP.
	 * @return the byte popped from the stack.
	 */
	uint8_t popStack();

	/**
	 * Pushes a 16-bit value onto the stack, high byte first so the low byte ends up at the lower address (little-endian).
	 * @param value the 16-bit value to push.
	 */
	void pushStack16(const uint16_t value);

	/**
	 * Pops a 16-bit value off the stack, low byte first (little-endian).
	 * @return the 16-bit value popped from the stack.
	 */
	uint16_t popStack16();

	//getter and setters functions

	/** @return the operand data fetched for the current instruction. */
	uint16_t getFetchedData() const { return fetchedData; }
	/** Sets the operand data for the current instruction. */
	inline void setFetchedData(const uint16_t value) { fetchedData = value; };

	/** @return true if the IME (interrupt master enable) flag is set. */
	bool getMasterInterruptEnabled() const { return masterInterruptEnabled; }
	/** Sets the IME (interrupt master enable) flag. */
	inline void setMasterInterruptEnabled(const bool value) { masterInterruptEnabled = value; }

	/** @return true if the current instruction writes its result to memory rather than a register. */
	bool getDestinationIsMemory() const { return destinationIsMemory; }
	/** Sets whether the current instruction's destination is memory. */
	inline void setDestinationIsMemory(const bool value) { destinationIsMemory = value; }

	/** @return the memory address the current instruction will write to. */
	uint16_t getMemoryDestination() const { return memoryDestination; }
	/** Sets the memory address the current instruction will write to. */
	inline void setMemoryDestination(const uint16_t value) { memoryDestination = value; }

	/** @return the non-owning message bus used for memory access. */
	IComponentMessanger* getBus() const { return m_bus; }

	/** @return the shared emulator clock. */
	std::shared_ptr<emulatorClock> getClock() const { return m_clock; }

	/** @return a copy of the CPU register file, for debugging/inspection. */
	registers getRegistersDebug() const { return m_registers; }
	/** @return a mutable pointer to the CPU register file. */
	registers* getRegisters() { return &m_registers; }

	/** @return the program counter. */
	uint16_t getPC() const { return m_registers.pc; }
	/** @return the stack pointer. */
	uint16_t getSP() const { return m_registers.sp; }

	/** @return the raw opcode byte currently being executed. */
	uint8_t getCurrentOpcode() const { return currentOpcode; }

	/** @return the decoded metadata for the current opcode. */
	opcode getCurrentOpcodeData() const;

	/** Enables or disables single-step (stepping) mode. */
	void setSteppingMode(const bool value) { steppingMode = value; }
	/** @return true if single-step (stepping) mode is enabled. */
	bool getSteppingMode() const { return steppingMode; }

	/** Enables or disables Gameboy Doctor instruction tracing (each instruction is written to trace.txt via CPU_TRACE). */
	void setTraceLogging(const bool value) { traceLogging = value; }
	/** @return true if Gameboy Doctor instruction tracing is enabled. */
	bool getTraceLogging() const { return traceLogging; }

	/** Sets the flag marking the current single step as complete. */
	void setStepComplete(const bool value) { stepComplete = value; }
	/** @return true if the current single step has completed. */
	bool getStepComplete() const { return stepComplete; }

	/** Sets the halted (HALT) state. */
	void setHalted(const bool value) { halted = value; }
	/** @return true if the CPU is halted (HALT). */
	bool getHalted() const { return halted; }

	/** Sets the latch that enables IME after the next instruction (deferred EI behaviour). */
	void setEnablingIME(const bool value) { enablingIME = value; }
	/** @return true if IME is pending enable after the next instruction (deferred EI). */
	bool getEnablingIME() const { return enablingIME; }

	/**
	 * Snapshots the recorded opcode history, oldest entry first. Built on demand and bounded by
	 * OpcodeHistoryCapacity, so the emulation thread never pays for it; safe to call from the UI thread.
	 * @return the mnemonics of the most recently executed instructions (empty while recording is off).
	 */
	std::vector<std::string> getOpcodesHistory() const;

	/** Enables/disables opcode-history recording. Off by default so normal play pays nothing. */
	void setRecordOpcodeHistory(const bool enabled) { m_recordOpcodeHistory.store(enabled, std::memory_order_relaxed); }
	/** @return true if opcode-history recording is currently enabled. */
	bool getRecordOpcodeHistory() const { return m_recordOpcodeHistory.load(std::memory_order_relaxed); }

	// utility functions
	/**
	 * Swaps the high and low bytes of a 16-bit value (endianness reversal).
	 * @param number the value whose bytes to swap.
	 * @return the byte-swapped value.
	 */
	static uint16_t reverse(const uint16_t number) { return ((number & 0xFF00) >> 8) | ((number & 0x00FF) << 8); }

	// test functions
	/**
	 * Executes a single instruction from a supplied opcode, bypassing the PC fetch (unit-test helper).
	 * @param opcode the opcode byte to decode and execute.
	 */
	void execSingleInstructionWithOpcode(uint8_t opcode);
	/** Fetches, decodes, and executes the single instruction at PC (unit-test helper). */
	void execSingleInstruction();
private:
	/**
	 * Emits one Gameboy Doctor trace line capturing CPU state at the start of the instruction at `pc`.
	 * Must be called before fetchOpcode(); logs A/F/B/C/D/E/H/L, SP, PC and the four PCMEM bytes via CPU_TRACE, with F as the raw flags byte.
	 * @param pc the pre-fetch program counter of the instruction being traced.
	 */
	void traceInstruction(uint16_t pc);

	/**
	 * Appends one executed opcode byte to the history ring buffer when recording is enabled.
	 * Called on the emulation thread only (single producer); a no-op when recording is off.
	 * @param opcodeByte the primary opcode byte that was executed.
	 */
	void recordOpcode(uint8_t opcodeByte);

	// Non-owning: the emulator owns the bus and outlives the cpu (the
	// emulation thread is joined before teardown).
	IComponentMessanger* m_bus = nullptr;
	std::shared_ptr<emulatorClock> m_clock;
	// Fixed-capacity ring buffer of recently executed opcode bytes. Written only by the emulation
	// thread (single producer) via recordOpcode; snapshotted by the UI thread via getOpcodesHistory.
	// Storing the raw byte keeps the hot path to one relaxed atomic store; mnemonics are resolved
	// lazily from the static opcode table when the debug panel asks for them.
	static constexpr std::size_t OpcodeHistoryCapacity = 512;
	std::array<std::atomic<uint8_t>, OpcodeHistoryCapacity> m_opcodeHistory{};
	std::atomic<uint64_t> m_opcodeHistoryCount{0};
	std::atomic<bool> m_recordOpcodeHistory{false};
	bool steppingMode = false;
	bool stepComplete = false;
	bool halted = false;
	bool traceLogging = false;
	registers m_registers = {};
	uint8_t currentOpcode = 0;
	opcode currentOpcodeData;
	uint16_t fetchedData;
	uint16_t memoryDestination;
	bool destinationIsMemory;
	bool masterInterruptEnabled;
	bool enablingIME;
	SerialPortDebugger m_serialDebugger;
};

#endif //GAMEBOYEMULATOR_CPU_H