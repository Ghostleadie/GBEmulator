//
// Frontend input poller (view/controller layer).
//

#include "inputHandler.h"

#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_stdinc.h>

#include "../components/joypad.h"

inputHandler::~inputHandler()
{
	if (m_gamepad)
	{
		SDL_CloseGamepad(m_gamepad);
	}
}

void inputHandler::init()
{
	m_bindings.load();
}

void inputHandler::refreshGamepad()
{
	if (m_gamepad && SDL_GamepadConnected(m_gamepad))
	{
		return; // still good
	}

	if (m_gamepad)
	{
		SDL_CloseGamepad(m_gamepad); // unplugged since we opened it
		m_gamepad = nullptr;
	}

	int count = 0;
	SDL_JoystickID* ids = SDL_GetGamepads(&count);
	if (!ids)
	{
		return;
	}

	// First pad only; multi-controller support is not a thing here yet.
	if (count > 0)
	{
		m_gamepad = SDL_OpenGamepad(ids[0]);
	}

	SDL_free(ids);
}

void inputHandler::poll(joypad& pad)
{
	refreshGamepad();

	// bit i (== gbButton i) set means that input is pressed this frame. The
	// joypad reads the low nibble as the D-pad and the high nibble as the
	// action buttons, which is exactly the gbButton order.
	uint8_t mask = 0;

	// Keyboard: a bound, currently-held key sets that button's bit.
	int numKeys = 0;
	if (const bool* keys = SDL_GetKeyboardState(&numKeys))
	{
		for (int i = 0; i < GB_BUTTON_COUNT; ++i)
		{
			const SDL_Scancode sc = m_bindings.keyboard[i];
			if (sc != SDL_SCANCODE_UNKNOWN && static_cast<int>(sc) < numKeys && keys[sc])
			{
				mask |= static_cast<uint8_t>(1u << i);
			}
		}
	}

	if (m_gamepad)
	{
		// Face/dpad buttons.
		for (int i = 0; i < GB_BUTTON_COUNT; ++i)
		{
			const SDL_GamepadButton b = m_bindings.gamepad[i];
			if (b != SDL_GAMEPAD_BUTTON_INVALID && SDL_GetGamepadButton(m_gamepad, b))
			{
				mask |= static_cast<uint8_t>(1u << i);
			}
		}

		// Left stick doubles as the D-pad. Deadzone is a fraction of full
		// deflection (Sint16 range), matching how it is stored/edited.
		if (m_bindings.stickAsDpad)
		{
			const int threshold = static_cast<int>(m_bindings.stickDeadzone * 32767.0f);
			const int x = SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_LEFTX);
			const int y = SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_LEFTY);

			if (x < -threshold) mask |= static_cast<uint8_t>(1u << gbIndex(gbButton::GB_LEFT));
			if (x >  threshold) mask |= static_cast<uint8_t>(1u << gbIndex(gbButton::GB_RIGHT));
			if (y < -threshold) mask |= static_cast<uint8_t>(1u << gbIndex(gbButton::GB_UP));
			if (y >  threshold) mask |= static_cast<uint8_t>(1u << gbIndex(gbButton::GB_DOWN));
		}
	}

	pad.setButtons(mask);
}
