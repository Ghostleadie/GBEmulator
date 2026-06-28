//
// Created by Jack_ on 30/08/2025.
//

#ifndef GAMEBOYEMULATOR_DEBUGUI_H
#define GAMEBOYEMULATOR_DEBUGUI_H
#include "debugpanels/busDebugUI.h"
#include "debugpanels/cartridgeDebugUI.h"
#include "debugpanels/cpuDebugUI.h"
#include "debugpanels/tileDebugUI.h"

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
	debugUI(const std::shared_ptr<cartridgeLoader>& cartridge_loader, const std::shared_ptr<cpu>& cpu, const std::shared_ptr<bus>& bus) : m_cpu(cpu), m_Loader(cartridge_loader), m_bus(bus){};
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
	busDebugUI busDebugUI;
	tileDebugUI tileDebugUI;
};


#endif //GAMEBOYEMULATOR_DEBUGUI_H