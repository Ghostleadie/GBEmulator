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
	/** Destroys the tile-atlas SDL texture, if one was created. */
	~tileDebugUI();
	/**
	 * Creates the streaming SDL texture used as the tile atlas.
	 *
	 * Idempotent: any existing texture is destroyed first, so re-init never leaks.
	 * @param renderer SDL renderer the texture is created on and later drawn with.
	 */
	void init(SDL_Renderer* renderer);
	/**
	 * Rebuilds the VRAM tile atlas and draws it; called once per frame.
	 *
	 * Decodes the 384 tiles at 0x8000 (16x24, 2bpp) into the texture, uploads it,
	 * and shows the sheet with a zoom slider. The texture is lazily created on the
	 * first call using the renderer Dear ImGui draws with; the decode and upload
	 * are skipped entirely while the window is collapsed or hidden.
	 * @param m_bus Bus used to read the VRAM tile bytes.
	 * @param open Caller's visibility flag; the window's close button clears it.
	 */
	void updateUI(bus& m_bus, bool* open);
private:
	static constexpr int COLS = 16, ROWS = 24;
	static constexpr int ATLAS_W = COLS * 8;   // 128
	static constexpr int ATLAS_H = ROWS * 8;   // 192
	SDL_Renderer* m_renderer = nullptr;
	SDL_Texture*  m_texture  = nullptr;
	uint32_t      m_pixels[ATLAS_W * ATLAS_H] = {};
};

#endif //GAMEBOYEMULATOR_TILEDEBUGUI_H