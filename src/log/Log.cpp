#include "Log.h"

std::shared_ptr<spdlog::logger> Log::m_emulatorLogger;
std::shared_ptr<spdlog::logger> Log::m_cartridgeLogger;

void Log::Init()
{
	spdlog::set_pattern("%^[%T] %n: %v%$");
	m_emulatorLogger = spdlog::stdout_color_mt("Emulator");
	m_emulatorLogger->set_level(spdlog::level::trace);
	m_cartridgeLogger = spdlog::stdout_color_mt("Cartridge");
	m_cartridgeLogger->set_level(spdlog::level::trace);
}
