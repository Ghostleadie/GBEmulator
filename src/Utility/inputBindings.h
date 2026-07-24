//
// Created by Jack_ on 24/07/2026.
//

#ifndef GAMEBOYEMULATOR_INPUTBINDINGS_H
#define GAMEBOYEMULATOR_INPUTBINDINGS_H

#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_scancode.h>

#include <cstddef>
#include <cstdint>

/**
 * The eight Game Boy inputs, in hardware order: bits 0..3 of P1 read the dpad
 * when P14 is pulled low and the buttons when P15 is. Keeping the enum in that
 * order means the joypad register can index this directly once it is wired up.
 */
enum class gbButton : std::uint8_t
{
	GB_RIGHT = 0,
	GB_LEFT,
	GB_UP,
	GB_DOWN,
	GB_A,
	GB_B,
	GB_SELECT,
	GB_START,
};

// Number of Game Boy buttons. Was the trailing GB_BUTTON_COUNT enumerator; now a
// real constant so it can still bound the binding arrays and loops below.
inline constexpr int GB_BUTTON_COUNT = 8;

/**
 * Underlying array index of a button, for subscripting the binding arrays.
 * A scoped enum will not convert to an integer implicitly, so index through this.
 * @param button Button to convert.
 * @return The button's zero-based array index.
 */
constexpr std::size_t gbIndex(const gbButton button) noexcept
{
	return static_cast<std::size_t>(button);
}

/**
 * Human-readable label for a button, e.g. "D-Pad Up".
 * @param button Button to describe.
 * @return A static display string, or "?" if the button is out of range.
 */
const char* gbButtonLabel(gbButton button);

/**
 * Keyboard + gamepad mapping, persisted under the "input" key of
 * settings/settings.json. Stored as SDL's own key/button *names* rather than
 * raw enum values, so the file survives SDL renumbering and stays hand-editable.
 */
struct inputBindings
{
	SDL_Scancode      keyboard[GB_BUTTON_COUNT] = {};
	SDL_GamepadButton gamepad[GB_BUTTON_COUNT]  = {};

	// Left analog stick doubles as the dpad. Deadzone is a fraction of full
	// deflection; anything under it reads as centred.
	bool  stickAsDpad   = true;
	float stickDeadzone = 0.5f;

	/**
	 * Builds the sensible out-of-the-box mapping (arrows + Z/X, Enter/RShift).
	 * @return A fully-populated default binding set.
	 */
	static inputBindings defaults();

	/**
	 * Replaces *this with what is on disk.
	 * Entries that are missing or name a key SDL does not recognise keep their
	 * current value, so a partially hand-edited file still loads everything valid.
	 */
	void load();

	/**
	 * Writes the "input" section. Other settings keys are preserved.
	 * @return True on success, false if the write failed.
	 */
	bool save() const;

	bool operator==(const inputBindings&) const = default;
};

#endif //GAMEBOYEMULATOR_INPUTBINDINGS_H
