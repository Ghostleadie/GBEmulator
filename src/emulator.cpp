//
// Created by Jack_ on 30/08/2025.
//

#include "emulator.h"

#include "components/bus.h"
#include "components/cartridgeLoader.h"
#include "components/joypad.h"
#include "components/ppu.h"
#include "components/timer.h"
#include "components/apu.h"
#include "components/cpu/cpu.h"
#include "components/interruptController.h"
#include "Utility/settings.h"
#include "SDL3/SDL_timer.h"

// Enumerators used unqualified throughout this file; enum class keeps the type
// safety while using enum spares every call site the emulatorStates:: prefix.
using enum emulatorStates;

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
	m_apu = std::make_shared<apu>();


	m_bus->connectComponents(m_cartridge, m_joypad, m_ppu, m_timer, m_cpu, m_interruptController);
	m_bus->connectAudio(m_apu.get());

	m_debugUI = std::make_unique<debugUI>(m_cartridge, m_cpu, m_bus);
	m_settingsUI = std::make_unique<settingsUI>();
	m_screen = std::make_unique<screen>();
	m_menu = std::make_unique<mainMenu>();
	m_input = std::make_unique<inputHandler>();

	m_menu->init();
	m_cpu->init();
	m_timer->init(m_interruptController);
	m_ppu->init();
	m_joypad->init(m_interruptController);
	m_apu->init();
	m_input->init();
	m_clock->addDevice(m_timer.get());
	m_clock->addDevice(m_ppu.get());
	m_clock->addDevice(m_joypad.get());
	m_clock->addDevice(m_apu.get());

	// Seed the APU with the volume saved on disk, so audio comes up at the
	// user's level before the settings window is ever opened. The default
	// matches the settings UI's own fallback.
	const nlohmann::json root = settings::load();
	int volume = 75;
	if (const auto it = root.find("volume"); it != root.end() && it->is_number())
	{
		volume = it->get<int>();
	}
	m_apu->setMasterVolume(volume);
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
			// Sample physical input on the main thread and hand it to the joypad,
			// which carries it to the emulation thread. While the settings window is
			// open it owns the keyboard (rebinding), so release everything and skip
			// the poll rather than leaking those keystrokes into the game.
			if (settingsWindowOpen)
			{
				m_joypad->setButtons(0);
			}
			else
			{
				m_input->poll(*m_joypad);
			}
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

	// Bar first and unconditionally -- it stays on screen whether or not the
	// panels are showing, and carries the toggle that opens them.
	m_debugUI->drawMenuBar(&debugUIActive, &settingsWindowOpen);

	if (debugUIActive)
	{
		m_debugUI->UpdateUIPanels();
	}

	if (settingsWindowOpen)
	{
		m_settingsUI->updateUI(&settingsWindowOpen);
	}

	// Pick up rebinds the moment they are saved, so the change is live without a
	// restart. Cheap when nothing was saved (just a flag check).
	if (m_settingsUI->consumeInputChanged())
	{
		m_input->reloadBindings();
	}

	// Same for the master volume: push it into the APU as soon as it is saved.
	if (m_settingsUI->consumeVolumeChanged())
	{
		m_apu->setMasterVolume(m_settingsUI->savedVolume());
	}
}

void emulator::renderFrame(SDL_Renderer* renderer)
{
	// Only while a ROM is live -- nothing to show in the menu (background stays
	// cleared and the main menu draws over it).
	if (state == EMU_STATE_RUNNING || state == EMU_STATE_PAUSED)
	{
		m_screen->present(renderer, *m_ppu);
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

				// Flush the battery-backed .sav periodically (throttled inside).
				// Runs on this thread -- the same one that writes cart RAM -- so
				// it needs no locking.
				m_cartridge->saveBatteryIfDirty();

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

	// Emulation thread is stopped: safe to write the final save from here.
	if (m_cartridge)
	{
		m_cartridge->saveBattery();
	}
}
