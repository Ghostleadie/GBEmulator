//
// Created by Jack_ on 31/08/2025.
//

#include "ppu.h"

#include "../Utility/utility.h"

void ppu::init()
{
	currentFrame = 0;
	m_lineTick = 0;
	buffer = std::array<uint32_t, 160*144>{0};

	lineX = 0;
	pushedX = 0;
	fetchX = 0;
	state = FSTILE;

	initLCD();
	lcdData.stat &= ~0b11;
	lcdData.stat |= MODE_OAM;
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
		uint8_t high = (bgwFetchData[1] >> bit) & 1;
		uint8_t low = ((bgwFetchData[2] >> bit) & 1) << 1;
		uint32_t colorIndex = high | low;

		if (x >= 0) {
			fifo.push(colorIndex);
		}
	}

	return true;
}

void ppu::fetch()
{
	switch (state)
	{
		case FSTILE: {
			if (utility::checkBit(lcdData.lcdc, 0))
			{
				bgwFetchData[0] = read((utility::checkBit(lcdData.lcdc, 4) ? 0x8000 : 0x8800) + (mapX / 8) + ((mapY / 8) * 32));

				if ((utility::checkBit(lcdData.lcdc, 4) ? 0x8000 : 0x8800) == 0x8800)
				{
					bgwFetchData[0] += 128;
				}
			}

			state = FSDATA0;
			fetchX += 8;
		}
		break;
		case FSDATA0: {
			bgwFetchData[1] = read(utility::checkBit(lcdData.lcdc, 4) + (bgwFetchData[0] * 16) + tileY);

			state = FSDATA1;
		}
		break;
		case FSDATA1: {
			bgwFetchData[2] = read(utility::checkBit(lcdData.lcdc, 4) + (bgwFetchData[0] * 16) + tileY + 1);

			state = FSSLEEP;

		}
		break;
		case FSSLEEP: {
			state = FSPUSH;
		}
		break;
		case FSPUSH: {
			pushPixel();
			state = FSTILE;
		}
		break;
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
}

void ppu::XFERMode()
{
	process();
	if (pushedX >= XRes)
	{
		utility::ClearQueue( fifo);
		setLcdMode(MODE_HBLANK);
		if (lcdData.stat & SS_HBLANK)
		{
			if (auto s = m_interruptSink.lock())
			{
				s->raise(INT_LCD_STAT);
			}
		}
	}
}

void ppu::increment_ly()
{
	lcdData.ly++;
	if (lcdData.ly >= lcdData.lyCompare)
	{
		utility::setBitTo(lcdData.stat,2,1);
		if (lcdData.stat & SS_LYC)
		{
			if (auto s = m_interruptSink.lock())
			{
				s->raise(INT_LCD_STAT);
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

			if (auto s = m_interruptSink.lock())
			{
				s->raise(INT_VBLANK);

				if (lcdData.stat & SS_LYC)
				{
					s->raise(INT_LCD_STAT);
				}
			}

			currentFrame++;
			uint32_t end = utility::GetTicks();
			uint32_t frameTime = end - lastFrame;

			if (frameTime < TargetFPS)
			{
				utility::Delay(TargetFPS - frameTime);
			}

			if (end - startTimer >= 1000)
			{
				uint32_t fps = frameCounter;
				startTimer = end;
				frameCounter = 0;
			}

			frameCounter++;
			lastFrame = utility::GetTicks();
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
	lcdData.stat = (lcdData.stat & ~0b11) | m;
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

	write(dma.byte, m_bus->read((dma.value * 0x100) + dma.byte));
	dma.byte++;

	dma.active = dma.byte < 0xA0;
}
