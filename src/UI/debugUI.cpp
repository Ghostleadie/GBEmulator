//
// Created by Jack_ on 30/08/2025.
//

#include "debugUI.h"
#include "../components/cartridgeLoader.h"
#include "../Utility/settings.h"
#include "imgui.h"

namespace
{
	// Section of settings/settings.json this panel state lives under.
	constexpr const char* PANELS_KEY = "debugPanels";

	/**
	 * Reads a boolean entry from a JSON object into `out` when present.
	 * Leaves `out` alone unless the key exists and holds a bool, so a partial or
	 * hand-edited section still picks up whatever entries are valid.
	 * @param obj JSON object to read from.
	 * @param key Entry name to look up.
	 * @param out Destination, left unchanged when the key is missing or not a bool.
	 */
	void readFlag(const nlohmann::json& obj, const char* key, bool& out)
	{
		if (const auto it = obj.find(key); it != obj.end() && it->is_boolean())
		{
			out = it->get<bool>();
		}
	}
}

void debugUI::loadPanelState()
{
	const nlohmann::json root = settings::load();

	if (const auto it = root.find(PANELS_KEY); it != root.end() && it->is_object())
	{
		readFlag(*it, "romDebug",  m_panels.cartridge);
		readFlag(*it, "cpuDebug",  m_panels.cpu);
		readFlag(*it, "busDebug",  m_panels.bus);
		readFlag(*it, "vramTiles", m_panels.tiles);
	}

	// Whatever we ended up with is what the file effectively holds, so the first
	// frame does not write the defaults straight back out.
	m_savedPanels = m_panels;
}

void debugUI::savePanelStateIfChanged()
{
	if (m_panels == m_savedPanels)
	{
		return;
	}

	nlohmann::json section;
	section["romDebug"]  = m_panels.cartridge;
	section["cpuDebug"]  = m_panels.cpu;
	section["busDebug"]  = m_panels.bus;
	section["vramTiles"] = m_panels.tiles;

	// Track the attempt either way: a failing write logs once instead of retrying
	// every frame for the rest of the session.
	settings::saveSection(PANELS_KEY, section);
	m_savedPanels = m_panels;
}

void debugUI::drawMenuBar(bool* debugActive, bool* showSettings)
{
	// This runs every frame even when the panels are hidden, so it owns the
	// load/save of the panel state rather than UpdateUIPanels.
	if (!m_stateLoaded)
	{
		loadPanelState();
		m_stateLoaded = true;
	}

	// Only call EndMainMenuBar when Begin succeeded (ImGui contract).
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Debug"))
		{
			// Master switch, same one the ` key flips.
			ImGui::MenuItem("Debug Panels", "`", debugActive);
			ImGui::Separator();

			// The individual toggles do nothing while the master switch is off.
			ImGui::BeginDisabled(!*debugActive);

			// Labels match the window titles so the menu reads as a window list.
			ImGui::MenuItem("Rom Debug",  nullptr, &m_panels.cartridge);
			ImGui::MenuItem("CPU Debug",  nullptr, &m_panels.cpu);
			ImGui::MenuItem("Bus Debug",  nullptr, &m_panels.bus);
			ImGui::MenuItem("VRAM Tiles", nullptr, &m_panels.tiles);

			ImGui::Separator();
			if (ImGui::MenuItem("Show All"))
			{
				m_panels = { true, true, true, true };
			}
			if (ImGui::MenuItem("Hide All"))
			{
				m_panels = { false, false, false, false };
			}

			ImGui::EndDisabled();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Settings"))
		{
			// Deliberately an action, not a checkbox: the window owns its own
			// close so it can prompt about unsaved changes first.
			if (ImGui::MenuItem("Controls and Audio"))
			{
				*showSettings = true;
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	savePanelStateIfChanged();
}

void debugUI::UpdateUIPanels()
{
	if (m_panels.cartridge)
	{
		cartridgeDebugUI.updateUI(m_Loader.lock()->peekCartridgeContext(), &m_panels.cartridge);
	}
	if (m_panels.cpu)
	{
		cpuDebugUI.updateUI(*m_cpu.lock(), &m_panels.cpu);
	}
	if (m_panels.bus)
	{
		busDebugUI.updateUI(*m_bus.lock(), &m_panels.bus);
	}
	if (m_panels.tiles)
	{
		tileDebugUI.updateUI(*m_bus.lock(), &m_panels.tiles);
	}
}
