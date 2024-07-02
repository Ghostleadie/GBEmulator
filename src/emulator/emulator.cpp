#include "emulator.h"
#include "cartridge.h"
#include "memory.h"
#include "cpu.h"
#include "bus.h"
#include "instructions.h"

std::shared_ptr <emulator> m_emulator;
std::shared_ptr <cartridgeLoader> m_loader;
std::shared_ptr <cpu> m_cpu;

void emulation::initEmulator()
{
    m_loader = std::shared_ptr <cartridgeLoader>(new cartridgeLoader());
    m_memory = std::shared_ptr <memory>(new memory());
    m_instructions = std::shared_ptr <instructions>(new instructions());
    m_bus = std::shared_ptr <bus>(new bus(m_loader, m_memory));
    m_cpu = std::shared_ptr <cpu>(new cpu(m_bus,m_instructions,m_loader));
    //work around will find a better solution later
    m_bus->connectCPU(m_cpu);
  
}

int emulation::runEmulator(int argc, char** argv)
{
    if (argc < 2) {
        GBE_INFO("Error no ROMS available");
        return -1;
    }
    
    if (!m_loader->loadCartridge(argv[1])) {
        GBE_WARN("Failed to load ROM file: %s\n", argv[1]);
        return -2;
    }

    SDL_Init(SDL_INIT_VIDEO);
    printf("SDL INIT\n");

    m_cpu->init();

    m_emulator.running = true;
    m_emulator.paused = false;
    m_emulator.ticks = 0;

    while (m_emulator.running) {
        if (m_emulator.paused) {
            delay(10);
            continue;
        }

        if (!m_cpu->step()) {
            printf("CPU Stopped\n");
            return -3;
        }

        m_emulator.ticks++;
    }

	return 0;
}

void emulation::delay(uint32_t ms)
{	
	SDL_Delay(ms);	
}

void emulation::cycles(int cpuCycles)
{

}

emulator::emulator()
{
    bool paused = false;
    bool running = true;
    uint64_t ticks = 0;
}
