//
// Created by Jack_ on 31/08/2025.
//

#ifndef GAMEBOYEMULATOR_CPU_H
#define GAMEBOYEMULATOR_CPU_H
#include "../base/component.h"
#include "opcodes.h"

class bus;

struct registers
{
	struct
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
	};

	struct
	{
		union
		{
			struct
			{
				uint8_t c;
				uint8_t b;
			};

			uint16_t bc;
		};
	};

	struct
	{
		union
		{
			struct
			{
				uint8_t e;
				uint8_t d;
			};

			uint16_t de;
		};
	};

	struct
	{
		union
		{
			struct
			{
				uint8_t l;
				uint8_t h;
			};

			uint16_t hl;
		};
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

enum interruptTypes
{
	INT_VBLANK = 1,
	INT_LCD_STAT = 2,
	INT_TIMER = 4,
	INT_SERIAL = 8,
	INT_JOYPAD = 16
};

class cpu
{
public:
	cpu(const std::shared_ptr<bus>& bus)
		: m_bus(bus) {};

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


	// flag functions
	bool checkConditionFlags();

	void setFlags(uint8_t z, uint8_t n, uint8_t h, uint8_t c);

	void setZeroFlag(uint8_t z);

	void setSubtractFlag(uint8_t n);

	void setHalfCarryFlag(uint8_t h);

	void setCarryFlag(uint8_t c);

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

	uint8_t getIERegister() const;

	void setIERegister(uint8_t value);

	uint16_t getFetchedData() const { return fetchedData; }
	inline void setFetchedData(const uint16_t value) { fetchedData = value; };

	bool getMasterInterruptEnabled() const { return masterInterruptEnabled; }
	inline void setMasterInterruptEnabled(const bool value) { masterInterruptEnabled = value; }

	bool getDestinationIsMemory() const { return destinationIsMemory; }
	inline void setDestinationIsMemory(const bool value) { destinationIsMemory = value; }

	uint16_t getMemoryDestination() const { return memoryDestination; }
	inline void setMemoryDestination(const uint16_t value) { memoryDestination = value; }

	std::weak_ptr<bus> getBus() const { return m_bus; }

	registers getRegistersDebug() const { return registers; }
	registers& getRegisters() { return registers; }

	uint16_t getPC() const { return registers.pc; }
	uint16_t getSP() const { return registers.sp; }

	uint8_t getCurrentOpcode() const { return currentOpcode; }
	opcode getCurrentOpcodeData() const;

	void setSteppingMode(const bool value) { steppingMode = value; }
	bool getSteppingMode() const { return steppingMode; }

	void setStepComplete(const bool value) { stepComplete = value; }
	bool getStepComplete() const { return stepComplete; }

	void setHalted(const bool value) { halted = value; }
	bool getHalted() const { return halted; }

	void setEnablingIME(const bool value) { enablingIME = value; }
	bool getEnablingIME() const { return enablingIME; }

	void setInterruptFlags(const uint8_t value) { interruptFlags = value; }
	uint8_t getInterruptFlags() const { return interruptFlags; }

	// utility functions
	static uint16_t reverse(const uint16_t number) { return ((number & 0xFF00) >> 8) | ((number & 0x00FF) << 8); }
private:
	std::shared_ptr<bus> m_bus;
	std::vector<opcode> opcodesHistory;
	bool steppingMode = false;
	bool stepComplete = false;
	bool halted = false;
	registers registers = {};
	uint8_t currentOpcode = 0;
	opcode currentOpcodeData;
	uint16_t fetchedData;
	uint16_t memoryDestination;
	bool destinationIsMemory;
	bool masterInterruptEnabled;
	uint8_t interruptEnableRegister;
	bool enablingIME;
	uint8_t interruptFlags;

public:

private:
};

#endif //GAMEBOYEMULATOR_CPU_H