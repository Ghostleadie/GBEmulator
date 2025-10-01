//
// Created by Jack_ on 01/10/2025.
//

#ifndef GAMEBOYEMULATOR_TESTMEMORY_H
#define GAMEBOYEMULATOR_TESTMEMORY_H

#include <array>
#include <string>
#include <cstdint>
#include <functional>
#include "../src/components/base/component.h"

class testMemory : public memorycomponentMessanger
{
private:
	std::array<uint8_t, 0x10000> MEM;

public:
	testMemory() : memorycomponentMessanger() {
		MEM.fill(0);
	}

	// Override the pure virtual methods from memoryComponent
	virtual uint8_t read(uint16_t address) override {
		return MEM[address];
	}

	virtual void write(uint16_t address, uint8_t value) override {
		MEM[address] = value;
	}

	// Add read16 method if your CPU needs it
	uint16_t read16(const uint16_t addr) const {
		return MEM[addr] | (MEM[addr + 1] << 8);
	}

	void reset() {
		MEM.fill(0);
	}
};


#endif //GAMEBOYEMULATOR_TESTMEMORY_H