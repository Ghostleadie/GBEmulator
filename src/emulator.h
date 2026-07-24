//
// Created by Jack_ on 30/08/2025.
//

#ifndef GAMEBOYEMULATOR_EMULATOR_H
#define GAMEBOYEMULATOR_EMULATOR_H
#include <memory>
#include <string>
#include <cstdint>
#include <thread>
#include <atomic>


#include "components/emulatorClock.h"
#include "UI/debugUI.h"
#include "UI/mainMenu.h"
#include "UI/settingsUI.h"
#include "UI/screen.h"
#include "UI/inputHandler.h"

class timer;
class ppu;
class joypad;
class bus;
class cartridgeLoader;
class cpu;
class interruptController;
class apu;
struct SDL_Renderer;

enum class emulatorStates
{
    EMU_STATE_MENU = 0,
    EMU_STATE_RUNNING,
    EMU_STATE_PAUSED,
    EMU_STATE_QUIT
};

class emulator
{
public:
	emulator() = default;
	~emulator() = default;
	/** Constructs and wires every emulator component, UI panel, and clocked device. */
	void initalizeEmulator();

	/**
	 * Advances the emulator one host frame: drives the menu/running/paused state
	 * machine, starts or stops the emulation thread on transitions, samples input,
	 * and draws the debug and settings UI.
	 */
	void runEmulator();

	/** Starts the background thread that runs the CPU if it is not already running. */
	void startEmulation();

	/** Signals the background emulation thread to stop and joins it. */
	void stopEmulation();

	/**
	 * Blit the Game Boy screen to the main window.
	 * Call from the render loop with the window's SDL_Renderer, after clearing and
	 * before ImGui's draw data.
	 * @param renderer The main window's renderer to draw into.
	 */
	void renderFrame(SDL_Renderer* renderer);

	/** @return The shared CPU component. */
	const std::shared_ptr<cpu>& getCPU() const { return m_cpu; }
	/** @return The shared cartridge loader. */
	const std::shared_ptr<cartridgeLoader>& getCartridge() const { return m_cartridge; }
	/** @return The shared system bus. */
	const std::shared_ptr<bus>& getBus() const { return m_bus; }
	/** @return The shared joypad component. */
	const std::shared_ptr<joypad>& getJoypad() const { return m_joypad; }
	/** @return The shared PPU component. */
	const std::shared_ptr<ppu>& getPPU() const { return m_ppu; }
	/** @return The shared timer component. */
	const std::shared_ptr<timer>& getTimer() const { return m_timer;}
	/** @return The shared APU component. */
	const std::shared_ptr<apu>& getAPU() const { return m_apu;}
	/** @return The shared emulator clock. */
	const std::shared_ptr<emulatorClock>& getClock() const { return m_clock;}
	/** @return The shared interrupt controller. */
	const std::shared_ptr<interruptController>& getInterruptController() const { return m_interruptController; }

	//std::thread emuThread;
	//std::atomic<bool> running{false};
	/**
	 * True while the settings window has a claim on Escape (mid-rebind, or the save
	 * prompt is up), so the event loop does not quit out from under it.
	 * @return True when the settings UI should receive Escape instead of quitting.
	 */
	bool settingsWantsEscape() const { return m_settingsUI && m_settingsUI->wantsEscape(); }

public:
	bool debugUIActive = false;
	bool settingsWindowOpen = false;
	emulatorStates state = emulatorStates::EMU_STATE_MENU;
private:
	static uint64_t ticks;
	bool romLoaded = false;
	bool mainmenuActive = true;
	std::string filepath;

	std::shared_ptr<cpu> m_cpu;
	std::shared_ptr<cartridgeLoader> m_cartridge;
	std::shared_ptr<bus> m_bus;
	std::shared_ptr<joypad> m_joypad;
	std::shared_ptr<ppu> m_ppu;
	std::shared_ptr<timer> m_timer;
	std::shared_ptr<apu> m_apu;
	std::shared_ptr<interruptController> m_interruptController;
	std::shared_ptr<emulatorClock> m_clock;

	std::unique_ptr<mainMenu> m_menu;
	std::unique_ptr<debugUI> m_debugUI;
	std::unique_ptr<settingsUI> m_settingsUI;
	std::unique_ptr<screen> m_screen;
	std::unique_ptr<inputHandler> m_input;

	std::thread m_emulationThread;
	std::atomic<bool> m_running{false};
	std::atomic<bool> m_shouldStop{false};
};


#endif //GAMEBOYEMULATOR_EMULATOR_H