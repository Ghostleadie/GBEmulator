#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <spdlog/spdlog.h>
#include <nfd.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

int main(int, char**)
{
    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        spdlog::error("SDL_Init Error: {}", SDL_GetError());
        return -1;
    }
    if (TTF_Init() == -1) {
        //spdlog::error("TTF_Init Error: {}", TTF_GetError());
        return -1;
    }

    NFD_Init();

    SDL_Window* window = SDL_CreateWindow("My ImGui + SDL3 App", 1280, 720, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    // Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // Example UI
        ImGui::Begin("Hello, world!");
        ImGui::Text("This is an ImGui + SDL2 + SDL_ttf example.");
        if (ImGui::Button("Open File...")) {
            nfdchar_t* outPath = nullptr;
            // Add file filters for a better user experience
            nfdfilteritem_t filters[] = { { "Source Files", "c,cpp,cc,h,hpp" }, { "All Files", "*" } };
            nfdresult_t result = NFD_OpenDialog(&outPath, filters, 2, nullptr);

            if (result == NFD_OKAY) {
                spdlog::info("Selected file: {}", outPath);
                NFD_FreePath(outPath);  // Use NFD_FreePath instead of free
            }
            else if (result == NFD_CANCEL) {
                spdlog::info("User canceled file dialog");
            }
            else {
                spdlog::error("Error opening file dialog: {}", NFD_GetError());
            }
        }
        ImGui::End();

        // Render
        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    NFD_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}
