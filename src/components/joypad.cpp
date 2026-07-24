//
// Created by Jack_ on 31/08/2025.
//

#include "joypad.h"

void joypad::init(std::shared_ptr<IInterruptSink> interruptSink)
{
	m_interruptSink = interruptSink;
}

uint8_t joypad::selectedLow() const
{
	const uint8_t buttons = m_buttons.load(std::memory_order_relaxed);
	uint8_t low = 0;

	// P14 low (bit 4 == 0) selects the D-pad -> low nibble of the mask.
	if ((m_select & 0x10) == 0)
	{
		low |= buttons & 0x0F;
	}
	// P15 low (bit 5 == 0) selects the action buttons -> high nibble of the mask.
	if ((m_select & 0x20) == 0)
	{
		low |= (buttons >> 4) & 0x0F;
	}

	return low; // set bit = pressed + selected = line pulled low
}

uint8_t joypad::read(uint16_t /*address*/)
{
	// Bits 7-6 read high, bits 5-4 echo the select lines, bits 3-0 are the
	// selected inputs where 1 = released/open and 0 = pressed. selectedLow()
	// marks pressed lines with a 1, so invert it into the low nibble.
	return static_cast<uint8_t>(0xC0 | (m_select & 0x30) | (0x0F & ~selectedLow()));
}

void joypad::write(uint16_t /*address*/, uint8_t value)
{
	// Only P14/P15 (bits 4,5) are writable; the rest are driven by the buttons.
	m_select = value & 0x30;
}

void joypad::tick()
{
	const uint8_t low = selectedLow();

	// A bit set now but clear last tick is a selected line pulled from high to
	// low: request the joypad interrupt once, on that edge.
	if (low & ~m_prevSelectedLow)
	{
		if (const auto sink = m_interruptSink.lock())
		{
			sink->raise(interruptTypes::INT_JOYPAD);
		}
	}

	m_prevSelectedLow = low;
}
