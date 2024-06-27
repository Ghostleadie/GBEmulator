#pragma once

class memory
{
public:
	uint8_t read(uint16_t address);
	void write(uint16_t address, uint8_t value);
	

private:
	std::array <uint8_t, 0x2000> wRAM;
	std::array<uint8_t, 0x80> hRAM;
};