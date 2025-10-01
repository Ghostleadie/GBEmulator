//
// Created by Jack_ on 01/10/2025.
//

#pragma once

#include <array>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>

#define LOG_ERROR(...)
#define LOG_WARN(...)
#define LOG_INFO(...)
#define LOG_TRACE(...)
#define LOG_DEBUG(...)
#define CARTRIDGE_INFO(...)

// Redirect emulator::cycles to our mock
#define emulator emulator_mock