#include <type_traits>
#include "SDL2/SDL.h"

template<typename LibraryType>
struct Deleter {};

template<>
struct Deleter<SDL_Window> {
    void operator()(SDL_Window* ptr) {
        SDL_DestroyWindow(ptr);
    }
};

template<>
struct Deleter<SDL_Renderer> {
    void operator()(SDL_Renderer* ptr) {
        SDL_DestroyRenderer(ptr);
    }
};

template<>
struct Deleter<SDL_Texture> {
    void operator()(SDL_Texture* ptr) {
        SDL_DestroyTexture(ptr);
    }
};

template<>
struct Deleter<SDL_Surface> {
    void operator()(SDL_Surface* ptr) {
        SDL_FreeSurface(ptr);
    }
};