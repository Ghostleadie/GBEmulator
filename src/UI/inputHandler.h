//
// Frontend input poller (view/controller layer).
//

#ifndef GAMEBOYEMULATOR_INPUTHANDLER_H
#define GAMEBOYEMULATOR_INPUTHANDLER_H

#include "../Utility/inputBindings.h"

struct SDL_Gamepad;
class joypad;

// Reads the live keyboard + gamepad state each frame, maps it through the user's
// saved bindings, and pushes the resulting button mask into the emulated joypad.
//
// Runs on the main thread -- SDL input state must be polled there -- and the
// joypad carries the mask across to the emulation thread via its atomic. Lives
// in UI/ rather than components/ so the headless core stays SDL-free, the same
// reason `screen` does.
class inputHandler
{
public:
	/** Closes the open gamepad, if one is held. */
	~inputHandler();

	/** Loads the saved bindings from settings.json. Call once at startup. */
	void init();

	/**
	 * Re-reads the bindings from disk so a save in the settings window takes
	 * effect without a restart.
	 */
	void reloadBindings() { m_bindings.load(); }

	/**
	 * Samples the live keyboard and gamepad state and pushes the button mask into the joypad.
	 * Main thread, once per frame while a ROM is running.
	 * @param pad Emulated joypad the resulting button mask is written to.
	 */
	void poll(joypad& pad);

private:
	/**
	 * Opens the first connected gamepad if we do not already hold a live one.
	 * Cheap to call every poll and a no-op once one is open; also drops the
	 * handle when the pad it held has been unplugged.
	 */
	void refreshGamepad();

	inputBindings m_bindings;
	SDL_Gamepad*  m_gamepad = nullptr;
};

#endif //GAMEBOYEMULATOR_INPUTHANDLER_H
