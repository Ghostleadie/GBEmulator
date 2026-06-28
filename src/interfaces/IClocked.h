//
// Created by Jack_ on 28/06/2026.
//

#ifndef GAMEBOYEMULATOR_ICLOCKED_H
#define GAMEBOYEMULATOR_ICLOCKED_H

// A device driven by the master (T-cycle / dot) clock. The emulatorClock ticks
// every registered device once per T-cycle; each device decides what one tick
// means to it (e.g. the timer advances DIV every tick; the PPU steps OAM DMA
// every 4th tick = once per M-cycle).
class IClocked
{
public:
	virtual ~IClocked() = default;
	virtual void tick() = 0; // advance exactly one T-cycle
};

#endif //GAMEBOYEMULATOR_ICLOCKED_H
