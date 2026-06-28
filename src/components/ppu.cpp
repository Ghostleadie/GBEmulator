//
// Created by Jack_ on 31/08/2025.
//

#include "ppu.h"

void ppu::init()
{
	currentFrame = 0;
	lineTicks = 0;
	buffer = std::array<uint32_t, 160*144>{0};
	initLCD();
	lcd.stat &= ~0b11;
	lcd.stat |= MODE_OAM;
	oamRAM = std::array<oam_entry, 40>{0};
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
			case 0xFF40: return lcd.lcdc;
			case 0xFF41: return lcd.stat;
			case 0xFF42: return lcd.scrollY;
			case 0xFF43: return lcd.scrollX;
			case 0xFF44: return lcd.ly;
			case 0xFF45: return lcd.lyCompare;
			case 0xFF46: return lcd.dma;
			case 0xFF47: return lcd.bgPalette;
			case 0xFF48: return lcd.objPallette[0];
			case 0xFF49: return lcd.objPallette[1];
			case 0xFF4A: return lcd.winY;
			case 0xFF4B: return lcd.winX;
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
				lcd.lcdc = value;
				break;
			}
			case 0xFF41:
			{
				lcd.stat = value;
				break;
			}   // refine later: mask bits 0-2 (read-only)
			case 0xFF42:
			{
				lcd.scrollY = value;
				break;
			}
			case 0xFF43:
			{
				lcd.scrollX = value;
				break;
			}
			case 0xFF44:
			{
				/* LY is read-only - ignore CPU writes */
				break;
			}
			case 0xFF45:
			{
				lcd.lyCompare = value;
				break;
			}
			case 0xFF46:
			{
				lcd.dma = value;
				startDMA(value);
				break;
			}       // OAM DMA
			case 0xFF47:
			{
				lcd.bgPalette = value;
				updatePalette(value, 0);
				break;
			}
			case 0xFF48:
			{
				lcd.objPallette[0] = value;
				updatePalette(value & 0xFC, 1);
				break;
			}
			case 0xFF49:
			{
				lcd.objPallette[1] = value;
				updatePalette(value & 0xFC, 2);
				break;
			}
			case 0xFF4A:
			{
				lcd.winY = value;
				break;
			}
			case 0xFF4B:
			{
				lcd.winX = value;
				break;
			}
			default: break;
		}

	}
}

void ppu::tick()
{
	// OAM DMA advances one byte per M-cycle = once every 4 T-cycles.
	if (++m_dmaDots == 4)
	{
		m_dmaDots = 0;
		tickDMA();
	}

	if (lcd.lcdc & 0x80)
	{
		if (++m_lineDots >= 456)
		{
			m_lineDots = 0;
			if (++lcd.ly >= 154)
			{
				lcd.ly = 0;
			}
			// TODO: set STAT mode bits + raise VBlank/STAT interrupts (needs an IInterruptSink).
			
		}
	}
	else
	{
		lcd.ly = 0;
		m_lineDots = 0;
	}
}

void ppu::initLCD()
{
	for (int i = 0; i < 4; i++)
	{
		lcd.bgColours[i] = colours_default[i];
		lcd.Sprite1Colours[i] = colours_default[i];
		lcd.Sprite2Colours[i] = colours_default[i];
	}
}

void ppu::updatePalette(uint8_t paletteData, uint8_t pal)
{
	std::array<uint32_t, 4>* colours = &lcd.bgColours;
	if (pal == 1)
	{
		colours = &lcd.Sprite1Colours;
	}
	else if (pal == 2)
	{
		colours = &lcd.Sprite2Colours;
	}

	(*colours)[0] = colours_default[ paletteData       & 0b11];
	(*colours)[1] = colours_default[(paletteData >> 2) & 0b11];
	(*colours)[2] = colours_default[(paletteData >> 4) & 0b11];
	(*colours)[3] = colours_default[(paletteData >> 6) & 0b11];
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
