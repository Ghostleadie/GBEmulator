//
// Created by Jack_ on 05/09/2025.
//

#include "busDebugUI.h"
#include "imgui.h"
#include <cstring>


void busDebugUI::updateUI(bus& m_bus, bool* open)
{
	if (!ImGui::Begin("Bus Debug", open))
	{
		ImGui::End();
		return;
	}
	ImGui::Text("Serial Output");
	ImGui::Separator();

	const char* serial = m_bus.getSerialOutput();
	const std::size_t len = std::strlen(serial);
	const bool grew = (len != lastSerialLen);
	lastSerialLen = len;

	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -55), true, ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::TextUnformatted(serial);
	if (grew)
	{
		ImGui::SetScrollHereY(1.0f); // auto-scroll while new output arrives
	}
	ImGui::EndChild();

	if (ImGui::Button("Clear"))
	{
		m_bus.clearSerialOutput();
		lastSerialLen = 0;
	}

	// Toggle the bus-side serial logging ("Serial:" lines + Failed trap).
	static bool serialLoggingEnabled = false;
	ImGui::Checkbox("Enable Bus Serial Logging", &serialLoggingEnabled);
	if (m_bus.getSerialLogging() != serialLoggingEnabled)
	{
		m_bus.setSerialLogging(serialLoggingEnabled);
	}

	ImGui::End();
}
