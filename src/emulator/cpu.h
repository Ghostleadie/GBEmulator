#pragma once

#include "instructions.h"

class bus;
class cartridgeLoader;

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
	bool masterInteruptEnabled = true;

	cpuContext() {}
};

class cpu
{
public:
	cpu() {};
	cpu(std::shared_ptr <bus> bus, std::shared_ptr <instructions> instructions, std::shared_ptr <cartridgeLoader> loader);
	void init();
	bool step();
	void fetchInstruction();
	void fetchData();
	void execute();
	uint8_t getIERegister();
	void setIERegister(uint8_t value);
protected:
	
	uint16_t readRegistry(registryType rt);
	void setRegistry(registryType rt, uint16_t value);
	uint16_t reverse(uint16_t n);
	static bool checkCondition(std::weak_ptr<cpuContext> ctx);
	void setFlags(std::weak_ptr<cpuContext> ctx, char z, char n, char h, char c);
private:
	std::shared_ptr <cpuContext> ctx;
	std::shared_ptr <bus> m_bus;
	std::shared_ptr <instructions> m_instructions;
	std::shared_ptr <cartridgeLoader> m_loader;
};