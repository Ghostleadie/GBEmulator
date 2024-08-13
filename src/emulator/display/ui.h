#pragma once

#include "SDL2/SDL.h"
#include "SDL_ttf.h"
struct emulator;

class ui
{
public:
	ui() {};
	ui(std::shared_ptr <emulator> emulator);
	void init();
	void handleEvents(std::weak_ptr<emulator> emulator);


	
	SDL_Texture* sdlTexture;
	SDL_Surface* screen;

private:
	std::shared_ptr <emulator> m_emulator;

	static const int screenWidth = 1024;
	static const int screenHeight = 768;
};