//
// Created by Jack_ on 24/09/2025.
//

#ifndef GAMEBOYEMULATOR_EMULATORCLOCK_H
#define GAMEBOYEMULATOR_EMULATORCLOCK_H
#include "../interfaces/IClock.h"
#include "../interfaces/IClocked.h"
#include <vector>
#include <cstdint>

class emulatorClock : public IClock
{
public:
	emulatorClock() = default;
	~emulatorClock() override = default;

	// Register a device to be ticked once per T-cycle. Non-owning: the emulator
	// owns the devices and outlives the clock. Registration order = tick order.
	void addDevice(IClocked* device) { m_devices.push_back(device); }

	void cycles(std::uint64_t cpuCycles) override; // cpuCycles = M-cycles

	uint64_t getTicks() const { return m_ticks; }  // T-cycle count since start
private:
	std::vector<IClocked*> m_devices;
	std::uint64_t          m_ticks{0};
};


#endif //GAMEBOYEMULATOR_EMULATORCLOCK_H
