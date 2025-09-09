//
// Created by Jack_ on 30/08/2025.
//

#include "mainMenu.h"
#include "imgui.h"
#include "nfd.h"
#include "SDL3/SDL.h"

void mainMenu::init()
{
	//(void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
}

bool mainMenu::openMainMenu(std::string& filepath)
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::SetNextWindowSize(ImVec2(150, 250), ImGuiCond_FirstUseEver);
	ImGui::Begin("GameBoy Emulator", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
	if (ImGui::Button("Load Rom")) {
		nfdu8char_t* outPath;
		nfdu8filteritem_t filters[1] = { { "Roms", "gb" } };
		nfdopendialogu8args_t args = { 0 };
		args.filterList = filters;
		args.filterCount = 1;
		nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
		if (result == NFD_OKAY)
		{
			const char* path = outPath;
			LOG_INFO("Rom Path: {}", path);
			filepath = outPath; // Store the selected file path
			NFD_FreePathU8(outPath);
			ImGui::End();
			return false;
		}
		else if (result == NFD_CANCEL)
		{
			LOG_INFO("User pressed cancel.");
		}
		else
		{
			LOG_ERROR("NFD: {}", NFD_GetError());
		}
	}
	if (ImGui::Button("Settings"))
	{

	}
	if (ImGui::Button("Quit"))
	{
		SDL_Event e{};
		e.type = SDL_EVENT_QUIT;
		SDL_PushEvent(&e);
	}
	ImGui::End();
	return true;
}