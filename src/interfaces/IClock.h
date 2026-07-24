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
    /** Advances the clock by the given number of elapsed CPU cycles.
     * @param cpuCycles number of CPU cycles that have elapsed since the previous call.
     */
    virtual void cycles(std::uint64_t cpuCycles) = 0;
};


#endif //GAMEBOYEMULATOR_ICLOCK_H