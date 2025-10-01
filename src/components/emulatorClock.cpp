//
// Created by Jack_ on 24/09/2025.
//

#include "emulatorClock.h"

void emulatorClock::init(std::shared_ptr<timer> t)
{
	m_timer = t;
}

void emulatorClock::cycles(const std::uint64_t cpuCycles)
{
	const std::uint64_t n = cpuCycles * 4;
	for (std::uint64_t i = 0; i < n; ++i) {
		++m_ticks;
		if (!m_timer.expired())
		{
			m_timer.lock()->tick();
		}
	}
}