//
// Game Boy LCD output (view layer).
//

#include "screen.h"

#include <SDL3/SDL.h>
#include "imgui.h"
#include "../components/ppu.h"

screen::~screen()
{
	if (m_texture) SDL_DestroyTexture(m_texture);
}

void screen::ensureTexture(SDL_Renderer* renderer)
{
	if (m_texture) return;

	m_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888,
								  SDL_TEXTUREACCESS_STREAMING,
								  ppu::ScreenWidth, ppu::ScreenHeight);
	SDL_SetTextureScaleMode(m_texture, SDL_SCALEMODE_NEAREST); // crisp pixels, no blur
}

void screen::present(SDL_Renderer* renderer, const ppu& m_ppu)
{
	if (!renderer) return;
	ensureTexture(renderer);
	if (!m_texture) return;

	// Thread-safe snapshot of the last completed frame. The PPU runs on the
	// emulation thread and swaps its present buffer under a lock at VBLANK; this
	// copies it out under the same lock, so we never read a half-drawn frame.
	m_ppu.copyFrame(m_pixels.data());
	SDL_UpdateTexture(m_texture, nullptr, m_pixels.data(),
					  ppu::ScreenWidth * sizeof(uint32_t));

	// Fit the 160x144 frame inside the window preserving aspect (letterbox), and
	// center it. Recomputed each frame so it tracks window resizes.
	int outW = 0, outH = 0;
	SDL_GetRenderOutputSize(renderer, &outW, &outH);

	// Reserve the main menu bar strip at the top so the LCD sits below the bar
	// instead of being clipped underneath it. GetFrameHeight is font + padding,
	// stable for the whole frame regardless of when it's queried.
	const float top = ImGui::GetFrameHeight();
	const float availH = SDL_max(outH - top, 1.0f);

	const float scale = SDL_min(outW / static_cast<float>(ppu::ScreenWidth),
								availH / static_cast<float>(ppu::ScreenHeight));
	const float w = ppu::ScreenWidth  * scale;
	const float h = ppu::ScreenHeight * scale;
	const SDL_FRect dst = { (outW - w) * 0.5f, top + (availH - h) * 0.5f, w, h };

	SDL_RenderTexture(renderer, m_texture, nullptr, &dst);
}
