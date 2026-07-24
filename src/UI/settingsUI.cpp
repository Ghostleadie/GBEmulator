//
// Created by Jack_ on 24/07/2026.
//

#include "settingsUI.h"

#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_stdinc.h>

#include "imgui.h"
#include "../log/log.h"
#include "../Utility/settings.h"

#include <algorithm>

namespace
{
	// Friendlier than the hardware order the enum uses.
	using enum gbButton;
	constexpr gbButton DISPLAY_ORDER[GB_BUTTON_COUNT] =
	{
		GB_UP, GB_DOWN, GB_LEFT, GB_RIGHT, GB_A, GB_B, GB_START, GB_SELECT
	};

	constexpr const char* UNBOUND = "Unbound";

	// Top-level settings key and the value used when the file has no entry.
	constexpr const char* VOLUME_KEY     = "volume";
	constexpr int         DEFAULT_VOLUME = 75;

	constexpr const char* CONFIRM_POPUP = "Unsaved Changes";

	/**
	 * Returns a human-readable name for a keyboard scancode.
	 * @param code Scancode to name.
	 * @return The scancode's display name, or "Unbound" when it is unknown or unnamed.
	 */
	const char* keyDisplayName(const SDL_Scancode code)
	{
		if (code == SDL_SCANCODE_UNKNOWN)
		{
			return UNBOUND;
		}
		const char* name = SDL_GetScancodeName(code);
		return (name && name[0]) ? name : UNBOUND;
	}

	/**
	 * Returns a human-readable name for a gamepad button.
	 * @param button Gamepad button to name.
	 * @return The button's display name, or "Unbound" when it is invalid or unnamed.
	 */
	const char* padDisplayName(const SDL_GamepadButton button)
	{
		if (button == SDL_GAMEPAD_BUTTON_INVALID)
		{
			return UNBOUND;
		}
		const char* name = SDL_GetGamepadStringForButton(button);
		return (name && name[0]) ? name : UNBOUND;
	}
}

settingsUI::~settingsUI()
{
	if (m_gamepad)
	{
		SDL_CloseGamepad(m_gamepad);
	}
}

void settingsUI::refreshGamepad()
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
		if (!m_gamepad)
		{
			LOG_WARN("settings: could not open gamepad: {}", SDL_GetError());
		}
	}

	SDL_free(ids);
}

void settingsUI::beginCapture(const int index, const bool gamepadTab)
{
	m_capturing      = index;
	m_captureGamepad = gamepadTab;

	// Snapshot what is already down. Without this the Enter key that activated
	// the button (keyboard nav) would immediately bind itself.
	int numKeys = 0;
	if (const bool* keys = SDL_GetKeyboardState(&numKeys))
	{
		const int limit = (numKeys < SDL_SCANCODE_COUNT) ? numKeys : SDL_SCANCODE_COUNT;
		for (int i = 0; i < limit; ++i)
		{
			m_keyHeldAtStart[i] = keys[i];
		}
	}

	for (int i = 0; i < SDL_GAMEPAD_BUTTON_COUNT; ++i)
	{
		m_padHeldAtStart[i] = m_gamepad && SDL_GetGamepadButton(m_gamepad, static_cast<SDL_GamepadButton>(i));
	}
}

void settingsUI::cancelCapture()
{
	m_capturing = -1;
}

void settingsUI::clearDuplicateKeyboard(const int index)
{
	for (int i = 0; i < GB_BUTTON_COUNT; ++i)
	{
		if (i != index && m_bindings.keyboard[i] == m_bindings.keyboard[index])
		{
			LOG_INFO("input: '{}' taken from {} by {}", keyDisplayName(m_bindings.keyboard[i]),
					 gbButtonLabel(static_cast<gbButton>(i)), gbButtonLabel(static_cast<gbButton>(index)));
			m_bindings.keyboard[i] = SDL_SCANCODE_UNKNOWN;
		}
	}
}

void settingsUI::clearDuplicateGamepad(const int index)
{
	for (int i = 0; i < GB_BUTTON_COUNT; ++i)
	{
		if (i != index && m_bindings.gamepad[i] == m_bindings.gamepad[index])
		{
			LOG_INFO("input: '{}' taken from {} by {}", padDisplayName(m_bindings.gamepad[i]),
					 gbButtonLabel(static_cast<gbButton>(i)), gbButtonLabel(static_cast<gbButton>(index)));
			m_bindings.gamepad[i] = SDL_GAMEPAD_BUTTON_INVALID;
		}
	}
}

void settingsUI::pollKeyboardCapture()
{
	int numKeys = 0;
	const bool* keys = SDL_GetKeyboardState(&numKeys);
	if (!keys)
	{
		return;
	}

	const int limit = (numKeys < SDL_SCANCODE_COUNT) ? numKeys : SDL_SCANCODE_COUNT;
	for (int i = 0; i < limit; ++i)
	{
		if (!keys[i])
		{
			m_keyHeldAtStart[i] = false; // released, so a later press counts
			continue;
		}
		if (m_keyHeldAtStart[i])
		{
			continue; // held since before capture started
		}

		m_bindings.keyboard[m_capturing] = static_cast<SDL_Scancode>(i);
		clearDuplicateKeyboard(m_capturing);
		cancelCapture();
		return;
	}
}

void settingsUI::pollGamepadCapture()
{
	if (!m_gamepad)
	{
		return;
	}

	for (int i = 0; i < SDL_GAMEPAD_BUTTON_COUNT; ++i)
	{
		const auto button = static_cast<SDL_GamepadButton>(i);
		if (!SDL_GetGamepadButton(m_gamepad, button))
		{
			m_padHeldAtStart[i] = false;
			continue;
		}
		if (m_padHeldAtStart[i])
		{
			continue;
		}

		m_bindings.gamepad[m_capturing] = button;
		clearDuplicateGamepad(m_capturing);
		cancelCapture();
		return;
	}
}

void settingsUI::drawBindingRow(const int index, const bool gamepadTab)
{
	const auto button = static_cast<gbButton>(index);
	const bool capturingThis = (m_capturing == index) && (m_captureGamepad == gamepadTab);

	ImGui::PushID(index);
	ImGui::TableNextRow();

	ImGui::TableSetColumnIndex(0);
	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(gbButtonLabel(button));

	ImGui::TableSetColumnIndex(1);
	const char* label = capturingThis
		? "Press any input..."
		: (gamepadTab ? padDisplayName(m_bindings.gamepad[index]) : keyDisplayName(m_bindings.keyboard[index]));

	if (ImGui::Button(label, ImVec2(-FLT_MIN, 0.0f)))
	{
		// Clicking the row that is already capturing cancels it.
		if (capturingThis)
		{
			cancelCapture();
		}
		else
		{
			beginCapture(index, gamepadTab);
		}
	}

	ImGui::PopID();
}

void settingsUI::drawKeyboardTab()
{
	ImGui::TextDisabled("Click a binding, then press the key you want.");
	ImGui::Spacing();

	if (ImGui::BeginTable("keyboardBindings", 2, ImGuiTableFlags_SizingStretchProp))
	{
		for (const gbButton button : DISPLAY_ORDER)
		{
			drawBindingRow(static_cast<int>(button), false);
		}
		ImGui::EndTable();
	}
}

void settingsUI::drawGamepadTab()
{
	refreshGamepad();

	if (!m_gamepad)
	{
		ImGui::TextDisabled("No gamepad detected. Plug one in -- it is picked up automatically.");
		return;
	}

	const char* name = SDL_GetGamepadName(m_gamepad);
	ImGui::Text("Gamepad: %s", name ? name : "Unknown");
	ImGui::TextDisabled("Click a binding, then press the button you want.");
	ImGui::Spacing();

	if (ImGui::BeginTable("gamepadBindings", 2, ImGuiTableFlags_SizingStretchProp))
	{
		for (const gbButton button : DISPLAY_ORDER)
		{
			drawBindingRow(static_cast<int>(button), true);
		}
		ImGui::EndTable();
	}

	ImGui::Spacing();
	ImGui::SeparatorText("Left Stick");
	ImGui::Checkbox("Use left stick as D-Pad", &m_bindings.stickAsDpad);

	ImGui::BeginDisabled(!m_bindings.stickAsDpad);
	ImGui::SliderFloat("Deadzone", &m_bindings.stickDeadzone, 0.05f, 0.95f, "%.2f");
	ImGui::EndDisabled();
}

void settingsUI::drawAudioTab()
{
	ImGui::TextDisabled("Master volume for all four sound channels. Applied on Save.");
	ImGui::Spacing();
	ImGui::SliderInt("Master Volume", &m_volume, 0, 100, "%d%%");
}

void settingsUI::loadAll()
{
	m_bindings.load();
	m_savedBindings = m_bindings;

	const nlohmann::json root = settings::load();
	m_volume = DEFAULT_VOLUME;
	if (const auto it = root.find(VOLUME_KEY); it != root.end() && it->is_number())
	{
		// Clamped so a hand-edited file cannot push the slider off its track.
		m_volume = std::clamp(it->get<int>(), 0, 100);
	}
	m_savedVolume = m_volume;
}

bool settingsUI::isDirty() const
{
	return !(m_bindings == m_savedBindings) || m_volume != m_savedVolume;
}

void settingsUI::applyChanges()
{
	// Recorded as saved either way: a failing write logs once rather than
	// re-prompting on every interaction.
	m_bindings.save();
	settings::saveSection(VOLUME_KEY, m_volume);

	m_savedBindings = m_bindings;
	m_savedVolume   = m_volume;

	// Signal the live pollers to pick up what we just wrote: the input poller
	// re-reads the bindings, the frontend pushes the volume into the APU.
	m_inputSaved  = true;
	m_volumeSaved = true;
}

void settingsUI::revertChanges()
{
	m_bindings = m_savedBindings;
	m_volume   = m_savedVolume;
}

void settingsUI::drawCloseConfirm(bool* open)
{
	if (!m_confirmClose)
	{
		return;
	}

	if (!ImGui::IsPopupOpen(CONFIRM_POPUP))
	{
		ImGui::OpenPopup(CONFIRM_POPUP);
	}

	const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (!ImGui::BeginPopupModal(CONFIRM_POPUP, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		// OpenPopup above takes effect this same frame, so reaching here means
		// ImGui dismissed it (Escape). Treat that as Cancel -- otherwise the flag
		// would just re-open it on the next frame.
		m_confirmClose = false;
		return;
	}

	ImGui::TextUnformatted("Save your changes before closing?");
	ImGui::Separator();

	const ImVec2 buttonSize(110.0f, 0.0f);

	if (ImGui::Button("Save", buttonSize))
	{
		applyChanges();
		m_confirmClose = false;
		*open = false;
		ImGui::CloseCurrentPopup();
	}
	ImGui::SameLine();

	if (ImGui::Button("Discard", buttonSize))
	{
		revertChanges();
		m_confirmClose = false;
		*open = false;
		ImGui::CloseCurrentPopup();
	}
	ImGui::SameLine();

	// Cancel leaves the window open with the edits intact.
	if (ImGui::Button("Cancel", buttonSize))
	{
		m_confirmClose = false;
		ImGui::CloseCurrentPopup();
	}

	ImGui::EndPopup();
}

void settingsUI::updateUI(bool* open)
{
	if (!m_loaded)
	{
		loadAll();
		m_loaded = true;
	}

	// Begin gets a local flag, not the caller's: closing has to go through the
	// unsaved-changes check rather than clearing *open behind our back.
	bool stayOpen = true;

	ImGui::SetNextWindowSize(ImVec2(440, 400), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Settings", &stayOpen))
	{
		if (ImGui::BeginTabBar("settingsTabs"))
		{
			if (ImGui::BeginTabItem("Keyboard"))
			{
				// Switching tabs mid-capture would leave a stale row highlighted.
				if (m_capturing >= 0 && m_captureGamepad)
				{
					cancelCapture();
				}
				drawKeyboardTab();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Gamepad"))
			{
				if (m_capturing >= 0 && !m_captureGamepad)
				{
					cancelCapture();
				}
				drawGamepadTab();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Audio"))
			{
				cancelCapture();
				drawAudioTab();
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::Separator();

		ImGui::BeginDisabled(!isDirty());
		if (ImGui::Button("Save"))
		{
			cancelCapture();
			applyChanges();
		}
		ImGui::SameLine();
		if (ImGui::Button("Revert"))
		{
			cancelCapture();
			revertChanges();
		}
		ImGui::EndDisabled();

		ImGui::SameLine();
		if (ImGui::Button("Reset All to Defaults"))
		{
			cancelCapture();
			m_bindings = inputBindings::defaults();
			m_volume   = DEFAULT_VOLUME;
		}

		if (isDirty())
		{
			ImGui::SameLine();
			ImGui::TextDisabled("(unsaved)");
		}

		// Polled after the widgets so a capture started this frame does not
		// consume the very click that started it.
		if (m_capturing >= 0)
		{
			if (m_captureGamepad)
			{
				pollGamepadCapture();
			}
			else
			{
				pollKeyboardCapture();
			}
		}
	}
	ImGui::End();

	if (!stayOpen)
	{
		cancelCapture();
		if (isDirty())
		{
			m_confirmClose = true; // window stays up behind the modal
		}
		else
		{
			*open = false;
		}
	}

	// Root level, outside the window, so the modal still shows if the window
	// itself is collapsed.
	drawCloseConfirm(open);
}
