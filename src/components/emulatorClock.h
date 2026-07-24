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

	/**
	 * Registers a device to be ticked once per T-cycle.
	 * @param device Non-owning pointer; the emulator owns the devices and outlives the clock. Registration order sets tick order.
	 */
	void addDevice(IClocked* device) { m_devices.push_back(device); }

	/**
	 * Runs the master clock forward, ticking every registered device on each T-cycle.
	 * @param cpuCycles Number of M-cycles to advance; each expands to 4 T-cycles (dots), so devices tick four times per M-cycle.
	 */
	void cycles(std::uint64_t cpuCycles) override;

	/** Returns the number of T-cycles ticked since start. */
	uint64_t getTicks() const { return m_ticks; }
private:
	std::vector<IClocked*> m_devices;
	std::uint64_t          m_ticks{0};
};


#endif //GAMEBOYEMULATOR_EMULATORCLOCK_H
