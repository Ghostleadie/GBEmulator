//
// Created by Jack_ on 31/08/2025.
//

#include "timer.h"
#include "../Utility/utility.h"

void timer::init(std::shared_ptr<IInterruptSink> interruptSink)
{
	div = 0xAC00;
	m_interruptSink = interruptSink;
}

void timer::tick()
{
	uint16_t oldDiv = div;

	div++;

	bool update = false;

	switch (tac & 0b11)
	{
		case 0b00:
		{
			update = (utility::checkBit(oldDiv, 9) && !utility::checkBit(div, 9));
			break;
		}
		case 0b01:
		{
			update = (utility::checkBit(oldDiv, 3) && !utility::checkBit(div, 3));
			break;
		}
		case 0b10:
		{
			update = (utility::checkBit(oldDiv, 5) && !utility::checkBit(div, 5));
			break;
		}
		case 0b11:
		{
			update = (utility::checkBit(oldDiv, 7) && !utility::checkBit(div, 7));
			break;
		}
	}

	if (update && utility::checkBit(tac, 2))
	{
		if (tima == 0xFF)
		{
			//TIMA overflow: reload from TMA and request the timer interrupt.
			tima = tma;

			if (auto sink = m_interruptSink.lock())
			{
				sink->raise(interruptTypes::INT_TIMER);
			}
		}
		else
		{
			tima++;
		}
	}
}

uint8_t timer::read(uint16_t address)
{
	switch(address) {
		case 0xFF04:
		{
			return div >> 8;
		}
		case 0xFF05:
		{
			return tima;
		}
		case 0xFF06:
		{
			return tma;
		}
		case 0xFF07:
		{
			return tac;
		}
		default:
			LOG_ERROR("Unrecognised timer write at address: {:04X}", address);
			return 0xff;
	}
}

void timer::write(uint16_t address, uint8_t value)
{
	switch(address)
	{
		case 0xFF04:
		{
			//DIV
			div = 0;
			break;
		}

		case 0xFF05:
		{
			//TIMA
			tima = value;
			break;
		}

		case 0xFF06:
		{
			//TMA
			tma = value;
			break;
		}

		case 0xFF07:
		{
			//TAC
			tac = value;
			break;
		}
		default:
		{
			LOG_ERROR("Unrecognised timer write at address: {:04X}", address);
		}
	}
}