#include "timer.h"
#include "cpu/interrupts.h"

void timer::init()
{
	timerCtx.div = 0xAC00;
}

void timer::tick(std::weak_ptr<interrupts> interrupts)
{
    uint16_t prev_div = timerCtx.div;

    timerCtx.div++;

    bool timer_update = false;

    switch (timerCtx.tac & (0b11)) {
    case 0b00:
        timer_update = (prev_div & (1 << 9)) && (!(timerCtx.div & (1 << 9)));
        break;
    case 0b01:
        timer_update = (prev_div & (1 << 3)) && (!(timerCtx.div & (1 << 3)));
        break;
    case 0b10:
        timer_update = (prev_div & (1 << 5)) && (!(timerCtx.div & (1 << 5)));
        break;
    case 0b11:
        timer_update = (prev_div & (1 << 7)) && (!(timerCtx.div & (1 << 7)));
        break;
    }

    if (timer_update && timerCtx.tac & (1 << 2)) {
        timerCtx.tima++;

        if (timerCtx.tima == 0xFF) {
            timerCtx.tima = timerCtx.tma;
            if (auto m_interrupts = interrupts.lock())
            {
                m_interrupts->requestInterrupts(IT_TIMER);
            }
        }
    }
}

void timer::write(uint16_t address, uint8_t value)
{
    switch (address) {
    case 0xFF04:
        //DIV
        timerCtx.div = 0;
        break;

    case 0xFF05:
        //TIMA
        timerCtx.tima = value;
        break;

    case 0xFF06:
        //TMA
        timerCtx.tma = value;
        break;

    case 0xFF07:
        //TAC
        timerCtx.tac = value;
        break;
    }
}

uint8_t timer::read(uint16_t address)
{
    switch (address) {
    case 0xFF04:
        return timerCtx.div >> 8;
    case 0xFF05:
        return timerCtx.tima;
    case 0xFF06:
        return timerCtx.tma;
    case 0xFF07:
        return timerCtx.tac;
    }
}

timerContext& timer::getTimerContext()
{
	return timerCtx;
}
