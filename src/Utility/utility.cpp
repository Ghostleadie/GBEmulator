#include "utility.h"


std::string utility::uint8ToHex(const uint8_t i) {
    return std::format("{:#X}", static_cast<int>(i));
}

bool utility::inRange(uint16_t address, uint16_t low, uint16_t high)
{
    return low <= address && address <= high;
}

bool utility::isReg16Bit(registryType rt)
{
    return rt >= RT_AF;
}

uint8_t utility::setBit(const uint8_t value, const uint8_t bit)
{
    auto value_set = value | (1 << bit);
    return static_cast<uint8_t>(value_set);
}

uint8_t utility::clearBit(const uint8_t value, const uint8_t bit)
{
    auto value_cleared = value & ~(1 << bit);
    return static_cast<uint8_t>(value_cleared);
}

uint8_t utility::setBitTo(const uint8_t value, const uint8_t bit, bool bit_on)
{
    return bit_on ? utility::setBit(value, bit) : utility::clearBit(value, bit);
}

bool utility::checkBit(const uint8_t value, const uint8_t bit)
{
    return (value & (1 << bit)) ? true : false;
}
