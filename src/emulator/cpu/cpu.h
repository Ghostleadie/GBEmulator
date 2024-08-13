#pragma once

#include "../instructions.h"

class bus;
class cartridgeLoader;
class interrupts;

struct cpuRegisters
{
	uint8_t a;
	uint8_t f;
	uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t e;
	uint8_t h;
	uint8_t l;
	uint16_t programCounter;
	uint16_t stackCounter;

	cpuRegisters() {}
};

struct cpuContext
{
	cpuRegisters registers;
	//current fetched data
	uint16_t fetchedData;
	uint16_t memoryDestination;
	uint8_t currentOpcode;
	uint8_t interruptEnableRegister;
	instruction currentInstruction;

	bool halted = false;
	bool stepping = false;
	bool destinationIsMemory = false;
	bool masterInterruptEnabled = true;
	bool enableIME = false;

	uint8_t interruptFlags;

	cpuContext() {}
};

class cpu
{
public:
	cpu() {};
	cpu(std::shared_ptr <bus> bus, std::shared_ptr <instructions> instructions, std::shared_ptr <cartridgeLoader> loader, std::shared_ptr <interrupts> interrupts);
	void init();
	bool step();
	void goToAddress(std::weak_ptr<cpuContext> ctx, uint16_t address, bool pushPC);
	void fetchInstruction();
	void fetchData();
	void execute();
	uint8_t getIERegister();
	void setIERegister(uint8_t value);
	uint8_t getInterruptFlags();
	void setInterruptFlags(uint8_t value);
	void pushStack(uint8_t value);
	void pushStack(uint16_t value);
	void pushStack(uint16_t high, uint16_t low);
	uint16_t popStack(int bits = 8);
private:
	uint16_t readRegistry(registryType rt);
	void setRegistry(registryType rt, uint16_t value);
	uint8_t cbReadReg(registryType rt);
	void cbsetReg(registryType rt, uint8_t value);
	uint16_t reverse(uint16_t n);
	static bool checkCondition(std::weak_ptr<cpuContext> ctx);
	void setFlags(std::weak_ptr<cpuContext> ctx, uint8_t z, uint8_t n, uint8_t h, uint8_t c);
	void setZeroFlag(std::weak_ptr<cpuContext> ctx, uint8_t z);
	void setSubtractFlag(std::weak_ptr<cpuContext> ctx, uint8_t n);
	void setHalfCarryFlag(std::weak_ptr<cpuContext> ctx, uint8_t h);
	void setCarryFlag(std::weak_ptr<cpuContext> ctx, uint8_t c);
	void isFlagSet();

	//instructions
	void instructionADC();
	void instructionADD();
	void instructionAND();
	void instructionCALL();
	void instructionCCF();
	// instruction for executing additional instructions
	// prefix cb 
	void instructionCB();
	void instructionCPL();
	void instructionCP();
	void instructionDAA();
	void instructionDEC(std::weak_ptr<bus> bus);
	void instructionDI();
	void instructionEI();
	void instructionHALT();
	void instructionINC(std::weak_ptr<bus> bus);
	void instructionJP();
	void instructionJPHL();
	void instructionJR();
	void instructionLD(std::weak_ptr<bus> bus);
	void instructionLDH(std::weak_ptr<bus> bus);
	void instructionNOP();
	void instructionOR();
	void instructionPOP();
	void instructionPUSH();
	void instructionRET();
	void instructionRETI();
	void instructionRLA();
	void instructionRLCA();
	void instructionRRA();
	void instructionRRCA();
	void instructionRST();
	void instructionSBC();
	void instructionSCF();
	void instructionSTOP();
	void instructionSUB();
	void instructionXOR();

private:
	std::shared_ptr <interrupts> m_interrupts;
	std::shared_ptr <cpuContext> ctx;
	std::shared_ptr <bus> m_bus;
	std::shared_ptr <instructions> m_instructions;
	std::shared_ptr <cartridgeLoader> m_loader;
};