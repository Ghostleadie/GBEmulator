include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)

#----------------------------------------------------------------------

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.14.1
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
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
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
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

    target_link_libraries(imgui PRIVATE glfw)
    target_link_libraries(imgui PUBLIC SDL2::SDL2)
endif ()

FetchContent_MakeAvailable(imgui)

#----------------------------------------------------------------------

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(GLFW_LIBRARY_TYPE "Link glfw static or dynamic" STATIC)
option(GLFW_BUILD_TESTS "" OFF)
option(GLFW_BUILD_DOCS "" OFF)
option(GLFW_INSTALL "" OFF)
option(GLFW_BUILD_EXAMPLES "" OFF)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG        3.3.8
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)
message("Fetching glfw")
FetchContent_MakeAvailable(glfw)

#----------------------------------------------------------------------

FetchContent_Declare(
  SDL2
  GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
  GIT_TAG release-2.30.3
  GIT_PROGRESS   TRUE
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

#list(APPEND SDL2_LIBRARIES ${PROJECT_SOURCE_DIR}/build/Debug/SDL2maind.lib)
#list(APPEND SDL2_LIBRARIES ${PROJECT_SOURCE_DIR}/build/Debug/SDL2-staticd.lib)
#list(APPEND SDL2_LIBRARIES ${PROJECT_SOURCE_DIR}/build/Debug/SDL2d.lib)
