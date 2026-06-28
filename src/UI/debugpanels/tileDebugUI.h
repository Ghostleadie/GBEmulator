//
// Created by Jack_ on 28/06/2026.
//

#ifndef GAMEBOYEMULATOR_TILEDEBUGUI_H
#define GAMEBOYEMULATOR_TILEDEBUGUI_H

#include <cstdint>
struct SDL_Renderer;
struct SDL_Texture;
class bus;

class tileDebugUI
{
public:
	~tileDebugUI();
	void init(SDL_Renderer* renderer);   // create the texture once
	void updateUI(bus& m_bus);           // rebuild atlas + draw each frame
private:
	static constexpr int COLS = 16, ROWS = 24;
	static constexpr int ATLAS_W = COLS * 8;   // 128
	static constexpr int ATLAS_H = ROWS * 8;   // 192
	SDL_Renderer* m_renderer = nullptr;
	SDL_Texture*  m_texture  = nullptr;
	uint32_t      m_pixels[ATLAS_W * ATLAS_H] = {};
};

#endif //GAMEBOYEMULATOR_TILEDEBUGUI_H