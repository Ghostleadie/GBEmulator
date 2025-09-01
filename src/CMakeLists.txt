
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
        Components/cpu/cpu.cpp
        Components/cpu/cpu.h
        Components/cartridgeLoader.cpp
        Components/cartridgeLoader.h
        Components/bus.cpp
        Components/bus.h
        Components/bus.h
        Components/ppu.cpp
        Components/ppu.h
        Components/timer.cpp
        Components/timer.h
        Components/joypad.cpp
        Components/joypad.h
        Components/apu.cpp
        Components/apu.h
        Components/cpu/opcodes.cpp
        Components/cpu/opcodes.h
        Components/cpu/opcodes.h)

target_precompile_headers(GameBoyEmulator PUBLIC pch.h)

target_link_libraries(GameBoyEmulator PRIVATE
    imgui
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
    spdlog::spdlog
    nfd
    nlohmann_json
)