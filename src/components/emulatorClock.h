//
// Created by Jack_ on 24/09/2025.
//

#ifndef GAMEBOYEMULATOR_EMULATORCLOCK_H
#define GAMEBOYEMULATOR_EMULATORCLOCK_H
#include "ppu.h"
#include "timer.h"
#include "../interfaces/IClock.h"

class emulatorClock : public IClock
{
public:
	emulatorClock() = default;
	~emulatorClock() override = default;

	emulatorClock(std::shared_ptr<timer> t/*, std::shared_ptr<ppu> p*/)
	: m_timer(t)/*, m_ppu(p)*/ {}

	void init(std::shared_ptr<timer> t);

	void cycles(std::uint64_t cpuCycles) override;

	uint64_t getTicks() const { return m_ticks; }
private:
	std::weak_ptr<timer> m_timer;
	//std::weak_ptr<ppu>   m_ppu;
	std::uint64_t        m_ticks{0};
};


#endif //GAMEBOYEMULATOR_EMULATORCLOCK_H