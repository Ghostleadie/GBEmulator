
# Main application executable
add_executable(GameBoyEmulator main.cpp
        log/Log.cpp
        log/Log.cpp
        Emulator.cpp
        Emulator.h
        UI/debugUI.cpp
        UI/debugUI.h
        UI/mainMenu.cpp
        UI/mainMenu.h
        UI/pauseMenu.cpp
        UI/pauseMenu.h
        interfaces/IComponentMessanger.h
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
        Components/cpu/opcodes.cpp
        Components/cpu/opcodes.h
        Components/cpu/opcodes.h
        interfaces/ICartridgeDebug.h
        UI/debugpanels/cartridgedebugUI.cpp
        UI/debugpanels/cartridgedebugUI.h
        UI/debugpanels/busDebugUI.cpp
        UI/debugpanels/busDebugUI.h
        Utility/utility.h
        UI/debugpanels/cpuDebugUI.cpp
        UI/debugpanels/cpuDebugUI.h
        components/cpu/opcodeFunctions.cpp
        components/cpu/opcodeFunctions.h
        Utility/SerialPortDebugger.h
        Utility/SerialPortDebugger.cpp
        interfaces/IClock.h
        components/emulatorClock.cpp
        components/emulatorClock.h
        components/interruptController.cpp
        components/interruptController.h
        interfaces/InterruptSink.h
)

target_precompile_headers(GameBoyEmulator PUBLIC pch.h)

# Treat "missing return value" as a hard error so UB fall-through can't compile.
if (MSVC)
    target_compile_options(GameBoyEmulator PRIVATE /we4715 /we4716)
else()
    target_compile_options(GameBoyEmulator PRIVATE -Werror=return-type)
endif()

target_link_libraries(GameBoyEmulator PRIVATE
    imgui
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
    spdlog::spdlog
    nfd
    nlohmann_json
)

add_custom_command(TARGET GameBoyEmulator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:nfd>
        $<TARGET_FILE_DIR:GameBoyEmulator>
)

add_custom_command(TARGET GameBoyEmulator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE_DIR:GameBoyEmulator>
)

add_custom_command(TARGET GameBoyEmulator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3_ttf::SDL3_ttf>
        $<TARGET_FILE_DIR:GameBoyEmulator>
)

add_custom_command(TARGET GameBoyEmulator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:spdlog::spdlog>
        $<TARGET_FILE_DIR:GameBoyEmulator>
)