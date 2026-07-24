//
// Created by Jack_ on 24/07/2026.
//

#include "settings.h"

#include <filesystem>
#include <fstream>

#include "../log/log.h"

namespace
{
	// Relative to the working directory, which is the repo root -- the same place
	// the timestamped log files and trace.txt land.
	const std::filesystem::path SETTINGS_PATH = "settings/settings.json";
	const std::filesystem::path SETTINGS_TEMP = "settings/settings.json.tmp";
}

nlohmann::json settings::load()
{
	std::ifstream in(SETTINGS_PATH);
	if (!in)
	{
		return nlohmann::json::object(); // first run, or no settings dir yet
	}

	// allow_exceptions=false so a hand-edited/truncated file degrades to defaults
	// instead of taking the process down. The UTF-8 BOM is skipped by the lexer.
	nlohmann::json parsed = nlohmann::json::parse(in, nullptr, false, true);
	if (parsed.is_discarded() || !parsed.is_object())
	{
		LOG_WARN("settings: {} is not valid JSON, using defaults", SETTINGS_PATH.string());
		return nlohmann::json::object();
	}

	return parsed;
}

bool settings::saveSection(const std::string& key, const nlohmann::json& section)
{
	nlohmann::json root = load();
	root[key] = section;

	std::error_code ec;
	std::filesystem::create_directories(SETTINGS_PATH.parent_path(), ec);
	if (ec)
	{
		LOG_ERROR("settings: cannot create {}: {}", SETTINGS_PATH.parent_path().string(), ec.message());
		return false;
	}

	// Write to a temp file and rename, so a crash mid-write cannot leave a
	// half-written settings.json behind.
	{
		std::ofstream out(SETTINGS_TEMP, std::ios::trunc);
		if (!out)
		{
			LOG_ERROR("settings: cannot open {} for writing", SETTINGS_TEMP.string());
			return false;
		}

		out << root.dump(2) << '\n';
		if (!out.good())
		{
			LOG_ERROR("settings: write to {} failed", SETTINGS_TEMP.string());
			return false;
		}
	}

	std::filesystem::rename(SETTINGS_TEMP, SETTINGS_PATH, ec);
	if (ec)
	{
		LOG_ERROR("settings: cannot replace {}: {}", SETTINGS_PATH.string(), ec.message());
		std::filesystem::remove(SETTINGS_TEMP, ec);
		return false;
	}

	return true;
}
