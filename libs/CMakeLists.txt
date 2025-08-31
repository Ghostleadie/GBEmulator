include(FetchContent)

# spdlog
find_package(spdlog QUIET)
if(NOT spdlog_FOUND)
    message(STATUS "spdlog not found, fetching...")
    FetchContent_Declare(
      spdlog
      GIT_REPOSITORY https://github.com/gabime/spdlog.git
      GIT_TAG v1.14.1
    )
    FetchContent_MakeAvailable(spdlog)
endif()

# ImGui
find_package(imgui QUIET)
if(NOT imgui_FOUND)
    FetchContent_Declare(
      imgui
      GIT_REPOSITORY https://github.com/ocornut/imgui.git
      GIT_TAG master
    )
    FetchContent_MakeAvailable(imgui)
      add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.h
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.h
        ${imgui_SOURCE_DIR}/imgui.h
        ${imgui_SOURCE_DIR}/imconfig.h
        ${imgui_SOURCE_DIR}/imgui_internal.h
        ${imgui_SOURCE_DIR}/imstb_rectpack.h
        ${imgui_SOURCE_DIR}/imstb_textedit.h
        ${imgui_SOURCE_DIR}/imstb_truetype.h
      )
      target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${SDL_SOURCE_DIR}/include
      )
      target_link_libraries(imgui PUBLIC SDL3::SDL3)


    add_library(imgui::imgui ALIAS imgui)
endif ()

# Freetype
# find_package(Freetype QUIET)
#if(NOT Freetype_FOUND)
#  message(STATUS "Freetype not found, fetching...")
#  FetchContent_Declare(
#          freetype
#          GIT_REPOSITORY https://github.com/freetype/freetype.git
#          GIT_TAG VER-2-13-2
#  )
#  FetchContent_MakeAvailable(freetype)
#endif()

# SDL
find_package(SDL QUIET)
if(NOT SDL_FOUND)
    message(STATUS "SDL not found, fetching...")
    FetchContent_Declare(
      SDL
      GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
      GIT_TAG main
    )
    FetchContent_MakeAvailable(SDL)
endif()
# SDL_ttf
find_package(SDL_ttf QUIET)
if(NOT SDL_ttf_FOUND)
    message(STATUS "SDL_ttf not found, fetching...")
    FetchContent_Declare(
      SDL_ttf
      GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
      GIT_TAG release-3.2.2
    )
    FetchContent_MakeAvailable(SDL_ttf)
endif()

# Native File Dialog Extended
find_package(nfd QUIET)
if(NOT nfd_FOUND)
    message(STATUS "nfd not found, fetching...")
    FetchContent_Declare(
      nfd
      GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended.git
      GIT_TAG master
    )
    FetchContent_MakeAvailable(nfd)
endif()