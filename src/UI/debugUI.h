//
// Created by Jack_ on 30/08/2025.
//

#ifndef GAMEBOYEMULATOR_DEBUGUI_H
#define GAMEBOYEMULATOR_DEBUGUI_H
#include "debugpanels/cartridgeDebugUI.h"
#include "debugpanels/cpuDebugUI.h"

class timer;
class ppu;
class joypad;
class apu;
class bus;
class cpu;
class cartridgeLoader;
struct CartridgeContext;

class debugUI
{
public:
	debugUI() = default;
	debugUI(const std::shared_ptr<cartridgeLoader>& cartridge_loader, const std::shared_ptr<cpu>& cpu) : m_cpu(cpu), m_Loader(cartridge_loader){};
	~debugUI() = default;
	void UpdateUIPanels();

private:
	std::weak_ptr<cpu> m_cpu;
	std::weak_ptr<cartridgeLoader> m_Loader;
	std::weak_ptr<bus> m_bus;
	std::weak_ptr<apu> m_apu;
	std::weak_ptr<joypad> m_joypad;
	std::weak_ptr<ppu> m_ppu;
	std::weak_ptr<timer> m_timer;
	cartridgeDebugUI cartridgeDebugUI;
	cpuDebugUI cpuDebugUI;

};


#endif //GAMEBOYEMULATOR_DEBUGUI_H