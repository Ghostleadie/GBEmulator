#include "bus.h"
#include "cartridge.h"
#include "cpu/cpu.h"
#include "cpu/interrupts.h"
#include "display/ui.h"
#include "emulator.h"
#include "instructions.h"
#include "memory.h"
#include <future>

using namespace std::chrono_literals;

void emulation::initEmulator()
{
    m_emulator = std::make_shared<emulator>();
    m_loader = std::make_shared<cartridgeLoader>();
    m_memory = std::make_shared <memory>();
    m_instructions = std::make_shared<instructions>();
    m_bus = std::make_shared<bus>(m_loader, m_memory);
    m_interrupts = std::make_shared<interrupts>();
    m_cpu = std::make_shared<cpu>(m_bus, m_instructions, m_loader, m_interrupts);
    //work around will find a better solution later
    m_bus->connectCPU(m_cpu);
    m_interrupts->ConnectCPU(m_cpu);
    
  
}

void* emulation::runCPU()
{
    m_cpu->init();
    m_emulator->running = true;
    m_emulator->paused = false;
    m_emulator->ticks = 0;

    while (m_emulator->running) {
        if (m_emulator->paused) {
            delay(10);
            continue;
        }

        if (!m_cpu->step()) {
            printf("CPU Stopped\n");
            return 0;
        }

        m_emulator->ticks++;
    }
    return 0;
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

    m_ui->init();
    std::thread cpuThread(&emulation::runCPU, this);
    
    while (!m_emulator->die)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(1000));
        m_ui->handleEvents(m_emulator);
    }
    
    
    SDL_Quit();
    cpuThread.join();
    
    //exit(-7);
    //SDL_Quit();

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
