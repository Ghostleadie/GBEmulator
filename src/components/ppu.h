//
// Created by Jack_ on 31/08/2025.
//

#ifndef GAMEBOYEMULATOR_PPU_H
#define GAMEBOYEMULATOR_PPU_H
#include "../interfaces/IComponentMessanger.h"
#include "../interfaces/IClocked.h"
#include <array>
#include <queue>
#include "../interfaces/IInterruptSink.h"

struct oam_dma
{
	bool active;
	uint8_t byte;
	uint8_t value;
	uint8_t start_delay;
};

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

struct fifo_entry
{
	fifo_entry* next;
	uint32_t value; // color value
};

struct pixelfifo
{
	fifo_entry* head;
	fifo_entry* tail;
	uint32_t size;
};

struct lcd {
	uint8_t lcdc = 0x91;								// 0xFF40  control bits (BG/OBJ/window enable, tile bases…)
	uint8_t stat = 0x85;								// 0xFF41  mode (0-3) + LYC=LY flag + STAT-IRQ enables
	uint8_t scrollY  = 0;								// 0xFF42
	uint8_t scrollX  = 0;								// 0xFF43
	uint8_t ly   = 0;									// 0xFF44  current scanline (PPU-driven; read-only to CPU)
	uint8_t lyCompare  = 0;								// 0xFF45
	uint8_t dma = 0 ;									// 0xFF46
	uint8_t bgPalette  = 0xFC;							// 0xFF47
	std::array<uint8_t, 2> objPallette = {0xFF, 0xFF};	// 0xFF48 & 0xFF49
	uint8_t winY   = 0;									// 0xFF4A
	uint8_t winX   = 0;									// 0xFF4B

	std::array<uint32_t,4> bgColours = {};
	std::array<uint32_t,4> Sprite1Colours = {};
	std::array<uint32_t,4> Sprite2Colours = {};
};

enum LCD_Mode
{
	MODE_HBLANK,
	MODE_VBLANK,
	MODE_OAM,
	MODE_XFER
};

enum StatFlags
{
	SS_HBLANK = (1 << 3),
	SS_VBLANK = (1 << 4),
	SS_OAM = (1 << 5),
	SS_LYC = (1 << 6),
};

enum FIFOState
{
	FSTILE,
	FSDATA0,
	FSDATA1,
	FSSLEEP,
	FSPUSH
};

class ppu : public IComponentMessanger, public IClocked
{
public:
	ppu(const std::shared_ptr<IInterruptSink>& interruptSink);

	void init();

	void connectBus(IComponentMessanger* bus) {m_bus = bus;}

	// ICompMessager start
	// Read a byte from a device-local address (offset from base).
	uint8_t read(uint16_t address) override;

	// Write a byte to a device-local address (offset from base).
	void write(uint16_t address, uint8_t value) override;
	// ICompMessager End

	// IClocked start
	void tick() override;
	// IClocked end

	//LCD
	void initLCD();
	void updatePalette(uint8_t paletteData, uint8_t pal);

	LCD_Mode lcdMode() const;

	//LCD End

	//pixel pipeline start

	void pushPixel();

	void process();

	bool add();

	void fetch();
	//pixel pipeline end

	//LCD States
	void OAMMode();
	void XFERMode();
	void VBLANKMode();
	void HBLANKMode();

	void increment_ly();

	void setLcdMode(LCD_Mode m);

	void startDMA(uint8_t start);
	bool isTransferringDMA() const {return dma.active;}
private:
	void tickDMA();

	std::array<uint8_t, 0x2000> vram = {};    // 0x8000-0x9FFF
	std::array<oam_entry, 40> oamRAM = {};     // 0xFE00-0xFE9F (40 * 4 bytes)

	uint32_t currentFrame = 0;
	std::array<uint32_t, 160*144> buffer = {};

	std::weak_ptr<IInterruptSink> m_interruptSink;

	lcd lcdData = {};

	IComponentMessanger* m_bus = nullptr;

	static constexpr std::array<uint32_t, 4> colours_default = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

	oam_dma dma = {};
	uint8_t m_dmaDots = 0;     // T-cycle divider so DMA steps once per M-cycle
	uint16_t m_lineTick = 0;   // T-cycle counter within the current scanline (0-455)

	static constexpr int LinesPerFrame = 154;
	static constexpr int TicksPerLine = 456;
	static constexpr int YRes = 144;
	static constexpr int XRes = 160;

	static constexpr uint32_t TargetFPS = 60;
	uint32_t lastFrame = 0;
	uint32_t startTimer = 0;
	uint32_t frameCounter = 0;

	std::queue<uint32_t> fifo;
	FIFOState state;
	uint8_t lineX{};
	uint8_t pushedX{};
	uint8_t fetchX{};
	std::array<uint8_t,3> bgwFetchData = {};
	std::array<uint8_t,6> fetchEntryData = {};
	uint8_t mapX{};
	uint8_t mapY{};
	uint8_t tileY{};
	uint8_t fifoX{};
};


#endif //GAMEBOYEMULATOR_PPU_H