//
// Created by Jack_ on 30/08/2025.
//

#ifndef GAMEBOYEMULATOR_DEBUGUI_H
#define GAMEBOYEMULATOR_DEBUGUI_H
#include "../components/cartridgeLoader.h"
#include "debugpanels/cartridgeDebugUI.h"


struct cartridgeContext;

class debugUI
{
public:
	debugUI() = default;
	debugUI(const std::shared_ptr<cartridgeLoader>& cartridge_loader) : cLoader(cartridge_loader) {};
	~debugUI() = default;
	void UpdateUIPanels();

private:
	std::weak_ptr<cartridgeLoader> cLoader;
	cartridgeDebugUI cartridgeDebugUI;
};


#endif //GAMEBOYEMULATOR_DEBUGUI_H