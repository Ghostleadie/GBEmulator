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
				unsigned char f;
				unsigned char a;
			};

			unsigned short af;
		};
	};

	struct
	{
		union
		{
			struct
			{
				unsigned char c;
				unsigned char b;
			};

			unsigned short bc;
		};
	};

	struct
	{
		union
		{
			struct
			{
				unsigned char e;
				unsigned char d;
			};

			unsigned short de;
		};
	};

	struct
	{
		union
		{
			struct
			{
				unsigned char l;
				unsigned char h;
			};

			unsigned short hl;
		};
	};

	unsigned short sp;
	unsigned short pc;
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
	cpu(std::shared_ptr<bus> bus)
		: m_bus(bus) {};

	void init();

	void emulateCycle();

	void fetchOpcode();

	void fetchData();

	//void execute(opcode opcode);

	//void onInstructionExecuted(const std::string& instruction);

	static uint16_t reverse(const uint16_t number) { return ((number & 0xFF00) >> 8) | ((number & 0x00FF) << 8); }

	uint16_t readRegister(registryType reg) const;

	void writeRegister(registryType reg, const uint16_t& value);

	bool checkConditionFlags() const;

	void setFlags(uint8_t z, uint8_t n, uint8_t h, uint8_t c) const;

	void setZeroFlag(uint8_t z) const;

	void setSubtractFlag(uint8_t n) const;

	void setCarryFlag(uint8_t c) const;

	bool isZeroFlagSet() const;

	bool isSubtractFlagSet() const;

	bool isHalfCarryFlagSet() const;

	bool isCarryFlagSet() const;

	void setHalfCarryFlag(uint8_t h) const;

	void isFlagSet();

	//getter and setters

	uint8_t getIERegister() const;

	void setIERegister(uint8_t value);

	inline uint16_t getFetchedData() const { return fetchedData; }
	inline void setFetchedData(const uint16_t value) { fetchedData = value; };

	inline bool getMasterInterruptEnable() const { return masterInterruptEnable; }
	inline void setMasterInterruptEnable(const bool value) { masterInterruptEnable = value; }

	inline bool getDestinationIsMemory() const { return destinationIsMemory; }
	inline void setDestinationIsMemory(const bool value) { destinationIsMemory = value; }

	inline uint16_t getMemoryDestination() const { return memoryDestination; }
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

	void setIMEScheduled(const bool value) { imeScheduled = value; }
	bool getIMEScheduled() const { return imeScheduled; }

private:
	std::shared_ptr<bus> m_bus;
	bool steppingMode = false;
	bool stepComplete = false;
	bool halted = false;
	registers registers = {};
	uint8_t currentOpcode = 0;
	opcode currentOpcodeData;
	uint16_t fetchedData;
	uint16_t memoryDestination;
	bool destinationIsMemory;
	bool masterInterruptEnable;
	uint8_t interruptEnableRegister;
	bool imeScheduled;

public:

private:
};

#endif //GAMEBOYEMULATOR_CPU_H