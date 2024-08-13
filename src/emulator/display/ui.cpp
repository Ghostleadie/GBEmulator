#include "ui.h"
#include "../emulator.h"

SDL_Window* sdlWindow;
SDL_Renderer* sdlRenderer;
SDL_Texture* sdlTexture;
SDL_Surface* screen;


ui::ui(std::shared_ptr<emulator> emulator)
{
	m_emulator = emulator;

}

void ui::init()
{

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		GBE_ERROR("SDL Error: {}", SDL_GetError());
		return ;
	}
	GBE_INFO("SDL Init");
	TTF_Init();
	GBE_INFO("SDL ttf Init");

	//SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	// sdlWindow;
	//sdlRenderer;
	//SDL_Window*  sdlWindow = SDL_CreateWindow("GameBoy Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
	//if (sdlWindow = nullptr) {  // This should be if (win == nullptr) {
	//	std::cout << SDL_GetError() << std::endl;
	//	return;
	//}
	//SDL_Renderer*  sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	SDL_CreateWindowAndRenderer(screenWidth, screenHeight, 0, &sdlWindow, &sdlRenderer);
}

void ui::handleEvents(std::weak_ptr<emulator> emulator)
{
	if (auto emu = emulator.lock())
	{
		SDL_Event event;
		while (SDL_PollEvent(&event) > 0)
		{
			if (event.type == SDL_QUIT)
			{
				emu->die = true;
				emu->running = false;
				SDL_DestroyRenderer(sdlRenderer);
				SDL_DestroyWindow(sdlWindow);
			}
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
			{
				emu->die = true;
				emu->running = false;
				SDL_DestroyRenderer(sdlRenderer);
				SDL_DestroyWindow(sdlWindow);
			}

		}
	}
}
