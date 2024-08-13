# CMakeList.txt : CMake project for GBEmulator, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (GBEmulator "emulator/main.cpp" "emulator/main.h" "log/Log.cpp" "log/Log.h" "emulator/bus.cpp" "emulator/bus.h" "emulator/cartridge.cpp" "emulator/cartridge.h" "emulator/cpu/cpu.cpp" "emulator/cpu/cpu.h" "emulator/ppu.cpp" "emulator/timer.cpp" "emulator/ppu.h" "emulator/timer.h" "emulator/emulator.cpp" "emulator/emulator.h" "emulator/instructions.h" "emulator/instructions.cpp" "emulator/utility/utility.h" "emulator/memory.h" "emulator/memory.cpp" "emulator/cpu/cpuInstructions.cpp" "emulator/cpu/interrupts.h" "emulator/cpu/interrupts.cpp" "emulator/display/ui.h" "emulator/display/ui.cpp")

#find_package(SDL2 REQUIRED)
#find_package(SDL_ttf REQUIRED)
target_link_libraries(GBEmulator PUBLIC glfw imgui spdlog SDL2::SDL2 SDL2::SDL2main SDL2_ttf::SDL2_ttf-static)

target_include_directories(GBEmulator PUBLIC ${sdl2_SOURCE_DIR}/include)
#target_include_directories(GBEmulator PUBLIC ${sdl2_ttf_SOURCE_DIR}/include)


target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/emulator/")
target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/log/")

target_precompile_headers(GBEmulator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/pch.h)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GBEmulator PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
