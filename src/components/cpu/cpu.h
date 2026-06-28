//
// Created by Jack_ on 31/08/2025.
//

#ifndef GAMEBOYEMULATOR_CPU_H
#define GAMEBOYEMULATOR_CPU_H
#include "../../interfaces/IComponentMessanger.h"
#include "../../interfaces/IInterruptSink.h"
#include <vector>
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
	cpu(const std::shared_ptr<IComponentMessanger>& bus, const std::shared_ptr<emulatorClock>& clock)
		: m_bus(bus.get()), m_clock(clock) {};

	void init();

	void emulateCycle();

	void fetchOpcode();

	void fetchData();

	// register functions
	uint16_t readRegister(registryType reg) const;

	void writeRegister(registryType reg, const uint16_t& value);

	// interrupt functions

	void handleInterrupts();

	void handleInterrupt(uint16_t address);

	bool interruptCheck(uint16_t address, interruptTypes type);

	void requestInterrupt(interruptTypes type);

	// flag functions
	bool checkConditionFlags();

	void setFlags(int8_t z, int8_t n, int8_t h, int8_t c);

	void setZeroFlag(int8_t z);

	void setSubtractFlag(int8_t n);

	void setHalfCarryFlag(int8_t h);

	void setCarryFlag(int8_t c);

	bool isZeroFlagSet() const;

	bool isSubtractFlagSet() const;

	bool isHalfCarryFlagSet() const;

	bool isCarryFlagSet() const;


	// stack functions
	void pushStack(const uint8_t value);

	uint8_t popStack();

	void pushStack16(const uint16_t value);

	uint16_t popStack16();

	//getter and setters functions

	uint16_t getFetchedData() const { return fetchedData; }
	inline void setFetchedData(const uint16_t value) { fetchedData = value; };

	bool getMasterInterruptEnabled() const { return masterInterruptEnabled; }
	inline void setMasterInterruptEnabled(const bool value) { masterInterruptEnabled = value; }

	bool getDestinationIsMemory() const { return destinationIsMemory; }
	inline void setDestinationIsMemory(const bool value) { destinationIsMemory = value; }

	uint16_t getMemoryDestination() const { return memoryDestination; }
	inline void setMemoryDestination(const uint16_t value) { memoryDestination = value; }

	IComponentMessanger* getBus() const { return m_bus; }

	std::shared_ptr<emulatorClock> getClock() const { return m_clock; }

	registers getRegistersDebug() const { return registers; }
	registers* getRegisters() { return &registers; }

	uint16_t getPC() const { return registers.pc; }
	uint16_t getSP() const { return registers.sp; }

	uint8_t getCurrentOpcode() const { return currentOpcode; }

	opcode getCurrentOpcodeData() const;

	void setSteppingMode(const bool value) { steppingMode = value; }
	bool getSteppingMode() const { return steppingMode; }

	// Gameboy Doctor instruction trace (writes to trace.txt via CPU_TRACE).
	void setTraceLogging(const bool value) { traceLogging = value; }
	bool getTraceLogging() const { return traceLogging; }

	void setStepComplete(const bool value) { stepComplete = value; }
	bool getStepComplete() const { return stepComplete; }

	void setHalted(const bool value) { halted = value; }
	bool getHalted() const { return halted; }

	void setEnablingIME(const bool value) { enablingIME = value; }
	bool getEnablingIME() const { return enablingIME; }

	std::vector<std::string> getOpcodesHistory() const { return opcodesHistory; }

	// utility functions
	static uint16_t reverse(const uint16_t number) { return ((number & 0xFF00) >> 8) | ((number & 0x00FF) << 8); }

	// test functions
	void execSingleInstructionWithOpcode(uint8_t opcode);
	void execSingleInstruction();
private:
	// Emit one Gameboy Doctor trace line for the state at the start of the
	// instruction located at `pc` (must be called before fetchOpcode()).
	void traceInstruction(uint16_t pc);

	// Non-owning: the emulator owns the bus and outlives the cpu (the
	// emulation thread is joined before teardown).
	IComponentMessanger* m_bus = nullptr;
	std::shared_ptr<emulatorClock> m_clock;
	std::vector<std::string> opcodesHistory;
	bool steppingMode = false;
	bool stepComplete = false;
	bool halted = false;
	bool traceLogging = false;
	registers registers = {};
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