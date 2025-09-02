//
// Created by Jack_ on 30/08/2025.
//

#ifndef GAMEBOYEMULATOR_MAINMENU_H
#define GAMEBOYEMULATOR_MAINMENU_H
#include "imgui.h"


class mainMenu
{
public:
	void init();
	bool openMainMenu(std::string& filepath);
private:
	bool showSettingsWindow = false;
	bool showAboutWindow = false;
	bool showLoadCartridgeFileExplorer = false;
	//ImGuiIO& io = ImGui::GetIO();
};


#endif //GAMEBOYEMULATOR_MAINMENU_H