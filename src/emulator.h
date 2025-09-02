//
// Created by Jack_ on 30/08/2025.
//

#ifndef GAMEBOYEMULATOR_EMULATOR_H
#define GAMEBOYEMULATOR_EMULATOR_H
#include "UI/mainMenu.h"

class timer;
class ppu;
class joypad;
class bus;
class cartridgeLoader;
class cpu;
class apu;

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
	void initalizeEmulator();
	void runEmulator();

private:
	bool romLoaded = false;
	bool mainmenuActive = true;
	std::string filepath;
	emulatorStates state = EMU_STATE_MENU;
	std::shared_ptr<cpu> m_cpu;
	std::shared_ptr<cartridgeLoader> m_cartridge;
	std::shared_ptr<bus> m_bus;
	std::shared_ptr<apu> m_apu;
	std::shared_ptr<joypad> m_joypad;
	std::shared_ptr<ppu> m_ppu;
	std::shared_ptr<timer> m_timer;
	mainMenu menu;
};


#endif //GAMEBOYEMULATOR_EMULATOR_H