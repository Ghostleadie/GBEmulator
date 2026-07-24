//
// Created by Jack_ on 24/07/2026.
//

#ifndef GAMEBOYEMULATOR_SETTINGS_H
#define GAMEBOYEMULATOR_SETTINGS_H

#include <nlohmann/json.hpp>
#include <string>

/**
 * Read/modify/write access to settings/settings.json.
 *
 * Every save is a read-merge-write of the whole document, so writing one section
 * never drops the others (volume, input, debugPanels, ...). Callers own their own
 * section and only ever touch it by key.
 */
namespace settings
{
	/**
	 * Loads and parses the settings document.
	 * @return The parsed JSON object, or an empty object if the file is missing,
	 *         unreadable, or malformed. Never throws.
	 */
	nlohmann::json load();

	/**
	 * Replaces the value at `key` and rewrites the whole file.
	 * @param key Top-level section name to replace.
	 * @param section New JSON value to store under that key.
	 * @return False (and logs) if the write failed; the existing file is left
	 *         untouched in that case.
	 */
	bool saveSection(const std::string& key, const nlohmann::json& section);
}

#endif //GAMEBOYEMULATOR_SETTINGS_H
