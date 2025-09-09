//
// Created by Jack_ on 30/08/2025.
//

#include "debugUI.h"
#include "../components/cartridgeLoader.h"

void debugUI::UpdateUIPanels()
{
	if (auto loader = m_Loader.lock()) {
		cartridgeDebugUI.updateUI(loader->peekCartridgeContext());
	}
}