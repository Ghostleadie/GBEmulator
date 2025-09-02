//
// Created by Jack_ on 30/08/2025.
//

#include "emulator.h"

#include "Components/apu.h"
#include "Components/bus.h"
#include "Components/cartridgeLoader.h"
#include "Components/joypad.h"
#include "Components/ppu.h"
#include "Components/timer.h"
#include "Components/cpu/cpu.h"

void emulator::initalizeEmulator()
{
    m_cpu = std::make_shared<cpu>();
    m_bus = std::make_shared<bus>();
    m_cartridge = std::make_shared<cartridgeLoader>();
	m_apu = std::make_shared<apu>();
	m_joypad = std::make_shared<joypad>();
	m_ppu = std::make_shared<ppu>();
	m_timer = std::make_shared<timer>();

	m_bus->addComponent(m_cpu);
	m_bus->addComponent(m_cartridge);
	m_bus->addComponent(m_apu);
	m_bus->addComponent(m_joypad);
	m_bus->addComponent(m_ppu);
	m_bus->addComponent(m_timer);

	menu.init();
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
				mainmenuActive = menu.openMainMenu(filepath);
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

			//m_cpu->emulateCycle();
			break;
		}
	}
	//m_cartridge->loadCartridge(filepath);
}
