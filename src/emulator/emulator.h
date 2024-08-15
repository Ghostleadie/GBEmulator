#pragma once
#include <stdio.h>
#include "SDL.h"
#include "entt/entt.hpp"

class cartridgeLoader;
class memory;
class cpu;
class bus;
class instructions;
class ui;
class interrupts;
class io;

struct emulator
{
	bool paused;
	bool running;
	bool die = false;
	uint64_t ticks;

	emulator();
};

class emulation
{
public:
	emulation() {};
	void initEmulator();
	void* runCPU();
	int runEmulator(int argc, char** argv);
	

	//inline std::shared_ptr<emulator> getEmulator()& { return m_emulator;  };
	inline std::shared_ptr<cartridgeLoader> getCartridgeLoader()& { return m_loader; }
	inline std::shared_ptr<cpu> getCPU()& { return m_cpu; }

	void delay(uint32_t ms);

	static void cycles(int cpuCycles);
	
	entt::registry& get_registry();

private:
	entt::registry m_registry;
	//std::map<std::string, std::shared_ptr<c>>

	std::shared_ptr<emulator> m_emulator;
	std::shared_ptr<cartridgeLoader> m_loader;
	std::shared_ptr<cpu> m_cpu;
	std::shared_ptr<bus> m_bus;
	std::shared_ptr<instructions> m_instructions;
	std::shared_ptr<memory> m_memory;
	std::shared_ptr<ui> m_ui;
	std::shared_ptr<interrupts> m_interrupts;
	std::shared_ptr<io> m_io;
};