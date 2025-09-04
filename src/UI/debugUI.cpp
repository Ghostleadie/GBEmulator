//
// Created by Jack_ on 30/08/2025.
//

#include "debugUI.h"
#include "../components/cartridgeLoader.h"
#include "imgui.h"
#include "../emulator.h"

void debugUI::UpdateUIPanels()
{
	cartridgeDebugUI.updateUI(cLoader.lock()->peekCartridgeContext());
}