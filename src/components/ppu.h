//
// Created by Jack_ on 31/08/2025.
//

#ifndef GAMEBOYEMULATOR_PPU_H
#define GAMEBOYEMULATOR_PPU_H
#include "../interfaces/IComponentMessanger.h"
#include <array>

struct oam_entry
{
	uint8_t y;
	uint8_t x;
	uint8_t tile;
	/*
	*	bits
	*		7			6		5		 4		  3		 2	1 0
	*	Priority	Y flip	X flip	DMG palette	Bank	CGB palette
	*
	*	Priority: 0 = No, 1 = BG and Window color indices 1–3 are drawn over this OBJ
	*	Y flip: 0 = Normal, 1 = Entire OBJ is vertically mirrored
	*	X flip: 0 = Normal, 1 = Entire OBJ is horizontally mirrored
	*	DMG palette [Non CGB Mode only]: 0 = OBP0, 1 = OBP1
	*	Bank [CGB Mode Only]: 0 = Fetch tile from VRAM bank 0, 1 = Fetch tile from VRAM bank 1
	*	CGB palette [CGB Mode Only]: Which of OBP0–7 to use
	*/
	uint8_t flags;
};

enum memoryType
{
	oam,
	vram
};

class ppu : public memoryComponentMessanger
{
public:
	// Read a byte from a device-local address (offset from base).
	uint8_t read(uint16_t address) override;

	// Write a byte to a device-local address (offset from base).
	void write(uint16_t address, uint8_t value) override;

private:
	std::array<uint8_t, 0x2000> vram = {};    // 0x8000-0x9FFF
	std::array<oam_entry, 40> oamRAM = {};     // 0xFE00-0xFE9F (40 * 4 bytes)

};


#endif //GAMEBOYEMULATOR_PPU_H