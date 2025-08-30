
# Main application executable
add_executable(GBEmulator main.cpp)

target_link_libraries(GBEmulator PRIVATE
    imgui
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
    spdlog::spdlog
    nfd
)