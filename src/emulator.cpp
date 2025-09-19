//
// Created by Jack_ on 30/08/2025.
//

#include "emulator.h"

#include "components/bus.h"
#include "components/cartridgeLoader.h"
#include "components/joypad.h"
#include "components/ppu.h"
#include "components/timer.h"
#include "components/cpu/cpu.h"
#include "SDL3/SDL_timer.h"

uint64_t emulator::ticks = 0;

void emulator::initalizeEmulator()
{
	m_bus = std::make_shared<bus>();
    m_cpu = std::make_shared<cpu>(m_bus);

    m_cartridge = std::make_shared<cartridgeLoader>();
	m_joypad = std::make_shared<joypad>();
	m_ppu = std::make_shared<ppu>();
	m_timer = std::make_shared<timer>();

	m_bus->connectComponents(m_cartridge, m_joypad, m_ppu, m_timer, m_cpu);

	m_debugUI = std::make_unique<debugUI>(m_cartridge,m_cpu);
	m_menu = std::make_unique<mainMenu>();

	m_menu->init();
	m_cpu->init();
}

void emulator::runEmulator()
{
	switch (state)
	{
		case EMU_STATE_MENU:
		{
			if (mainmenuActive == true)
			{
				mainmenuActive = m_menu->openMainMenu(filepath);
			}
			if (!filepath.empty())
			{
				state = EMU_STATE_RUNNING;
			}
			break;
		}
		case EMU_STATE_RUNNING:
		{
			if (romLoaded == false)
			{
				romLoaded = m_cartridge->loadCartridge(filepath);
			}
			for (int i = 0; i < 10000; i++)
			{
				m_cpu->emulateCycle();
			}
			break;
		}
		case EMU_STATE_PAUSED:
		{
			SDL_Delay(10);
			break;
		}
		default:
		{
			break;
		}
	}

	ticks++;

	if (debugUIActive)
	{
		m_debugUI->UpdateUIPanels();
	}
}

void emulator::cycles(const uint64_t cycles)
{
	for (int i=0; i<cycles; i++) {
		for (int n=0; n<4; n++) {
			ticks++;
			//m_timer->tick();
			//m_ppu->tick();
		}

		//tick();
	}
}
