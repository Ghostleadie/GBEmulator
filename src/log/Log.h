#pragma once

#include <memory>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

class Log
{
public:
	static void Init();

	inline static std::shared_ptr<spdlog::logger>& GetEmulatorLogger() { return m_emulatorLogger;}
	inline static std::shared_ptr<spdlog::logger>& GetCartridgeLogger() { return m_cartridgeLogger; }
private:
	static std::shared_ptr<spdlog::logger> m_emulatorLogger;
	static std::shared_ptr<spdlog::logger> m_cartridgeLogger;
};