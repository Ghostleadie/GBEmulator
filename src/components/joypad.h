//
// Created by Jack_ on 31/08/2025.
//

#ifndef GAMEBOYEMULATOR_JOYPAD_H
#define GAMEBOYEMULATOR_JOYPAD_H
#include "../interfaces/IComponentMessanger.h"
#include "../interfaces/IClocked.h"
#include "../interfaces/IInterruptSink.h"

#include <atomic>
#include <cstdint>
#include <memory>

// Emulated joypad register P1/JOYP at 0xFF00.
//
// The physical button state is a bitmask, one bit per Game Boy input, in the
// same order as the frontend's gbButton enum:
//   bit 0 Right   bit 4 A
//   bit 1 Left    bit 5 B
//   bit 2 Up      bit 6 Select
//   bit 3 Down    bit 7 Start
// so the low nibble is the D-pad group (selected by P14) and the high nibble is
// the action group (selected by P15). A set bit means *pressed*. This class is
// deliberately free of SDL and of the bindings type: the frontend polls the
// keyboard/gamepad, maps through the user's bindings, and pushes the finished
// mask via setButtons(), keeping the headless core SDL-free.
//
// setButtons() runs on the main (input) thread; read()/write()/tick() run on the
// emulation thread. The only shared state is the atomic button mask, so the
// joypad interrupt is raised from tick() rather than from the input poll -- that
// keeps every interrupt-flag write on the emulation thread, matching the timer
// and PPU.
class joypad : public IComponentMessanger, public IClocked
{
public:
	joypad() = default;
	~joypad() override = default;

	/**
	 * Wires up where the joypad interrupt is raised.
	 * @param interruptSink Sink that receives INT_JOYPAD; held weakly so the joypad never keeps the interrupt controller alive.
	 */
	void init(std::shared_ptr<IInterruptSink> interruptSink);

	/**
	 * Reads the P1/JOYP register (0xFF00) for the currently selected button group(s).
	 * @param address Unused; the joypad occupies only 0xFF00.
	 * @return P1 byte: bits 7-6 read high, bits 5-4 echo the P14/P15 select lines, bits 3-0 are the selected inputs (1 = released, 0 = pressed).
	 */
	uint8_t read(uint16_t address) override;

	/**
	 * Writes the P1/JOYP register (0xFF00), latching only the group-select bits.
	 * @param address Unused; the joypad occupies only 0xFF00.
	 * @param value New register value; only P14/P15 (bits 4-5) are stored, the rest are driven by the buttons.
	 */
	void write(uint16_t address, uint8_t value) override;

	/**
	 * Advances the joypad one T-cycle and raises INT_JOYPAD on the interrupt edge.
	 * A selected input line going from released to pressed (high to low) since the previous tick is the joypad-interrupt condition.
	 */
	void tick() override;

	/**
	 * Pushes the current physical button state into the atomic mask (see the class bit layout).
	 * @param buttons Bitmask where a set bit means pressed; low nibble is the D-pad, high nibble the action buttons.
	 * Called from the frontend input poll on the main thread; the mask is read on the emulation thread.
	 */
	void setButtons(uint8_t buttons) { m_buttons.store(buttons, std::memory_order_relaxed); }

private:
	/**
	 * Builds the low nibble of P1 for the currently selected group(s).
	 * @return Nibble where a set bit is a pressed, selected button (an input line pulled low); P14 selects the D-pad, P15 the action buttons.
	 */
	uint8_t selectedLow() const;

	std::atomic<uint8_t> m_buttons{0};  // physical state; main thread writes, emu thread reads
	uint8_t m_select = 0x30;            // P14/P15 select bits (bits 4,5); CPU-written
	uint8_t m_prevSelectedLow = 0;      // last tick's selectedLow(), for edge detection

	std::weak_ptr<IInterruptSink> m_interruptSink;
};


#endif //GAMEBOYEMULATOR_JOYPAD_H
