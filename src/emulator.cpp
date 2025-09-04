//
// Created by Jack_ on 30/08/2025.
//

#include "emulator.h"

#include "components/apu.h"
#include "components/bus.h"
#include "components/cartridgeLoader.h"
#include "components/joypad.h"
#include "components/ppu.h"
#include "components/timer.h"
#include "components/cpu/cpu.h"

void emulator::initalizeEmulator()
{
	m_bus = std::make_shared<bus>();
    m_cpu = std::make_shared<cpu>(m_bus);

    m_cartridge = std::make_shared<cartridgeLoader>();
	m_apu = std::make_shared<apu>();
	m_joypad = std::make_shared<joypad>();
	m_ppu = std::make_shared<ppu>();
	m_timer = std::make_shared<timer>();

	m_bus->connectComponents(m_cartridge, m_apu, m_joypad, m_ppu, m_timer);

	m_menu.init();
	m_cpu->init();
	m_debugUI = debugUI(m_cartridge);
}

void emulator::runEmulator()
{
	switch (state)
	{
		case EMU_STATE_MENU:
		{
			if (mainmenuActive == true)
			{
				mainmenuActive = m_menu.openMainMenu(filepath);
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

			m_cpu->emulateCycle();
			break;
		}
	}

	if (debugUIActive)
	{
		m_debugUI.UpdateUIPanels();
	}
	//m_cartridge->loadCartridge(filepath);
}
