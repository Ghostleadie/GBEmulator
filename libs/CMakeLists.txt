include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)

#----------------------------------------------------------------------

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.14.1
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
    #SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/spdlog
)

message("Fetching spdlog")
FetchContent_MakeAvailable(spdlog)

#----------------------------------------------------------------------

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG        v1.90.7
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
    #SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/imgui
)

FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    message("Fetching imgui")
    FetchContent_Populate(imgui)

    add_library(imgui
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.h
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.h
        ${imgui_SOURCE_DIR}/imgui.h
        ${imgui_SOURCE_DIR}/imconfig.h
        ${imgui_SOURCE_DIR}/imgui_internal.h
        ${imgui_SOURCE_DIR}/imstb_rectpack.h
        ${imgui_SOURCE_DIR}/imstb_textedit.h
        ${imgui_SOURCE_DIR}/imstb_truetype.h)

    target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${glfw_SOURCE_DIR}/include
        ${sdl2_SOURCE_DIR}/include/sdl/include)

    target_link_libraries(imgui PUBLIC SDL2::SDL2)
endif ()

FetchContent_MakeAvailable(imgui)

#----------------------------------------------------------------------

FetchContent_Declare(
  SDL2
  GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
  GIT_TAG release-2.30.3
  GIT_PROGRESS   TRUE
  #SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/SDL2
)
FetchContent_GetProperties(SDL2)
if(NOT sdl2_POPULATED)
  FetchContent_Populate(SDL2)
  message(STATUS "Catch source dir: ${sdl2_SOURCE_DIR}")
  message(STATUS "Catch binary dir: ${sdl2_BINARY_DIR}")
  add_subdirectory(${sdl2_SOURCE_DIR} ${sdl2_BINARY_DIR})
endif()


#set(SDL2_LIBRARY "${CMAKE_SOURCE_DIR}/include/sdl/lib")
#set(SDL2_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/sdl/include")

message("Fetching SDL2")

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(SDL2_DISABLE_SDL2MAIN ON)

#set(SDL_LIBRARIES ${SDL_LIBRARIES} SDL2main SDL2-static)
#list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

FetchContent_MakeAvailable(SDL2)

#----------------------------------------------------------------------

FetchContent_Declare(
  SDL2_ttf
  GIT_REPOSITORY "https://github.com/libsdl-org/SDL_ttf.git"
  GIT_TAG  release-2.22.0
  GIT_PROGRESS   TRUE
  #SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/SDL2_ttf
)

message("Fetching SDL_ttf")

FetchContent_MakeAvailable(SDL2_ttf)

#----------------------------------------------------------------------

FetchContent_Declare(
  entt
  GIT_REPOSITORY "https://github.com/skypjack/entt.git"
  GIT_TAG  v3.13.2
  GIT_PROGRESS   TRUE
  #SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/SDL2_ttf
)

message("Fetching entt")

FetchContent_MakeAvailable(entt)