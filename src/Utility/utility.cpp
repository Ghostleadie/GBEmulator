#include "utility.h"

std::string utility::uint8ToHex(const uint8_t i) {
    return std::format("{:X}", static_cast<int>(i));
}

bool utility::inRange(uint16_t address, uint16_t low, uint16_t high)
{
    return low <= address && address <= high;
}
