#pragma once
#include "instructions.h"
#include "bus.h"

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
	instruction currentInstruction;

	bool halted = false;
	bool stepping = false;
	bool destinationIsMemory = false;

	cpuContext() {}
};

class cpu
{
public:
	cpu() {};
	cpu(std::shared_ptr <bus> bus, std::shared_ptr <instructions> instructions, std::shared_ptr <cartridgeLoader> loader);
	void init();
	bool step();
	
protected:
	void fetchInstruction();
	void fetchData();
	void execute();
	uint16_t cpuReadRegistry(registryType rt);
	uint16_t reverse(uint16_t n);
private:
	cpuContext ctx;
	std::shared_ptr <bus> m_bus;
	std::shared_ptr <instructions> m_instructions;
	std::shared_ptr <cartridgeLoader> m_loader;
};