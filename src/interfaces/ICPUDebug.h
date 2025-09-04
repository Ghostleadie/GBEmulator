//
// Created by Jack_ on 03/09/2025.
//

#ifndef GAMEBOYEMULATOR_ICPUDEBUG_H
#define GAMEBOYEMULATOR_ICPUDEBUG_H
#include <cstdint>

#include "../components/cpu/cpu.h"

class ICPUDebug
{
public:
	virtual ~ICPUDebug() = default;

	virtual uint16_t getPC() const = 0;
	virtual uint16_t getSP() const = 0;
	virtual const registers& getRegisters() const = 0;

	// Peek without advancing PC
	virtual uint8_t peekMemory(uint16_t addr) const = 0;
};


#endif //GAMEBOYEMULATOR_ICPUDEBUG_H