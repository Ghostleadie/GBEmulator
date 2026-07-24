#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <nfd.h>

#include "emulator.h"
#include "imgui.h"
#include "imgui_internal.h"   // window list, for keeping ImGui windows on screen on resize
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
// main.cpp

using enum emulatorStates;

/* We will use this renderer to draw into this window every frame. */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static emulator emu;

/**
 * Runs once at startup: initialises logging, SDL, SDL_ttf, the window and
 * renderer, ImGui, native file dialogs, and the emulator itself.
 * @param appstate Unused application-state pointer from the SDL callback framework.
 * @param argc Command-line argument count.
 * @param argv Command-line argument values.
 * @return SDL_APP_CONTINUE on success, or SDL_APP_FAILURE if a subsystem fails to start.
 */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("GameBoy Emulator", "0.0.1", "com.emulator.gameboy");
    log::Init();
if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO))
	{
        LOG_ERROR("SDL_Init Error: {}", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    LOG_INFO("SDL initialized");
    if (!TTF_Init())
    {
    	LOG_ERROR("TTF_Init Error: {}", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    LOG_INFO("SDL_ttf initialized");
    if (!SDL_CreateWindowAndRenderer("GameBoy Emulator", 640, 480, SDL_WINDOW_RESIZABLE, &window, &renderer))
    {
        LOG_ERROR("Couldn't create window/renderer: {}", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    LOG_INFO("SDL Window and Renderer created");
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
    NFD_Init();

	emu.initalizeEmulator();
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/**
 * Runs when a new event (mouse input, keypresses, etc) occurs.
 * Forwards the event to ImGui, toggles the debug UI (grave), pauses or resumes
 * (space), and quits on Escape unless the settings window wants it.
 * @param appstate Unused application-state pointer from the SDL callback framework.
 * @param event The SDL event to handle.
 * @return SDL_APP_SUCCESS to quit, otherwise SDL_APP_CONTINUE.
 */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
	ImGui_ImplSDL3_ProcessEvent(event);
	if (event->type == SDL_EVENT_KEY_DOWN)
	{
		if (event->key.key == SDLK_GRAVE)
		{
			emu.debugUIActive = !emu.debugUIActive;
		}
		if (event->key.key == SDLK_SPACE && emu.state != EMU_STATE_MENU)
		{
			if (emu.state == EMU_STATE_RUNNING)
			{
				emu.state = EMU_STATE_PAUSED;
				LOG_INFO("Emulator Paused");
			}
			else if (emu.state == EMU_STATE_PAUSED)
			{
				emu.state = EMU_STATE_RUNNING;
				LOG_INFO("Emulator Running");
			}
		}
		// Escape quits, except when the settings window wants it: binding Escape
		// to an input, or dismissing the unsaved-changes prompt.
		if (event->key.key == SDLK_ESCAPE && !emu.settingsWantsEscape())
		{
			return SDL_APP_SUCCESS;
		}
	}
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/**
 * Pulls every movable ImGui window back inside the current display bounds.
 * Called when the host window is resized (typically shrunk) so panels that were
 * placed in the old, larger area do not end up stranded off-screen and
 * unreachable. Windows that fit are clamped fully on-screen; ones larger than
 * the display are pinned to the top-left. Fixed (NoMove), child, popup and
 * tooltip windows are left alone.
 */
static void keepImGuiWindowsOnScreen()
{
	ImGuiContext* ctx = ImGui::GetCurrentContext();
	if (!ctx)
	{
		return;
	}
	const ImVec2 disp = ctx->IO.DisplaySize;
	if (disp.x <= 0.0f || disp.y <= 0.0f)
	{
		return;
	}

	for (ImGuiWindow* w : ctx->Windows)
	{
		if (!w->WasActive)
		{
			continue;
		}
		if (w->Flags & (ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_Tooltip |
						ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoMove))
		{
			continue;
		}

		ImVec2 pos = w->Pos;
		pos.x = (w->Size.x <= disp.x) ? ImClamp(pos.x, 0.0f, disp.x - w->Size.x) : 0.0f;
		pos.y = (w->Size.y <= disp.y) ? ImClamp(pos.y, 0.0f, disp.y - w->Size.y) : 0.0f;

		if (pos.x != w->Pos.x || pos.y != w->Pos.y)
		{
			ImGui::SetWindowPos(w, pos, ImGuiCond_Always);
		}
	}
}

/**
 * Runs once per frame, and is the heart of the program.
 * Starts a new ImGui frame, updates the emulator, then renders the Game Boy
 * screen beneath the ImGui overlay and presents at roughly 60 FPS.
 * @param appstate Unused application-state pointer from the SDL callback framework.
 * @return SDL_APP_CONTINUE to keep running.
 */
SDL_AppResult SDL_AppIterate(void *appstate)
{
	// Start a new ImGui frame once per iteration
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	// Re-clamp panels into view whenever the display size changes, so shrinking
	// the window never strands a panel off-screen. Cheap and skipped when the
	// size is unchanged. DisplaySize is set by ImGui_ImplSDL3_NewFrame above.
	static ImVec2 lastDisplaySize(0.0f, 0.0f);
	const ImVec2 disp = ImGui::GetIO().DisplaySize;
	if ((disp.x != lastDisplaySize.x || disp.y != lastDisplaySize.y) &&
		lastDisplaySize.x != 0.0f && lastDisplaySize.y != 0.0f)
	{
		keepImGuiWindowsOnScreen();
	}
	lastDisplaySize = disp;

	emu.runEmulator();
    ImGui::Render();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    emu.renderFrame(renderer); // Game Boy screen, beneath the ImGui overlay
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(16); // ~60 FPS

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/**
 * Runs once at shutdown: tears down ImGui, native file dialogs, SDL_ttf, and SDL.
 * @param appstate Unused application-state pointer from the SDL callback framework.
 * @param result The result code that triggered shutdown.
 */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
	// Stop the emulation thread and flush the battery-backed save before teardown.
	// Also ensures the thread is joined before the static emulator is destroyed.
	emu.stopEmulation();

	//imgui shutdown
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
	//nfd shutdown
    NFD_Quit();
	//sdl ttf shutdown
    TTF_Quit();
	//sdl shutdown
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
    SDL_Quit();
}