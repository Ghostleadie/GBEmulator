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

class timer;
class ppu;
class joypad;
class bus;
class cartridgeLoader;
class cpu;

enum emulatorStates
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
	void initalizeEmulator();
	void runEmulator();

	void startEmulation();
	void stopEmulation();

	const std::shared_ptr<cpu>& getCPU() const { return m_cpu; }
	const std::shared_ptr<cartridgeLoader>& getCartridge() const { return m_cartridge; }
	const std::shared_ptr<bus>& getBus() const { return m_bus; }
	const std::shared_ptr<joypad>& getJoypad() const { return m_joypad; }
	const std::shared_ptr<ppu>& getPPU() const { return m_ppu; }
	const std::shared_ptr<timer>& getTimer() const { return m_timer;}
	const std::shared_ptr<emulatorClock>& getClock() const { return m_clock;}

	//std::thread emuThread;
	//std::atomic<bool> running{false};
public:
	bool debugUIActive = false;
	emulatorStates state = EMU_STATE_MENU;
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
	std::shared_ptr<emulatorClock> m_clock;

	std::unique_ptr<mainMenu> m_menu;
	std::unique_ptr<debugUI> m_debugUI;

	std::thread m_emulationThread;
	std::atomic<bool> m_running{false};
	std::atomic<bool> m_shouldStop{false};
};


#endif //GAMEBOYEMULATOR_EMULATOR_H