#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <nfd.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
// main.cpp

/* We will use this renderer to draw into this window every frame. */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("GameBoy Emulator", "0.0.1", "com.emulator.gameboy");
    log::Init();
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        //LOG_ERROR("SDL_Init Error: {}", SDL_GetError());
        SDL_Log("\033[31mCouldn't initialize SDL: %s \033[0m", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    LOG_INFO("SDL initialized");
    if (!TTF_Init()) {
        SDL_Log("\033[31mCouldn't initialize SDL_ttf: %s \033[0m", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    LOG_INFO("SDL_ttf initialized");
    if (!SDL_CreateWindowAndRenderer("GameBoy Emulator", 640, 480, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("\033[31mCouldn't create window/renderer: %s\033[0m", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    LOG_INFO("SDL Window and Renderer created");
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
    NFD_Init();
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Hello, world!");
    ImGui::Text("This is an ImGui + SDL3 + SDL_ttf example.");
    if (ImGui::Button("Open File...")) {
        nfdchar_t* outPath = nullptr;
        nfdfilteritem_t filters[] = { { "Source Files", "c,cpp,cc,h,hpp" }, { "All Files", "*" } };
        nfdresult_t result = NFD_OpenDialog(&outPath, filters, 2, nullptr);

        if (result == NFD_OKAY) {
            NFD_FreePath(outPath);
        }
    }
    ImGui::End();

    ImGui::Render();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(16); // ~60 FPS

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    NFD_Quit();
    TTF_Quit();
    SDL_Quit();
    /* SDL will clean up the window/renderer for us. */
}