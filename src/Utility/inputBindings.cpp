//
// Created by Jack_ on 24/07/2026.
//

#include "inputBindings.h"

#include "settings.h"
#include "../log/log.h"

// SDL_scancode.h (pulled in by the header) declares the enum only; the name
// lookup functions live here.
#include <SDL3/SDL_keyboard.h>

#include <algorithm>
#include <cstdlib>
#include <string>

namespace
{
	// Section of settings/settings.json these bindings live under.
	constexpr const char* INPUT_KEY = "input";

	// Stable JSON keys, indexed by gbButton. Deliberately not the display labels:
	// renaming a label must not orphan someone's saved binding.
	constexpr const char* JSON_NAMES[GB_BUTTON_COUNT] =
	{
		"right", "left", "up", "down", "a", "b", "select", "start"
	};

	constexpr const char* DISPLAY_NAMES[GB_BUTTON_COUNT] =
	{
		"D-Pad Right", "D-Pad Left", "D-Pad Up", "D-Pad Down",
		"A", "B", "Select", "Start"
	};

	// Marks a numerically-stored scancode. Must be something SDL_GetScancodeName
	// can never return.
	const std::string SCANCODE_PREFIX = "scancode:";

	/**
	 * Reads obj[key] as a scancode name into `out`.
	 * Leaves `out` alone if the entry is missing, not a string, or names a key SDL
	 * does not know. An empty string is a deliberate "unbound", not a parse failure;
	 * a "scancode:<number>" form carries codes SDL cannot name back.
	 * @param obj JSON object to read from.
	 * @param key Key within obj to look up.
	 * @param out Scancode updated on success, otherwise left unchanged.
	 */
	void readScancode(const nlohmann::json& obj, const char* key, SDL_Scancode& out)
	{
		const auto it = obj.find(key);
		if (it == obj.end() || !it->is_string())
		{
			return;
		}

		const std::string raw = it->get<std::string>();
		if (raw.empty())
		{
			out = SDL_SCANCODE_UNKNOWN;
			return;
		}

		// "scancode:<number>" is the escape hatch for codes SDL cannot name back;
		// see scancodeName below.
		if (raw.rfind(SCANCODE_PREFIX, 0) == 0)
		{
			char* end = nullptr;
			const long code = std::strtol(raw.c_str() + SCANCODE_PREFIX.size(), &end, 10);
			if (end && *end == '\0' && code > 0 && code < SDL_SCANCODE_COUNT)
			{
				out = static_cast<SDL_Scancode>(code);
			}
			else
			{
				LOG_WARN("input: bad scancode '{}' for '{}', keeping default", raw, key);
			}
			return;
		}

		const SDL_Scancode code = SDL_GetScancodeFromName(raw.c_str());
		if (code == SDL_SCANCODE_UNKNOWN)
		{
			LOG_WARN("input: unknown key name '{}' for '{}', keeping default", raw, key);
			return;
		}
		out = code;
	}

	/**
	 * Reads obj[key] as a gamepad button name into `out`.
	 * Leaves `out` alone if the entry is missing or not a string; an empty string
	 * means "unbound", and an unrecognised name keeps the current value.
	 * @param obj JSON object to read from.
	 * @param key Key within obj to look up.
	 * @param out Button updated on success, otherwise left unchanged.
	 */
	void readGamepadButton(const nlohmann::json& obj, const char* key, SDL_GamepadButton& out)
	{
		const auto it = obj.find(key);
		if (it == obj.end() || !it->is_string())
		{
			return;
		}

		const std::string raw = it->get<std::string>();
		if (raw.empty())
		{
			out = SDL_GAMEPAD_BUTTON_INVALID;
			return;
		}

		const SDL_GamepadButton button = SDL_GetGamepadButtonFromString(raw.c_str());
		if (button == SDL_GAMEPAD_BUTTON_INVALID)
		{
			LOG_WARN("input: unknown gamepad button '{}' for '{}', keeping default", raw, key);
			return;
		}
		out = button;
	}

	/**
	 * Serialises a scancode to the string stored in settings.json.
	 * Empty string means unbound. Otherwise prefer SDL's own name so the file stays
	 * readable, but names are neither unique nor universal: RETURN2 reports "Return"
	 * exactly like RETURN, and most scancodes have no name at all. Those fall back to
	 * "scancode:<number>", a prefix SDL can never produce (NONUSHASH is literally
	 * named "#", so a bare "#123" form would be ambiguous).
	 * @param code Scancode to serialise.
	 * @return The name, a "scancode:<number>" fallback, or "" when unbound.
	 */
	std::string scancodeName(const SDL_Scancode code)
	{
		if (code == SDL_SCANCODE_UNKNOWN)
		{
			return "";
		}

		const char* name = SDL_GetScancodeName(code);
		const bool usable = name && name[0]
			&& SDL_GetScancodeFromName(name) == code
			&& std::string(name).rfind(SCANCODE_PREFIX, 0) != 0;

		return usable ? std::string(name) : SCANCODE_PREFIX + std::to_string(static_cast<int>(code));
	}

	/**
	 * Serialises a gamepad button to its SDL string name for settings.json.
	 * @param button Button to serialise.
	 * @return SDL's button name, or "" when the button is invalid/unbound.
	 */
	std::string gamepadButtonName(const SDL_GamepadButton button)
	{
		if (button == SDL_GAMEPAD_BUTTON_INVALID)
		{
			return "";
		}
		const char* name = SDL_GetGamepadStringForButton(button);
		return name ? name : "";
	}
}

const char* gbButtonLabel(const gbButton button)
{
	const int i = static_cast<int>(button);
	return (i >= 0 && i < GB_BUTTON_COUNT) ? DISPLAY_NAMES[i] : "?";
}

inputBindings inputBindings::defaults()
{
	using enum gbButton;
	inputBindings b;

	b.keyboard[gbIndex(GB_RIGHT)]  = SDL_SCANCODE_RIGHT;
	b.keyboard[gbIndex(GB_LEFT)]   = SDL_SCANCODE_LEFT;
	b.keyboard[gbIndex(GB_UP)]     = SDL_SCANCODE_UP;
	b.keyboard[gbIndex(GB_DOWN)]   = SDL_SCANCODE_DOWN;
	b.keyboard[gbIndex(GB_A)]      = SDL_SCANCODE_X;
	b.keyboard[gbIndex(GB_B)]      = SDL_SCANCODE_Z;
	b.keyboard[gbIndex(GB_SELECT)] = SDL_SCANCODE_RSHIFT;
	b.keyboard[gbIndex(GB_START)]  = SDL_SCANCODE_RETURN;

	// A sits right of B on the hardware, so map them to the east/south face
	// buttons rather than to the letters printed on an Xbox pad.
	b.gamepad[gbIndex(GB_RIGHT)]  = SDL_GAMEPAD_BUTTON_DPAD_RIGHT;
	b.gamepad[gbIndex(GB_LEFT)]   = SDL_GAMEPAD_BUTTON_DPAD_LEFT;
	b.gamepad[gbIndex(GB_UP)]     = SDL_GAMEPAD_BUTTON_DPAD_UP;
	b.gamepad[gbIndex(GB_DOWN)]   = SDL_GAMEPAD_BUTTON_DPAD_DOWN;
	b.gamepad[gbIndex(GB_A)]      = SDL_GAMEPAD_BUTTON_EAST;
	b.gamepad[gbIndex(GB_B)]      = SDL_GAMEPAD_BUTTON_SOUTH;
	b.gamepad[gbIndex(GB_SELECT)] = SDL_GAMEPAD_BUTTON_BACK;
	b.gamepad[gbIndex(GB_START)]  = SDL_GAMEPAD_BUTTON_START;

	return b;
}

void inputBindings::load()
{
	*this = defaults();

	const nlohmann::json root = settings::load();
	const auto section = root.find(INPUT_KEY);
	if (section == root.end() || !section->is_object())
	{
		return; // first run: defaults stand
	}

	if (const auto keys = section->find("keyboard"); keys != section->end() && keys->is_object())
	{
		for (int i = 0; i < GB_BUTTON_COUNT; ++i)
		{
			readScancode(*keys, JSON_NAMES[i], keyboard[i]);
		}
	}

	if (const auto pad = section->find("gamepad"); pad != section->end() && pad->is_object())
	{
		for (int i = 0; i < GB_BUTTON_COUNT; ++i)
		{
			readGamepadButton(*pad, JSON_NAMES[i], gamepad[i]);
		}
	}

	if (const auto it = section->find("stickAsDpad"); it != section->end() && it->is_boolean())
	{
		stickAsDpad = it->get<bool>();
	}

	if (const auto it = section->find("stickDeadzone"); it != section->end() && it->is_number())
	{
		// Clamped so a hand-edited file cannot produce a stick that never reads
		// centred (0) or never reads deflected (1).
		stickDeadzone = std::clamp(it->get<float>(), 0.05f, 0.95f);
	}
}

bool inputBindings::save() const
{
	nlohmann::json keys    = nlohmann::json::object();
	nlohmann::json padKeys = nlohmann::json::object();

	for (int i = 0; i < GB_BUTTON_COUNT; ++i)
	{
		keys[JSON_NAMES[i]]    = scancodeName(keyboard[i]);
		padKeys[JSON_NAMES[i]] = gamepadButtonName(gamepad[i]);
	}

	nlohmann::json section;
	section["keyboard"]      = keys;
	section["gamepad"]       = padKeys;
	section["stickAsDpad"]   = stickAsDpad;
	section["stickDeadzone"] = stickDeadzone;

	return settings::saveSection(INPUT_KEY, section);
}
