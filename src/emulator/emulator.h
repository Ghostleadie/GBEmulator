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
};

class emulation
{
public:
	int runEmulator(int argc, char** argv);

	emulator* getEmulator() { return &m_emulator;  };

	void delay(uint32_t ms);
private:
	emulator m_emulator;
	cartridgeLoader m_loader;
	cpu m_cpu;
	const char* cp = "roms/dmg-acid.gb";

};