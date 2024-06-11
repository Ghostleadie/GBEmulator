#include "emulator.h"

int emulation::runEmulator(int argc, char** argv)
{
    if (argc < 2) {
        GBE_INFO("Error no ROMS available");
        return -1;
    }
    
    if (!m_loader.loadCartridge(argv[1])) {
        GBE_WARN("Failed to load ROM file: %s\n", argv[1]);
        return -2;
    }

    SDL_Init(SDL_INIT_VIDEO);
    printf("SDL INIT\n");

    m_cpu.init();

    m_emulator.running = true;
    m_emulator.paused = false;
    m_emulator.ticks = 0;

    while (m_emulator.running) {
        if (m_emulator.paused) {
            delay(10);
            continue;
        }

        if (!m_cpu.step()) {
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

