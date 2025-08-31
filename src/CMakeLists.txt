
# Main application executable
add_executable(GameBoyEmulator main.cpp
        log/Log.cpp
        log/Log.cpp
        Components/Component.cpp
        Components/Component.h
        Emulator.cpp
        Emulator.h
        UI/debugUI.cpp
        UI/debugUI.h
        UI/mainMenu.cpp
        UI/mainMenu.h
        UI/pauseMenu.cpp
        UI/pauseMenu.h
        Components/ComponentMessanger.cpp
        Components/componentMessanger.h
        Components/cpu.cpp
        Components/cpu.h
        Components/cartridgeLoader.cpp
        Components/cartridgeLoader.h)

target_precompile_headers(GameBoyEmulator PUBLIC pch.h)

target_link_libraries(GameBoyEmulator PRIVATE
    imgui
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
    spdlog::spdlog
    nfd
)