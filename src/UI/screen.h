//
// Game Boy LCD output (view layer).
//

#ifndef GAMEBOYEMULATOR_SCREEN_H
#define GAMEBOYEMULATOR_SCREEN_H

#include <array>
#include <cstdint>

struct SDL_Texture;
struct SDL_Renderer;
class ppu;

// Presents the PPU framebuffer on screen. This is the VIEW for the Game Boy LCD:
// it owns the SDL_Texture and blits it, scaled, to the main window. The pixel data
// (the LCD's actual state) lives in the ppu, in components/ -- this class only
// draws it. Deliberately NOT in components/ so the headless core (romRunner) stays
// free of SDL.
//
// Runs on the main/render thread. Each present pulls a thread-safe snapshot of the
// last completed frame from the ppu, which is driven on the emulation thread.
class screen
{
public:
	/** Destroys the streaming texture if one was created. */
	~screen();

	/**
	 * Blits the latest PPU frame to the window, aspect-correct and letterboxed.
	 * Call once per frame on the main thread with the SDL_Renderer bound to the
	 * window, AFTER clearing it and BEFORE ImGui's draw data, so the ImGui panels
	 * overlay on top of the game.
	 * @param renderer SDL renderer bound to the main window; ignored when null.
	 * @param gpu PPU providing a thread-safe snapshot of the last completed frame.
	 */
	void present(SDL_Renderer* renderer, const ppu& gpu);

private:
	/**
	 * Lazily creates the streaming texture with the given renderer.
	 * No-op once the texture has been made.
	 * @param renderer SDL renderer used to create the texture.
	 */
	void ensureTexture(SDL_Renderer* renderer);

	SDL_Texture* m_texture = nullptr;
	std::array<uint32_t, 160 * 144> m_pixels = {}; // main-thread scratch copy of the frame
};

#endif //GAMEBOYEMULATOR_SCREEN_H
