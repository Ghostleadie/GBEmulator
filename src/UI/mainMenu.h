//
// Created by Jack_ on 30/08/2025.
//

#ifndef GAMEBOYEMULATOR_MAINMENU_H
#define GAMEBOYEMULATOR_MAINMENU_H


class mainMenu
{
public:
	/** One-time initialisation hook for the main menu, run once at startup. */
	void init();

	/**
	 * Draws the main menu window for one frame and handles its buttons.
	 * "Load Rom" opens a native file dialog; "Quit" posts an SDL quit event.
	 * @param filepath Receives the selected ROM path when the user loads one.
	 * @return True while the menu should stay open; false once a ROM has been selected.
	 */
	bool openMainMenu(std::string& filepath);
private:
	bool showSettingsWindow = false;
	bool showAboutWindow = false;
	bool showLoadCartridgeFileExplorer = false;
	//ImGuiIO& io = ImGui::GetIO();
};


#endif //GAMEBOYEMULATOR_MAINMENU_H