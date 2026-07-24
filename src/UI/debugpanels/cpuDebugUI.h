//
// Created by Jack_ on 07/09/2025.
//

#ifndef GAMEBOYEMULATOR_CPUDEBUGUI_H
#define GAMEBOYEMULATOR_CPUDEBUGUI_H


class cpu;

class cpuDebugUI {
public:
	/**
	 * Renders the CPU debug panel: registers, flags, and execution controls.
	 *
	 * Shows PC/SP, the 8- and 16-bit register pairs, and the Z/N/H/C flags, and
	 * exposes toggles for single-step mode and Gameboy Doctor trace logging plus
	 * a step-to-next-instruction button.
	 * @param cpu CPU whose state is shown and whose stepping/trace modes are driven.
	 * @param open Caller's visibility flag; the window's close button clears it.
	 */
	void updateUI(cpu& cpu, bool* open);
};


#endif //GAMEBOYEMULATOR_CPUDEBUGUI_H