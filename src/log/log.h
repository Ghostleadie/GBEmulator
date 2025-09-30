#ifndef GAMEBOYEMULATOR_LOG_H
#define GAMEBOYEMULATOR_LOG_H

#include <memory>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/bundled/printf.h"
#include "spdlog/sinks/basic_file_sink.h"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE


class log
{
public:
	static void Init();

	inline static std::shared_ptr<spdlog::logger>& GetEmulatorLogger() { return m_emulatorLogger;}
	inline static std::shared_ptr<spdlog::logger>& GetCartridgeLogger() { return m_cartridgeLogger; }
private:
	static std::shared_ptr<spdlog::logger> m_emulatorLogger;
	static std::shared_ptr<spdlog::logger> m_cartridgeLogger;
};

#define LOG_TRACE(...)		SPDLOG_LOGGER_TRACE(::log::GetEmulatorLogger(), __VA_ARGS__ )
#define LOG_INFO(...)		SPDLOG_LOGGER_INFO(::log::GetEmulatorLogger(), __VA_ARGS__ )
#define LOG_WARN(...)		SPDLOG_LOGGER_WARN(::log::GetEmulatorLogger(), __VA_ARGS__ )
#define LOG_ERROR(...)		SPDLOG_LOGGER_ERROR(::log::GetEmulatorLogger(), __VA_ARGS__ )

#define CARTRIDGE_TRACE(...)	SPDLOG_LOGGER_TRACE(::log::GetCartridgeLogger(), __VA_ARGS__)
#define CARTRIDGE_INFO(...)		SPDLOG_LOGGER_INFO(::log::GetCartridgeLogger(), __VA_ARGS__)
#define CARTRIDGE_WARN(...)		SPDLOG_LOGGER_WARN(::log::GetCartridgeLogger(), __VA_ARGS__)
#define CARTRIDGE_ERROR(...)	SPDLOG_LOGGER_ERROR(::log::GetCartridgeLogger(), __VA_ARGS__)


#endif //GAMEBOYEMULATOR_LOG_H