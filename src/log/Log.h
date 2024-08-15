#pragma once

#include <memory>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/bundled/printf.h"

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

#define GBE_TRACE(...)		::Log::GetEmulatorLogger()->trace(__VA_ARGS__)
#define GBE_INFO(...)		::Log::GetEmulatorLogger()->info(__VA_ARGS__)
#define GBE_WARN(...)		::Log::GetEmulatorLogger()->warn(__VA_ARGS__)
#define GBE_ERROR(...)		::Log::GetEmulatorLogger()->error(__VA_ARGS__)
#define GBE_FATAL(...)		::Log::GetEmulatorLogger()->fatal(__VA_ARGS__)

#define GBE_CARTRIDGE_TRACE(...)	::Log::GetCartridgeLogger()->trace(__VA_ARGS__)
#define GBE_CARTRIDGE_INFO(...)		::Log::GetCartridgeLogger()->info(__VA_ARGS__)
#define GBE_CARTRIDGE_WARN(...)		::Log::GetCartridgeLogger()->warn(__VA_ARGS__)
#define GBE_CARTRIDGE_ERROR(...)	::Log::GetCartridgeLogger()->error(__VA_ARGS__)
#define GBE_CARTRIDGE_FATAL(...)	::Log::GetCartridgeLogger()->fatal(__VA_ARGS__)