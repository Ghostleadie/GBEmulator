//
// Created by Jack_ on 31/08/2025.
//

#ifndef GAMEBOYEMULATOR_PPU_H
#define GAMEBOYEMULATOR_PPU_H
#include "../interfaces/IComponentMessanger.h"
#include "../interfaces/IClocked.h"
#include <array>
#include <queue>
#include <forward_list>
#include <mutex>
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

enum class LCD_Mode : uint8_t
{
	MODE_HBLANK,
	MODE_VBLANK,
	MODE_OAM,
	MODE_XFER
};

enum class StatFlags : uint8_t
{
	SS_HBLANK = (1 << 3),
	SS_VBLANK = (1 << 4),
	SS_OAM = (1 << 5),
	SS_LYC = (1 << 6),
};

enum class FIFOState
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
	/**
	 * Constructs the PPU and records the interrupt sink used to raise VBlank and
	 * LCD STAT interrupts.
	 * @param interruptSink Sink the PPU raises VBlank/STAT interrupts through.
	 */
	ppu(const std::shared_ptr<IInterruptSink>& interruptSink);

	/**
	 * Resets the PPU to power-on state: clears the work and present buffers,
	 * pipeline counters and window line, initialises the LCD palettes, and
	 * enters OAM-scan mode with cleared OAM.
	 */
	void init();

	/** Stores the bus pointer used as the source for OAM DMA reads. @param bus Bus messenger the DMA copies transfer bytes from. */
	void connectBus(IComponentMessanger* bus) {m_bus = bus;}

	// ICompMessager start
	/**
	 * Reads a byte from a PPU-mapped address: VRAM (0x8000-0x9FFF), OAM
	 * (0xFE00-0xFE9F) or an LCD register (0xFF40-0xFF4B).
	 * @param address Raw CPU address to read.
	 * @return The stored byte, or 0xFF if the address is not PPU-mapped.
	 */
	uint8_t read(uint16_t address) override;

	/**
	 * Writes a byte to a PPU-mapped address: VRAM, OAM or an LCD register.
	 * LY (0xFF44) is read-only and ignored; writing the DMA register (0xFF46)
	 * starts an OAM DMA transfer; writing a palette register (0xFF47-0xFF49)
	 * refreshes the cached colour table.
	 * @param address Raw CPU address to write.
	 * @param value Byte to store.
	 */
	void write(uint16_t address, uint8_t value) override;
	// ICompMessager End

	// IClocked start
	/**
	 * Advances the PPU by one T-cycle (dot): steps OAM DMA every four dots and,
	 * while the LCD is enabled, drives the current mode (OAM scan, drawing,
	 * HBlank or VBlank). Holds LY and the line counter at 0 while the LCD is off.
	 */
	void tick() override;
	// IClocked end

	//LCD
	/** Initialises the background and both object palette tables to the default DMG shades. */
	void initLCD();

	/**
	 * Rebuilds a cached colour table from a DMG palette-register byte, mapping
	 * each of the four 2-bit indices to its default shade.
	 * @param paletteData Palette-register value, 2 bits per colour index.
	 * @param pal Which table to update: 0 = background, 1 = OBP0, 2 = OBP1.
	 */
	void updatePalette(uint8_t paletteData, uint8_t pal);

	/** @return The current PPU mode from STAT bits 0-1 (HBlank, VBlank, OAM scan or drawing). */
	LCD_Mode lcdMode() const;

	//LCD End

	//pixel pipeline start

	/**
	 * Pops one pixel from the FIFO and writes it to the frame buffer once the
	 * FIFO has primed (more than 8 entries). Discards the first scrollX % 8
	 * pixels of the line to apply the fine horizontal scroll.
	 */
	void pushPixel();

	/**
	 * Runs one dot of the drawing pipeline: recomputes the background-map
	 * coordinates for the current fetch position, advances the tile fetcher on
	 * even dots, and pushes a pixel to the LCD.
	 */
	void process();

	/**
	 * Decodes the fetched tile's two data bytes into eight background pixels,
	 * overlays any sprite pixels, and pushes the resolved colours onto the FIFO.
	 * @return False if the FIFO is still full and cannot accept the tile yet; true once the pixels are queued.
	 */
	bool add();

	/**
	 * Runs one step of the background/window/sprite tile-fetcher state machine
	 * (tile index, low byte, high byte, sleep, push). Each pass produces the data
	 * for eight pixels that the push step feeds to the FIFO via add().
	 */
	void fetch();

	/**
	 * Per-tile sprite selection, run inside fetch(): picks up to three sprites
	 * from the scanline list that overlap the eight-pixel tile at the current
	 * fetch x and records them in fetched_entries for the data fetch.
	 */
	void loadSpriteTile();

	/**
	 * Reads one tile-data byte for each fetched sprite into fetchEntryData,
	 * honouring the 8x16 sprite height and the OAM Y-flip flag. Sprite tiles are
	 * always taken from the 0x8000 unsigned data area.
	 * @param offset Byte plane to fetch: 0 = low byte, 1 = high byte.
	 */
	void loadSpriteData(uint8_t offset);

	/**
	 * Fetches the window's tile-map index into bgwFetchData[0] when the current
	 * fetch x falls inside the window, replacing the background tile fetch for
	 * window pixels. Runs in fetch()'s tile (FSTILE) state; LCDC bit 6 selects the
	 * window tile map.
	 */
	void loadWindowTile();

	/**
	 * Overlays the sprite pixel at the current FIFO x onto the incoming
	 * background colour. Walks the sprites fetched for this tile; the first
	 * non-transparent pixel that wins the BG-priority test replaces the colour.
	 * @param bit Ignored on entry and overwritten internally; kept to match the pipeline call convention.
	 * @param color Fallback background colour returned when no sprite pixel applies.
	 * @param bgColor Background colour index (0-3) used for the OBJ-to-BG priority check.
	 * @return The resolved ABGR8888 colour: the winning sprite pixel, otherwise color.
	 */
	uint32_t fetchSpritePixels(int bit, uint32_t color, uint8_t bgColor);
	//pixel pipeline end

	/**
	 * Scans the 40 OAM entries at the start of a scanline and builds the list of
	 * up to ten sprites that intersect the current line, inserting them sorted by
	 * X so lower-X sprites take draw priority. Honours the 8x8/8x16 height from
	 * LCDC bit 2.
	 */
	void LoadLineSprites();

	//LCD States
	/**
	 * Handles OAM-scan mode (mode 2): scans the line's sprites on the first dot
	 * and switches to drawing mode once 80 dots have elapsed, resetting the pixel
	 * pipeline counters.
	 */
	void OAMMode();

	/**
	 * Handles pixel-transfer/drawing mode (mode 3): runs the pipeline each dot
	 * until the full 160-pixel line is drawn, then clears the FIFO, enters HBlank
	 * and raises the STAT interrupt if the HBlank source is enabled.
	 */
	void XFERMode();

	/**
	 * Handles VBlank mode (mode 1): advances LY at the end of each of the ten
	 * VBlank scanlines and, once the last line (154) is reached, returns to OAM
	 * scan for the next frame, resetting LY and the window line counter.
	 */
	void VBLANKMode();

	/**
	 * Handles HBlank mode (mode 0): advances LY at the end of the scanline, then
	 * either enters VBlank after the last visible line -- publishing the completed
	 * frame and raising the VBlank interrupt -- or returns to OAM scan for the
	 * next line. Also paces emulation to the target frame rate.
	 */
	void HBLANKMode();

	/**
	 * Advances LY to the next scanline and updates the LYC=LY coincidence flag in
	 * STAT, raising the STAT interrupt when they match and the LYC source is
	 * enabled. Also bumps the internal window line counter while the scanline lies
	 * within the window's vertical span (evaluated before LY is incremented).
	 */
	void increment_ly();

	/** @return True when the window is on-screen: enabled via LCDC bit 5 with WX/WY inside the visible range. */
	bool window_visible() const;

	/**
	 * Sets the PPU mode in STAT bits 0-1, leaving the other STAT bits untouched.
	 * @param m New PPU mode to store.
	 */
	void setLcdMode(LCD_Mode m);

	/**
	 * Begins an OAM DMA transfer from the page starting at start * 0x100 into OAM.
	 * The transfer runs one byte per M-cycle from tick() after a short start delay.
	 * @param start High byte of the source address (the value written to DMA register 0xFF46).
	 */
	void startDMA(uint8_t start);

	/** @return True while an OAM DMA transfer is in progress. */
	bool isTransferringDMA() const {return dma.active;}

	// LCD resolution. Public so the view layer (screen) can size its texture
	// without hardcoding. Mirror XRes/YRes below.
	static constexpr int ScreenWidth  = 160;
	static constexpr int ScreenHeight = 144;

	/**
	 * Copies the most recently completed frame into dst (ScreenWidth *
	 * ScreenHeight uint32_t values, ABGR8888). Thread-safe: the render thread may
	 * call this while the PPU runs on the emulation thread. Reads the present
	 * buffer, which is only swapped at frame end (VBlank), so dst never holds a
	 * half-drawn frame.
	 * @param dst Destination buffer of ScreenWidth*ScreenHeight ABGR8888 pixels.
	 */
	void copyFrame(uint32_t* dst) const;
private:
	/**
	 * Advances an in-progress OAM DMA by one byte, copying from the source page
	 * via the bus into OAM. Called once per M-cycle from tick() after the start
	 * delay, and completes after 0xA0 (160) bytes.
	 */
	void tickDMA();

	std::array<uint8_t, 0x2000> vram = {};    // 0x8000-0x9FFF
	std::array<oam_entry, 40> oamRAM = {};     // 0xFE00-0xFE9F (40 * 4 bytes)

	uint32_t currentFrame = 0;
	std::array<uint32_t, 160*144> buffer = {};          // work buffer, written as the frame renders

	// Double buffer for the render thread. `buffer` is copied here under
	// m_frameMutex the instant a frame completes (entering VBLANK); copyFrame()
	// reads it out under the same lock. Keeps the emulation and render threads off
	// each other's toes without locking on every pixel push.
	std::array<uint32_t, 160*144> m_presentBuffer = {};
	mutable std::mutex m_frameMutex;

	// Sprites overlapping the current scanline, walked front-to-back in draw order.
	// Was a hand-rolled singly-linked list (oam_line_entry.next); std::forward_list is
	// the STL equivalent -- single direction, cheap push_front. (The pixel FIFO that
	// used fifo_entry/pixelfifo is now the std::queue<uint32_t> below.)
	uint8_t line_Sprites_Count;
	std::forward_list<oam_entry> line_Sprites;

	uint8_t fetched_entry_count;
	std::array<oam_entry, 3> fetched_entries = {};

	std::weak_ptr<IInterruptSink> m_interruptSink;

	uint8_t windowLine;

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

	// Real DMG frame timing: 70224 dots per frame at the 4194304 Hz dot clock =
	// 59.727 FPS (16.742 ms), NOT 60. Kept as a double so the pacer can carry the
	// sub-millisecond remainder the integer sleep clock would otherwise drop.
	static constexpr double FrameTimeMs   = 1000.0 * (TicksPerLine * LinesPerFrame) / 4194304.0;
	static constexpr double MaxFrameLagMs = 100.0;   // past this much lag, resync instead of catching up
	double   m_frameDeadline = 0.0;  // wall-clock ms target for the next frame's end
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