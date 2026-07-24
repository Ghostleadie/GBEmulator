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

// Which debug windows are open. Persisted to settings/settings.json so the
// layout survives a restart.
struct panelVisibility
{
	bool cartridge = true;
	bool cpu       = true;
	bool bus       = true;
	bool tiles     = true;

	bool operator==(const panelVisibility&) const = default;
};

class debugUI
{
public:
	debugUI() = default;
	/**
	 * Constructs the debug UI with the core components its panels inspect.
	 * @param cartridge_loader Cartridge loader, queried for the loaded ROM's context.
	 * @param cpu CPU whose state the CPU panel displays.
	 * @param bus Bus backing the bus and VRAM-tile panels.
	 * Components are held as weak_ptrs, so the debug UI never keeps one alive.
	 */
	debugUI(const std::shared_ptr<cartridgeLoader>& cartridge_loader, const std::shared_ptr<cpu>& cpu, const std::shared_ptr<bus>& bus) : m_cpu(cpu), m_Loader(cartridge_loader), m_bus(bus){};
	~debugUI() = default;

	/**
	 * Draws the top-of-screen main menu bar for one frame.
	 * Drawn every frame regardless of whether the panels are showing, so it is
	 * always reachable, and owns the first-frame load and per-frame save of the
	 * panel visibility state.
	 * @param debugActive Master toggle for the debug panels (also bound to the ` key); edited in place.
	 * @param showSettings Settings-window visibility flag; set true when the user opens it; edited in place.
	 *
	 * The bar is app-wide chrome rather than debug-only at this point -- it lives
	 * here because that is where it started, not because it belongs to debugUI.
	 */
	void drawMenuBar(bool* debugActive, bool* showSettings);

	/**
	 * Draws each enabled debug panel for one frame.
	 * Only call when the master toggle is on; each panel is drawn only while its
	 * own visibility flag is set.
	 */
	void UpdateUIPanels();

private:
	/**
	 * Loads the panel visibility flags from settings/settings.json.
	 * Read once on the first frame -- the settings file is not available at
	 * construction time in every entry path.
	 */
	void loadPanelState();

	/**
	 * Writes the panel visibility flags back to settings/settings.json when they change.
	 * Rewrites the file only when a toggle actually moved, so the common case
	 * costs one struct comparison per frame.
	 */
	void savePanelStateIfChanged();

	panelVisibility m_panels;
	panelVisibility m_savedPanels;  // last state written to disk
	bool m_stateLoaded = false;

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