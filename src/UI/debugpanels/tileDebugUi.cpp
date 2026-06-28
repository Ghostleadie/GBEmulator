//
// Created by Jack_ on 28/06/2026.
//

#include "tileDebugUI.h"
#include "../../components/bus.h"
#include <SDL3/SDL.h>
#include "imgui.h"

// VRAM tile viewer, adapted from rockytriton/LLD_gbemu part11 lib/ui.c
// (update_dbg_window + display_tile). That tutorial draws into a *separate*
// SDL window; here the same tile sheet is decoded into an SDL_Texture and shown
// inside a Dear ImGui window instead.
//
// Layout: 384 tiles starting at 0x8000, 16 across x 24 down, each tile 8x8 px
// at 2 bits/pixel (16 bytes/tile). Palette matches the tutorial's tile_colors.
namespace
{
	constexpr uint16_t VRAM_TILE_BASE = 0x8000;
	constexpr int      BYTES_PER_TILE = 16;
	// DMG grayscale shades. ABGR8888 in memory, but grayscale so byte order is moot.
	constexpr uint32_t shade[4] = { 0xFFFFFFFFu, 0xFFAAAAAAu, 0xFF555555u, 0xFF000000u };
}

void tileDebugUI::init(SDL_Renderer* renderer)
{
	if (m_texture) SDL_DestroyTexture(m_texture); // idempotent: never leak on re-init
	m_renderer = renderer;
	m_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888,
								  SDL_TEXTUREACCESS_STREAMING, ATLAS_W, ATLAS_H);
	SDL_SetTextureScaleMode(m_texture, SDL_SCALEMODE_NEAREST); // crisp pixels, no blur
}

tileDebugUI::~tileDebugUI()
{
	if (m_texture) SDL_DestroyTexture(m_texture);
}

void tileDebugUI::updateUI(bus& busRef)
{
	// Open the window first so the decode + texture upload are skipped entirely
	// when the panel is collapsed or hidden.
	if (!ImGui::Begin("VRAM Tiles"))
	{
		ImGui::End();
		return;
	}

	// Lazily create the texture with the SAME SDL_Renderer Dear ImGui draws with.
	// imgui_impl_sdl3 stores the SDL_Window in the main viewport's PlatformHandle,
	// and SDL_GetRenderer returns the renderer bound to that window. This keeps the
	// tile viewer self-contained -- no SDL_Renderer plumbing through main/emulator.
	if (!m_texture)
	{
		// imgui_impl_sdl3 stores the SDL_WindowID (a Uint32), NOT the SDL_Window*,
		// in the viewport's PlatformHandle (changed 2024-08-19). Resolve the id to
		// the window, then to the renderer Dear ImGui draws with.
		const auto winId = static_cast<SDL_WindowID>(reinterpret_cast<uintptr_t>(ImGui::GetMainViewport()->PlatformHandle));
		if (SDL_Window* win = SDL_GetWindowFromID(winId))
		{
			init(SDL_GetRenderer(win));
		}
		if (!m_texture)
		{
			ImGui::TextUnformatted("Tile viewer: no SDL renderer available.");
			ImGui::End();
			return;
		}
	}

	// Decode all 384 tiles into the atlas.
	// 2bpp: the first byte of each row is the low bit-plane, the second the high
	// plane. (rockytriton's tutorial swaps these -- hi=first, lo=second -- which
	// swaps shades 1 and 2; this uses the hardware-correct order. Swap lo/hi below
	// to reproduce the video exactly.)
	for (int tile = 0; tile < COLS * ROWS; ++tile)              // 384 tiles
	{
		const int ox = (tile % COLS) * 8;                      // atlas pixel origin
		const int oy = (tile / COLS) * 8;
		const uint16_t base = static_cast<uint16_t>(VRAM_TILE_BASE + tile * BYTES_PER_TILE);
		for (int row = 0; row < 8; ++row)
		{
			const uint8_t lo = busRef.read(base + row * 2);
			const uint8_t hi = busRef.read(base + row * 2 + 1);
			for (int bit = 7; bit >= 0; --bit)                 // bit 7 = leftmost pixel
			{
				const uint8_t c = static_cast<uint8_t>((((hi >> bit) & 1) << 1) | ((lo >> bit) & 1));
				m_pixels[(oy + row) * ATLAS_W + (ox + (7 - bit))] = shade[c];
			}
		}
	}

	SDL_UpdateTexture(m_texture, nullptr, m_pixels, ATLAS_W * sizeof(uint32_t));

	static float zoom = 4.0f; // rockytriton uses scale = 4
	ImGui::SliderFloat("Zoom", &zoom, 1.0f, 8.0f, "%.0fx");
	ImGui::Image((ImTextureID)(intptr_t)m_texture, ImVec2(ATLAS_W * zoom, ATLAS_H * zoom));

	ImGui::End();
}
