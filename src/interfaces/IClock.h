//
// Created by Jack_ on 24/09/2025.
//

#ifndef GAMEBOYEMULATOR_ICLOCK_H
#define GAMEBOYEMULATOR_ICLOCK_H
#include <cstdint>


class IClock
{
public:
    virtual ~IClock() = default;
    virtual void cycles(std::uint64_t cpuCycles) = 0;
};


#endif //GAMEBOYEMULATOR_ICLOCK_H