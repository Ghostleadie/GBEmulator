#pragma once
#include <stdio.h>
#include "cartridge.h"
#include "cpu.h"
#include "SDL.h"

struct emulator
{
	bool paused;
	bool running;
	uint64_t ticks;

	emulator();
};

class emulation
{
public:
	emulation() {};
	void initEmulator();
	int runEmulator(int argc, char** argv);

	//inline std::shared_ptr<emulator> getEmulator()& { return m_emulator;  };
	inline std::shared_ptr<cartridgeLoader> getCartridgeLoader()& { return m_loader; }
	inline std::shared_ptr<cpu> getCPU()& { return m_cpu; }

	void delay(uint32_t ms);

	static void cycles(int cpuCycles);
private:
	emulator m_emulator;
	std::shared_ptr<cartridgeLoader> m_loader;
	std::shared_ptr<cpu> m_cpu;
	std::shared_ptr<bus> m_bus;
	std::shared_ptr<instructions> m_instructions;
	std::shared_ptr<memory> m_memory;

};