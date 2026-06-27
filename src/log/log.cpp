#include "log.h"

std::shared_ptr<spdlog::logger> log::m_emulatorLogger;
std::shared_ptr<spdlog::logger> log::m_cartridgeLogger;
std::shared_ptr<spdlog::logger> log::m_traceLogger;

void log::Init()
{
	spdlog::set_pattern("%^[%T][%n][%!:%#] %l: %v%$");

	auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

	// Generate timestamped filename
	auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << "log_" << std::put_time(std::localtime(&now_c), "%Y%m%d_%H%M%S") << ".txt";
	std::string filename = ss.str();

	auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, false);

	std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};
	m_emulatorLogger = std::make_shared<spdlog::logger>("Emulator", sinks.begin(), sinks.end());
	m_emulatorLogger->set_level(spdlog::level::trace);

	//m_emulatorLogger = spdlog::basic_logger_mt("Emulator", "log.txt");
	//m_emulatorLogger = spdlog::stdout_color_mt("Emulator");
	//m_emulatorLogger->set_level(spdlog::level::trace);
	m_cartridgeLogger = spdlog::stdout_color_mt("Cartridge");
	m_cartridgeLogger->set_level(spdlog::level::trace);

	// Dedicated CPU instruction trace for the Gameboy Doctor tool.
	// File-only, raw "%v" pattern (no timestamp/name/level prefix) so the
	// output can be diffed directly against a reference log. Truncated each
	// run and flushed per line so a hung/looping ROM still leaves a usable trace.
	auto trace_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("trace.txt", true);
	m_traceLogger = std::make_shared<spdlog::logger>("Trace", trace_sink);
	m_traceLogger->set_pattern("%v");
	m_traceLogger->set_level(spdlog::level::info);
	m_traceLogger->flush_on(spdlog::level::info);
}
