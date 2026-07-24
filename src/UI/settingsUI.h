//
// Created by Jack_ on 24/07/2026.
//

#ifndef GAMEBOYEMULATOR_SETTINGSUI_H
#define GAMEBOYEMULATOR_SETTINGSUI_H

#include "../Utility/inputBindings.h"

struct SDL_Gamepad;

// Settings window: keyboard mapping, gamepad mapping, audio. Rebinding is
// click-then-press.
//
// Edits are held in memory until the user saves. Closing with unsaved changes
// raises a modal offering Save / Discard / Cancel, so the window owns its own
// close rather than letting the caller clear the flag directly.
//
// Saving makes edits live without a restart: consumeInputChanged() lets the
// frontend reload the bindings into the joypad poller, and consumeVolumeChanged()
// lets it push the new master volume into the APU.
class settingsUI
{
public:
	/** Closes the open gamepad, if one is held. */
	~settingsUI();

	/**
	 * Draws the settings window and processes its input for one frame.
	 * Loads settings on the first call, then draws the Keyboard/Gamepad/Audio
	 * tabs, the Save/Revert/Reset buttons, polls any in-progress rebind capture,
	 * and shows the unsaved-changes prompt when the window is closed.
	 * @param open Caller's visibility flag; cleared once the window is dismissed (after the save prompt when edits are pending).
	 */
	void updateUI(bool* open);

	/**
	 * Whether Escape currently belongs to this window rather than to the app.
	 * True while a rebind is capturing (Escape is a bindable key) or the save
	 * prompt is up (Escape cancels it). The event loop checks this before quitting.
	 * @return True when this window should consume the Escape key.
	 */
	bool wantsEscape() const { return m_capturing >= 0 || m_confirmClose; }

	/**
	 * Reports whether the input bindings were saved since the last call, and clears the flag.
	 * Returns true exactly once after each save, so the caller can reload the
	 * live input bindings the moment they change on disk.
	 * @return True if a save happened since the previous call.
	 */
	bool consumeInputChanged() { const bool v = m_inputSaved; m_inputSaved = false; return v; }

	/**
	 * Reports whether the master volume was saved since the last call, and clears the flag.
	 * Mirrors consumeInputChanged() so the frontend can push the new volume into
	 * the APU the moment it changes on disk, without a restart.
	 * @return True if a save changed the volume since the previous call.
	 */
	bool consumeVolumeChanged() { const bool v = m_volumeSaved; m_volumeSaved = false; return v; }

	/** @return The master volume currently saved on disk, 0-100. */
	int savedVolume() const { return m_savedVolume; }

private:
	/** Draws the Keyboard tab: one binding row per Game Boy button. */
	void drawKeyboardTab();

	/**
	 * Draws the Gamepad tab: the binding table plus the left-stick-as-D-pad
	 * option, or a "no gamepad detected" message when none is connected.
	 */
	void drawGamepadTab();

	/**
	 * Draws the Audio tab: a master volume slider.
	 * Saved to settings.json and pushed into the APU on save (see consumeVolumeChanged()).
	 */
	void drawAudioTab();

	/**
	 * Draws the Save / Discard / Cancel prompt for closing with unsaved changes.
	 * @param open Caller's visibility flag; cleared once the user chooses Save or Discard.
	 */
	void drawCloseConfirm(bool* open);

	/** Whether there are unsaved edits (bindings or volume differ from disk). */
	bool isDirty() const;

	/**
	 * Writes the current edits to disk, then treats the current state as saved.
	 * Also raises the input-changed flag so the live poller re-reads the bindings.
	 */
	void applyChanges();

	/** Throws away the edits, restoring the state currently saved on disk. */
	void revertChanges();

	/**
	 * Draws one "label | current binding" table row; clicking the binding starts capture.
	 * @param index gbButton index this row represents.
	 * @param gamepadTab True to show and edit the gamepad binding, false for the keyboard binding.
	 */
	void drawBindingRow(int index, bool gamepadTab);

	/**
	 * Begins capturing the next input to rebind the given button.
	 * Snapshots which inputs are already held so the key or button that
	 * activated the row does not immediately bind itself.
	 * @param index gbButton index being rebound.
	 * @param gamepadTab True when capturing a gamepad button, false for a keyboard key.
	 */
	void beginCapture(int index, bool gamepadTab);

	/** Stops any in-progress rebind capture. */
	void cancelCapture();

	/**
	 * Called each frame while a keyboard rebind is capturing.
	 * Takes the first key that went down *after* capture started, so the keypress
	 * that opened the capture is ignored, then binds it and clears duplicates.
	 */
	void pollKeyboardCapture();

	/**
	 * Called each frame while a gamepad rebind is capturing.
	 * Takes the first button that went down *after* capture started, so the press
	 * that opened the capture is ignored, then binds it and clears duplicates.
	 */
	void pollGamepadCapture();

	/**
	 * Clears index's key from any other button that was bound to it, so one key
	 * can never drive two inputs.
	 * @param index gbButton index whose newly-assigned key wins the conflict.
	 */
	void clearDuplicateKeyboard(int index);

	/**
	 * Clears index's button from any other input that was bound to it, so one
	 * button can never drive two inputs.
	 * @param index gbButton index whose newly-assigned button wins the conflict.
	 */
	void clearDuplicateGamepad(int index);

	/** Opens the first connected gamepad, if any. Safe to call repeatedly. */
	void refreshGamepad();

	/**
	 * Loads the bindings and volume from settings.json into the working copies.
	 * Records them as the saved baseline for dirty-tracking; a missing or
	 * out-of-range volume falls back to the default and is clamped.
	 */
	void loadAll();

	// Working copies, plus what is currently on disk so dirt can be detected.
	inputBindings m_bindings;
	inputBindings m_savedBindings;
	int m_volume      = 0;
	int m_savedVolume = 0;

	bool m_loaded       = false;
	bool m_confirmClose = false;  // close requested, waiting on the modal
	bool m_inputSaved   = false;  // set on save, consumed by the live input poller
	bool m_volumeSaved  = false;  // set on save, consumed by the live APU volume push

	int  m_capturing      = -1;     // gbButton index being rebound, -1 when idle
	bool m_captureGamepad = false;  // which tab the capture belongs to

	// Input already held when capture began, so it does not count as the press.
	bool m_keyHeldAtStart[SDL_SCANCODE_COUNT] = {};
	bool m_padHeldAtStart[SDL_GAMEPAD_BUTTON_COUNT] = {};

	SDL_Gamepad* m_gamepad = nullptr;
};

#endif //GAMEBOYEMULATOR_SETTINGSUI_H
