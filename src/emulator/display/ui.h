#pragma once

class ui
{
public:
	ui() {};
	void init();
	void handleEvent();

private:
	std::unique_ptr<SDL_Window> sdlWindow;
	std::unique_ptr<SDL_Renderer> sdlRenderer;
	std::unique_ptr<SDL_Texture> sdlTexture;
	std::unique_ptr<SDL_Surface> screen;
};