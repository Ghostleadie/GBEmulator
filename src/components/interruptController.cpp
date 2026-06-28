//
// Created by Jack_ on 27/06/2026.
//

#include "interruptController.h"

void interruptController::raise(interruptTypes t)
{
	m_interruptFlags |= static_cast<uint8_t>(t);
}

uint8_t interruptController::read(uint16_t address)
{
	switch (address)
	{
		case 0xFF0F: return m_interruptFlags;
		case 0xFFFF: return m_interruptEnable;
		default:     return 0xFF;
	}
}

void interruptController::write(uint16_t address, uint8_t value)
{
	switch (address)
	{
		case 0xFF0F: m_interruptFlags = value; break;
		case 0xFFFF: m_interruptEnable = value; break;
		default: break;
	}
}
