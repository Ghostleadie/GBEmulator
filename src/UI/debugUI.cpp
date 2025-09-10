//
// Created by Jack_ on 30/08/2025.
//

#include "debugUI.h"
#include "../components/cartridgeLoader.h"

void debugUI::UpdateUIPanels()
{

	cartridgeDebugUI.updateUI(m_Loader.lock()->peekCartridgeContext());
	cpuDebugUI.updateUI(*m_cpu.lock());
	/*if (auto loader = m_Loader.lock()) {
		cartridgeDebugUI.updateUI(loader->peekCartridgeContext());
	}

	if (auto loader = m_cpu.lock()) {
		cpuDebugUI.updateUI(*loader);
	}*/
}