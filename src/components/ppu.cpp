//
// Created by Jack_ on 31/08/2025.
//

#include "ppu.h"

#include <cstring>

#include "../Utility/utility.h"

// Scoped mode/FIFO enums; using enum keeps the switch labels and assignments
// below unqualified. StatFlags stays qualified since it is mixed into the STAT
// byte via static_cast.
using enum LCD_Mode;
using enum FIFOState;

void ppu::init()
{
	currentFrame = 0;
	m_lineTick = 0;
	buffer = std::array<uint32_t, 160*144>{0};
	m_presentBuffer = std::array<uint32_t, 160*144>{0};

	lineX = 0;
	pushedX = 0;
	fetchX = 0;
	state = FSTILE;

	windowLine = 0;

	initLCD();
	lcdData.stat &= ~0b11;
	lcdData.stat |= static_cast<uint8_t>(MODE_OAM);
	oamRAM = std::array<oam_entry, 40>{0};
}

ppu::ppu(const std::shared_ptr<IInterruptSink>& interruptSink)
{
	m_interruptSink = interruptSink;
}

uint8_t ppu::read(uint16_t address)
{
	if (address >= 0x8000 && address <= 0x9FFF)
	{
		return vram[address - 0x8000];
	}
	if (address >= 0xFE00 && address <= 0xFE9F)
	{
		return reinterpret_cast<const uint8_t*>(oamRAM.data())[address - 0xFE00];
	}
	if (address >= 0xFF40 && address <= 0xFF4B)
	{
		switch (address)
		{
			case 0xFF40: return lcdData.lcdc;
			case 0xFF41: return lcdData.stat;
			case 0xFF42: return lcdData.scrollY;
			case 0xFF43: return lcdData.scrollX;
			case 0xFF44: return lcdData.ly;
			case 0xFF45: return lcdData.lyCompare;
			case 0xFF46: return lcdData.dma;
			case 0xFF47: return lcdData.bgPalette;
			case 0xFF48: return lcdData.objPallette[0];
			case 0xFF49: return lcdData.objPallette[1];
			case 0xFF4A: return lcdData.winY;
			case 0xFF4B: return lcdData.winX;
			default:     return 0xFF;
		}
	}
	return 0xFF;
}

void ppu::write(uint16_t address, uint8_t value)
{
	if (address >= 0x8000 && address <= 0x9FFF)
	{
		vram[address - 0x8000] = value;
		return;
	}
	if (address >= 0xFE00 && address <= 0xFE9F)
	{
		reinterpret_cast<uint8_t*>(oamRAM.data())[address - 0xFE00] = value;
		return;
	}
	if (address >= 0xFF40 && address <= 0xFF4B)
	{
		switch (address)
		{
			case 0xFF40:
			{
				lcdData.lcdc = value;
				break;
			}
			case 0xFF41:
			{
				lcdData.stat = value;
				break;
			}   // refine later: mask bits 0-2 (read-only)
			case 0xFF42:
			{
				lcdData.scrollY = value;
				break;
			}
			case 0xFF43:
			{
				lcdData.scrollX = value;
				break;
			}
			case 0xFF44:
			{
				/* LY is read-only - ignore CPU writes */
				break;
			}
			case 0xFF45:
			{
				lcdData.lyCompare = value;
				break;
			}
			case 0xFF46:
			{
				lcdData.dma = value;
				startDMA(value);
				break;
			}       // OAM DMA
			case 0xFF47:
			{
				lcdData.bgPalette = value;
				updatePalette(value, 0);
				break;
			}
			case 0xFF48:
			{
				lcdData.objPallette[0] = value;
				updatePalette(value & 0xFC, 1);
				break;
			}
			case 0xFF49:
			{
				lcdData.objPallette[1] = value;
				updatePalette(value & 0xFC, 2);
				break;
			}
			case 0xFF4A:
			{
				lcdData.winY = value;
				break;
			}
			case 0xFF4B:
			{
				lcdData.winX = value;
				break;
			}
			default: break;
		}

	}
}

void ppu::tick()
{
	if (++m_dmaDots == 4)
	{
		m_dmaDots = 0;
		tickDMA();
	}

	if (!(lcdData.lcdc & 0x80))
	{            // LCD off: hold reset, do nothing
		lcdData.ly = 0;
		m_lineTick = 0;
		setLcdMode(MODE_HBLANK);
		return;
	}

	m_lineTick++;                          // one dot per T-cycle
	switch (lcdMode())
	{
		case MODE_OAM:
		{
			OAMMode();
			break;
		}
		case MODE_XFER:
		{
			XFERMode();
			break;
		}
		case MODE_HBLANK:
		{
			HBLANKMode();
			break;
		}
		case MODE_VBLANK:
		{
			VBLANKMode();
			break;
		}
	}
}


void ppu::initLCD()
{
	for (int i = 0; i < 4; i++)
	{
		lcdData.bgColours[i] = colours_default[i];
		lcdData.Sprite1Colours[i] = colours_default[i];
		lcdData.Sprite2Colours[i] = colours_default[i];
	}
}

void ppu::updatePalette(uint8_t paletteData, uint8_t pal)
{
	std::array<uint32_t, 4>* colours = &lcdData.bgColours;
	if (pal == 1)
	{
		colours = &lcdData.Sprite1Colours;
	}
	else if (pal == 2)
	{
		colours = &lcdData.Sprite2Colours;
	}

	(*colours)[0] = colours_default[ paletteData       & 0b11];
	(*colours)[1] = colours_default[(paletteData >> 2) & 0b11];
	(*colours)[2] = colours_default[(paletteData >> 4) & 0b11];
	(*colours)[3] = colours_default[(paletteData >> 6) & 0b11];
}

LCD_Mode ppu::lcdMode() const
{
	return static_cast<LCD_Mode>(lcdData.stat & 0b11);
}

void ppu::pushPixel()
{
	if (fifo.size() > 8)
	{
		uint32_t pixeldata = fifo.front();
		fifo.pop();
		if (lineX >= (lcdData.scrollX % 8))
		{
			buffer[pushedX + (lcdData.ly * XRes)] = pixeldata;
			pushedX++;
		}

		lineX++;
	}
}

void ppu::process()
{
	mapY = lcdData.ly + lcdData.scrollY;
	mapX = fetchX + lcdData.scrollX;
	tileY = ((lcdData.ly + lcdData.scrollY) % 8) * 2;

	if (!(m_lineTick & 1))
	{
		fetch();
	}

	pushPixel();
}

bool ppu::add() {
	if (fifo.size() > 8)
	{
		return false;
	}

	int x = fetchX - (8 - (lcdData.scrollX % 8));

	for (int i = 0; i < 8; i++) {
		int bit = 7 - i;
		uint8_t high = utility::checkBit(bgwFetchData[1], bit);
		uint8_t low = utility::checkBit(bgwFetchData[2], bit) << 1;
		uint8_t bgColorIndex = high | low;

		// Resolve the 2-bit index to a real ABGR8888 color up front so the whole
		// pipeline works in color space -- fetchSpritePixels returns resolved colors,
		// so mixing index and color here would index bgColours out of bounds.
		uint32_t color = lcdData.bgColours[bgColorIndex];

		if (!utility::checkBit(lcdData.lcdc, 0))
		{
			color = lcdData.bgColours[0];
		}

		if (utility::checkBit(lcdData.lcdc, 1))
		{
			color = fetchSpritePixels(bit, color, bgColorIndex);
		}

		if (x >= 0) {
			fifo.push(color);
			fifoX++;    // FIFO x advances only for pixels actually queued; fetchSpritePixels reads it
		}
	}

	return true;
}

void ppu::loadSpriteTile()
{
	for (auto it = line_Sprites.begin(); it != line_Sprites.end(); ++it)
	{
		int spriteX = (it->x - 8) + (lcdData.scrollX % 8);

		// Does this sprite overlap the 8-pixel tile currently being fetched?
		if ((spriteX >= fetchX && spriteX < fetchX + 8) ||
			((spriteX + 8) >= fetchX && (spriteX + 8) < fetchX + 8))
		{
			fetched_entries[fetched_entry_count++] = *it;
		}

		// hardware fetches at most 3 sprites per pixel
		if (fetched_entry_count >= 3)
		{
			break;
		}
	}
}

void ppu::loadSpriteData(uint8_t offset)
{
	int currentY = lcdData.ly;
	uint8_t spriteHeight = utility::checkBit(lcdData.lcdc, 2) ? 16 : 8;

	for (int i = 0; i < fetched_entry_count; i++)
	{
		uint8_t ty = ((currentY + 16) - fetched_entries[i].y) * 2;

		// flags bit 6 = Y flip: mirror the row within the sprite's height
		if (utility::checkBit(fetched_entries[i].flags, 6))
		{
			ty = ((spriteHeight * 2) - 2) - ty;
		}

		uint8_t tileIndex = fetched_entries[i].tile;

		if (spriteHeight == 16)
		{
			tileIndex &= ~1;    // 8x16 sprites ignore the low bit; tiles come in pairs
		}

		// Sprite tile data is always in the 0x8000 (unsigned) area.
		fetchEntryData[(i * 2) + offset] = read(0x8000 + (tileIndex * 16) + ty + offset);
	}
}

void ppu::loadWindowTile()
{
	if (!window_visible())
	{
		return;
	}

	uint8_t window_y = lcdData.winY;

	if (fetchX + 7 >= lcdData.winX &&
		fetchX + 7 < lcdData.winX + YRes + 14)
	{
		if (lcdData.ly >= window_y && lcdData.ly < window_y + XRes)
		{
			uint8_t wTileY = windowLine / 8;

			// LCDC bit 6 selects the window tile map area.
			const uint16_t winMap = utility::checkBit(lcdData.lcdc, 6) ? 0x9C00 : 0x9800;
			bgwFetchData[0] = read(winMap +
				((fetchX + 7 - lcdData.winX) / 8) +
				(wTileY * 32));

			// 0x8800 addressing treats the index as signed; bias by 128 (bit 4 clear).
			if (!utility::checkBit(lcdData.lcdc, 4))
			{
				bgwFetchData[0] += 128;
			}
		}
	}
}

void ppu::fetch()
{
	switch (state)
	{
		case FSTILE: {
			fetched_entry_count = 0;

			if (utility::checkBit(lcdData.lcdc, 0))
			{
				// LCDC bit 3 selects the BG tile map, not the tile data area.
				const uint16_t mapBase = utility::checkBit(lcdData.lcdc, 3) ? 0x9C00 : 0x9800;
				bgwFetchData[0] = read(mapBase + (mapX / 8) + ((mapY / 8) * 32));

				// 0x8800 addressing treats the index as signed; biasing it by 128
				// makes 0x8800 + index behave like 0x9000 + (int8_t)index.
				if (!utility::checkBit(lcdData.lcdc, 4))
				{
					bgwFetchData[0] += 128;
				}

				loadWindowTile();
			}

			// LCDC bit 1 = OBJ enable; only scan if sprites landed on this line.
			if (utility::checkBit(lcdData.lcdc, 1) && !line_Sprites.empty())
			{
				loadSpriteTile();
			}

			state = FSDATA0;
			fetchX += 8;
		}
		break;
		case FSDATA0: {
			// LCDC bit 4 selects the tile data area: 0x8000 unsigned, 0x8800 signed.
			const uint16_t dataBase = utility::checkBit(lcdData.lcdc, 4) ? 0x8000 : 0x8800;
			bgwFetchData[1] = read(dataBase + (bgwFetchData[0] * 16) + tileY);

			loadSpriteData(0);

			state = FSDATA1;
		}
		break;
		case FSDATA1: {
			const uint16_t dataBase = utility::checkBit(lcdData.lcdc, 4) ? 0x8000 : 0x8800;
			bgwFetchData[2] = read(dataBase + (bgwFetchData[0] * 16) + tileY + 1);

			loadSpriteData(1);

			state = FSSLEEP;

		}
		break;
		case FSSLEEP: {
			state = FSPUSH;
		}
		break;
		case FSPUSH: {
			if (add())
			{
				state = FSTILE;
			}
			else
			{
				state = FSPUSH;
			}
			//pushPixel();

		}
		break;
	}

}

uint32_t ppu::fetchSpritePixels(int bit, uint32_t color, uint8_t bgColor)
{
	for (int i = 0; i < fetched_entry_count; i++)
	{
		// Sprite x in FIFO space: OAM x is +8 biased, scroll fine-adjust matches BG.
		int spriteX = (fetched_entries[i].x - 8) + (lcdData.scrollX % 8);

		if (spriteX + 8 < fifoX)
		{
			// past the pixel point already
			continue;
		}

		int offset = fifoX - spriteX;

		if (offset < 0 || offset > 7)
		{
			// out of bounds
			continue;
		}

		bit = 7 - offset;

		// flags bit 5 = X flip
		if (utility::checkBit(fetched_entries[i].flags, 5))
		{
			bit = offset;
		}

		uint8_t high = utility::checkBit(fetchEntryData[i * 2], bit);
		uint8_t low = utility::checkBit(fetchEntryData[(i * 2) + 1], bit) << 1;

		// flags bit 7 = priority (BG/window colors 1-3 drawn over this OBJ)
		bool bgPriority = utility::checkBit(fetched_entries[i].flags, 7);

		if (!(high | low))
		{
			// transparent sprite pixel, BG shows through
			continue;
		}

		if (!bgPriority || bgColor == 0)
		{
			// flags bit 4 = DMG palette: OBP0 (Sprite1) vs OBP1 (Sprite2)
			color = utility::checkBit(fetched_entries[i].flags, 4)
				? lcdData.Sprite2Colours[high | low]
				: lcdData.Sprite1Colours[high | low];

			if (high | low)
			{
				break;
			}
		}
	}

	return color;
}

void ppu::LoadLineSprites()
{
	int currentY = lcdData.ly;

	uint8_t spriteHeight = utility::checkBit(lcdData.lcdc, 2) ? 16 : 8;

	for (int i = 0; i<40; i++)
	{
		oam_entry e = oamRAM[i];
		if (!e.x)
		{
			continue;
		}

		if (line_Sprites_Count >= 10)
		{
			break;
		}

		if (e.y <= currentY + 16 && e.y + spriteHeight > currentY + 16)
		{
			line_Sprites_Count++;

			auto prev = line_Sprites.before_begin();
			for (auto it = line_Sprites.begin(); it != line_Sprites.end(); ++it)
			{
				if (it->x > e.x)
				{
					break;
				}
				prev = it;
			}
			line_Sprites.insert_after(prev, e);
		}
	}
}

void ppu::OAMMode()
{
	if (m_lineTick >= 80)
	{
		setLcdMode(MODE_XFER);

		state = FSTILE;
		lineX = 0;
		fetchX = 0;
		pushedX = 0;
		fifoX = 0;
	}

	if (m_lineTick == 1)
	{
		line_Sprites.clear();
		line_Sprites_Count = 0;

		LoadLineSprites();
	}
}

void ppu::XFERMode()
{
	process();
	if (pushedX >= XRes)
	{
		utility::ClearQueue( fifo);
		setLcdMode(MODE_HBLANK);
		if (lcdData.stat & static_cast<uint8_t>(StatFlags::SS_HBLANK))
		{
			if (auto s = m_interruptSink.lock())
			{
				s->raise(interruptTypes::INT_LCD_STAT);
			}
		}
	}
}

bool ppu::window_visible() const
{
	return utility::checkBit(lcdData.lcdc, 5) &&
		lcdData.winX <= 166 &&
		lcdData.winY < YRes;
}

void ppu::increment_ly()
{
	// Advance the window's internal line counter only while the current scanline
	// falls inside the window's vertical span. Must run before ly++ (uses old ly).
	if (window_visible() &&
		lcdData.ly >= lcdData.winY &&
		lcdData.ly < lcdData.winY + YRes)
	{
		windowLine++;
	}

	lcdData.ly++;
	if (lcdData.ly == lcdData.lyCompare)
	{
		utility::setBitTo(lcdData.stat,2,1);
		if (lcdData.stat & static_cast<uint8_t>(StatFlags::SS_LYC))
		{
			if (auto s = m_interruptSink.lock())
			{
				s->raise(interruptTypes::INT_LCD_STAT);
			}
		}
	}
	else
	{
		utility::setBitTo(lcdData.stat,2,0);
	}
}

void ppu::VBLANKMode()
{
	if (m_lineTick >= TicksPerLine)
	{
		increment_ly();
		if (lcdData.ly >= LinesPerFrame)
		{
			setLcdMode(MODE_OAM);
			lcdData.ly = 0;
			windowLine = 0;
		}

		m_lineTick = 0;
	}
}

void ppu::HBLANKMode()
{
	if (m_lineTick >= TicksPerLine)
	{
		increment_ly();
		if (lcdData.ly >= YRes)
		{
			setLcdMode(MODE_VBLANK);

			// Frame complete: publish it for the render thread. Swap under the
			// lock at the frame boundary so screen never sees a torn frame.
			{
				std::lock_guard<std::mutex> lock(m_frameMutex);
				m_presentBuffer = buffer;
			}

			if (auto s = m_interruptSink.lock())
			{
				s->raise(interruptTypes::INT_VBLANK);

				if (lcdData.stat & static_cast<uint8_t>(StatFlags::SS_LYC))
				{
					s->raise(interruptTypes::INT_LCD_STAT);
				}
			}

			currentFrame++;

			// Pace to the true DMG frame rate. The deadline advances by the exact
			// (fractional) frame time each frame; sleeping only the whole-millisecond
			// part still averages out to 16.742 ms because the remainder stays in the
			// double. The old code compared against a 60 that was treated as
			// milliseconds, capping the emulator at ~16.6 FPS.
			const double now = static_cast<double>(utility::GetTicks());
			if (m_frameDeadline == 0.0)
			{
				m_frameDeadline = now;   // seed on the first frame
			}
			m_frameDeadline += FrameTimeMs;

			if (now < m_frameDeadline)
			{
				utility::Delay(static_cast<uint32_t>(m_frameDeadline - now));
			}
			else if (now - m_frameDeadline > MaxFrameLagMs)
			{
				// Fell far behind real time (debugger, a stall, host too slow):
				// resync rather than sprint through frames to catch up, which would
				// fast-forward both the game and its audio.
				m_frameDeadline = now;
			}

			// Rough FPS tally per wall-clock second (debug aid).
			if (now - static_cast<double>(startTimer) >= 1000.0)
			{
				startTimer = static_cast<uint32_t>(now);
				frameCounter = 0;
			}
			frameCounter++;
		}
		else
		{
			setLcdMode(MODE_OAM);
		}

		m_lineTick = 0;
	}
}

void ppu::setLcdMode(LCD_Mode m)
{
	lcdData.stat = (lcdData.stat & ~0b11) | static_cast<uint8_t>(m);
}

void ppu::startDMA(uint8_t start)
{
	dma.active = true;
	dma.byte = 0;
	dma.start_delay = 2;
	dma.value = start;
}

void ppu::tickDMA()
{
	if (!dma.active)
	{
		return;
	}

	if (dma.start_delay)
	{
		--dma.start_delay;
		return;
	}

	write(0xFE00 + dma.byte, m_bus->read((dma.value * 0x100) + dma.byte));
	dma.byte++;

	dma.active = dma.byte < 0xA0;
}

void ppu::copyFrame(uint32_t* dst) const
{
	std::lock_guard<std::mutex> lock(m_frameMutex);
	std::memcpy(dst, m_presentBuffer.data(), m_presentBuffer.size() * sizeof(uint32_t));
}
