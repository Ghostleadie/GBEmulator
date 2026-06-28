//
// Created by Jack_ on 24/09/2025.
//

#include "emulatorClock.h"

void emulatorClock::cycles(const std::uint64_t cpuCycles)
{
	// The master clock runs at the T-cycle (dot) rate: 4 T-cycles per M-cycle.
	const std::uint64_t tcycles = cpuCycles * 4;
	for (std::uint64_t i = 0; i < tcycles; ++i)
	{
		++m_ticks;
		for (IClocked* device : m_devices)
		{
			device->tick();
		}
	}
}
