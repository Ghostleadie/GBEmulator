//
// Created by Jack_ on 31/08/2025.
//

#ifndef GAMEBOYEMULATOR_TIMER_H
#define GAMEBOYEMULATOR_TIMER_H
#include "../interfaces/IComponentMessanger.h"
#include "../interfaces/IInterruptSink.h"
#include "../interfaces/IClocked.h"

class timer : public IComponentMessanger, public IClocked
{
public:
	timer() = default;
	~timer() override = default;

	/**
	 * Connects the timer to its interrupt sink and seeds the internal DIV counter to its post-boot value (0xAC00).
	 * @param interruptSink Sink that receives INT_TIMER on TIMA overflow; held weakly so the timer never keeps it alive.
	 */
	void init(std::shared_ptr<IInterruptSink> interruptSink);
	/**
	 * Advances the timer one T-cycle: increments the 16-bit DIV counter and, when enabled, clocks TIMA on a falling edge of the TAC-selected DIV bit.
	 * TAC bits 0-1 pick the divider bit (9/3/5/7, i.e. 4096/262144/65536/16384 Hz); TAC bit 2 enables counting.
	 * On a TIMA overflow (0xFF -> 0x00) TIMA reloads from TMA and INT_TIMER is raised.
	 */
	void tick() override;

	/**
	 * Reads a timer register selected by its global I/O address.
	 * @param address 0xFF04 DIV (high byte of the counter), 0xFF05 TIMA, 0xFF06 TMA, or 0xFF07 TAC.
	 * @return The selected register's value, or 0xFF for an unrecognised address.
	 */
	uint8_t read(uint16_t address);

	/**
	 * Writes a timer register selected by its global I/O address.
	 * @param address 0xFF04 DIV, 0xFF05 TIMA, 0xFF06 TMA, or 0xFF07 TAC.
	 * @param value Byte to store; writing DIV (0xFF04) resets the whole 16-bit counter to 0 regardless of value.
	 * Unrecognised addresses are logged and ignored.
	 */
	void write(uint16_t address, uint8_t value);

private:
	uint16_t div;
	uint8_t tima;
	uint8_t tma;
	uint8_t tac;

	std::weak_ptr<IInterruptSink> m_interruptSink;
};


#endif //GAMEBOYEMULATOR_TIMER_H