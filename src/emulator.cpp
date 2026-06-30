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
#include "components/interruptController.h"
#include "SDL3/SDL_timer.h"

uint64_t emulator::ticks = 0;

void emulator::initalizeEmulator()
{
	m_bus = std::make_shared<bus>();
	m_timer = std::make_shared<timer>();
	m_interruptController = std::make_shared<interruptController>();
	m_ppu = std::make_shared<ppu>(m_interruptController);
	m_clock = std::make_shared<emulatorClock>();
    m_cpu = std::make_shared<cpu>(m_bus, m_clock);

    m_cartridge = std::make_shared<cartridgeLoader>();
	m_joypad = std::make_shared<joypad>();


	m_bus->connectComponents(m_cartridge, m_joypad, m_ppu, m_timer, m_cpu, m_interruptController);

	m_debugUI = std::make_unique<debugUI>(m_cartridge, m_cpu, m_bus);
	m_menu = std::make_unique<mainMenu>();

	m_menu->init();
	m_cpu->init();
	m_timer->init(m_interruptController);
	m_ppu->init();
	m_clock->addDevice(m_timer.get());
	m_clock->addDevice(m_ppu.get());
}

void emulator::runEmulator()
{
	// Track state transitions so we start/stop the emulation thread once
	static emulatorStates lastState = EMU_STATE_MENU;
	if (state != lastState)
	{
		if (state == EMU_STATE_RUNNING)
		{
			// start the background emulation thread if not already running
			startEmulation();
		}
		else if (lastState == EMU_STATE_RUNNING)
		{
			// leaving RUNNING -> stop the emulation thread
			stopEmulation();
		}
		lastState = state;
	}

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
				state = EMU_STATE_RUNNING; // will trigger startEmulation on next frame
			}
			break;
		}
		case EMU_STATE_RUNNING:
		{
			if (romLoaded == false)
			{
				romLoaded = m_cartridge->loadCartridge(filepath);
			}
			// Do not run CPU cycles here — background thread (startEmulation) handles it.
			break;
		}
		case EMU_STATE_PAUSED:
		{
			SDL_Delay(10);
			break;
		}
		default:
			break;
	}

	if (debugUIActive)
	{
		m_debugUI->UpdateUIPanels();
	}
}

void emulator::startEmulation()
{
	if (m_running)
		return; // Already running

	m_running = true;
	m_shouldStop = false;

	m_emulationThread = std::thread([this]() {
		static const uint32_t CYCLES_PER_FRAME = 69905;

		while (!m_shouldStop)
		{
			if (state == EMU_STATE_RUNNING && romLoaded)
			{
				uint32_t cyclesThisFrame = 0;
				while (cyclesThisFrame < CYCLES_PER_FRAME && !m_shouldStop)
				{
					uint32_t cyclesBefore = m_clock->getTicks();
					m_cpu->emulateCycle();
					cyclesThisFrame += (m_clock->getTicks() - cyclesBefore);
				}

				// Frame limiting
				//SDL_Delay(16); // ~60 FPS
			}
			else
			{
				SDL_Delay(1); // Yield CPU when not running
			}
		}
	});
}

void emulator::stopEmulation()
{
	m_shouldStop = true;

	if (m_emulationThread.joinable())
	{
		m_emulationThread.join();
	}

	m_running = false;
}
