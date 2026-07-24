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
	/** Initialises the logging system: builds the emulator, cartridge and CPU-trace loggers and their sinks.
	 * Call once at start-up before any logging macro is used. The emulator logger writes to the console and a
	 * timestamped log_YYYYMMDD_HHMMSS.txt file; the trace logger writes raw, prefix-free lines to trace.txt.
	 */
	static void Init();

	/** Returns the shared emulator logger (console + timestamped file). */
	inline static std::shared_ptr<spdlog::logger>& GetEmulatorLogger() { return m_emulatorLogger;}
	/** Returns the shared cartridge logger. */
	inline static std::shared_ptr<spdlog::logger>& GetCartridgeLogger() { return m_cartridgeLogger; }
	/** Returns the shared CPU instruction trace logger (raw lines to trace.txt). */
	inline static std::shared_ptr<spdlog::logger>& GetTraceLogger() { return m_traceLogger; }
private:
	static std::shared_ptr<spdlog::logger> m_emulatorLogger;
	static std::shared_ptr<spdlog::logger> m_cartridgeLogger;
	static std::shared_ptr<spdlog::logger> m_traceLogger;
};

#define LOG_TRACE(...)		SPDLOG_LOGGER_TRACE(::log::GetEmulatorLogger(), __VA_ARGS__ )
#define LOG_INFO(...)		SPDLOG_LOGGER_INFO(::log::GetEmulatorLogger(), __VA_ARGS__ )
#define LOG_WARN(...)		SPDLOG_LOGGER_WARN(::log::GetEmulatorLogger(), __VA_ARGS__ )
#define LOG_ERROR(...)		SPDLOG_LOGGER_ERROR(::log::GetEmulatorLogger(), __VA_ARGS__ )

#define CARTRIDGE_TRACE(...)	SPDLOG_LOGGER_TRACE(::log::GetCartridgeLogger(), __VA_ARGS__)
#define CARTRIDGE_INFO(...)		SPDLOG_LOGGER_INFO(::log::GetCartridgeLogger(), __VA_ARGS__)
#define CARTRIDGE_WARN(...)		SPDLOG_LOGGER_WARN(::log::GetCartridgeLogger(), __VA_ARGS__)
#define CARTRIDGE_ERROR(...)	SPDLOG_LOGGER_ERROR(::log::GetCartridgeLogger(), __VA_ARGS__)

// Raw, unformatted CPU instruction trace (Gameboy Doctor format) -> trace.txt
#define CPU_TRACE(...)			SPDLOG_LOGGER_INFO(::log::GetTraceLogger(), __VA_ARGS__)


#endif //GAMEBOYEMULATOR_LOG_H