﻿# CMakeList.txt : CMake project for GBEmulator, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (GBEmulator "emulator/GBEmulator.cpp" "emulator/GBEmulator.h" "log/Log.cpp" "log/Log.h")

#find_package(SDL2 REQUIRED)

target_link_libraries(GBEmulator PUBLIC glfw imgui spdlog SDL2::SDL2 SDL2::SDL2main)

target_include_directories(GBEmulator PUBLIC ${sdl2_SOURCE_DIR}/include)

target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/emulator/")
target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/log/")

target_precompile_headers(GBEmulator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/pch.h)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GBEmulator PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.