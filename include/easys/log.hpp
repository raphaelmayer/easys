#pragma once

#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <source_location>
#include <sstream>
#include <string_view>

#include "config.hpp"

namespace Easys::log {

enum class LogLevel : int { EASYS_NONE = 0, EASYS_ERROR = 1, EASYS_INFO = 2, EASYS_DEBUG = 3, EASYS_TRACE = 4 };

// Compile-time log level check
template <LogLevel L>
struct is_enabled {
	static constexpr bool value = (EASYS_LOG_ENABLED == 1) && (static_cast<int>(L) <= EASYS_LOG_LEVEL);
};

// Thread-safe file writer with lazy initialization
class FileWriter {
	static inline std::ofstream file_stream;
	static inline std::mutex file_mutex;

   public:
	static void write(std::string_view message)
	{
		if constexpr (EASYS_LOG_TO_FILE == 1)
		{
			std::lock_guard<std::mutex> lock(file_mutex);  // Specify template argument
			if (!file_stream.is_open())
			{
				file_stream.open(EASYS_LOG_FILE_PATH, std::ios::app);
			}
			if (file_stream)
			{
				file_stream << message << std::endl;
				file_stream.flush();
			}
		}
	}
};

// Get current timestamp
inline std::string get_timestamp()
{
	auto now = std::chrono::system_clock::now();
	auto time = std::chrono::system_clock::to_time_t(now);
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

	std::ostringstream oss;

	// Safer localtime usage
	std::tm tm;
#ifdef _WIN32
	localtime_s(&tm, &time);
#else
	localtime_r(&time, &tm);
#endif

	oss << std::put_time(&tm, "%H:%M:%S") << '.' << std::setfill('0') << std::setw(3) << ms.count();
	return oss.str();
}

// LogLevel names
constexpr std::string_view level_to_string(LogLevel level)
{
	switch (level)
	{
		case LogLevel::EASYS_ERROR:
			return "ERROR";
		case LogLevel::EASYS_INFO:
			return "INFO";
		case LogLevel::EASYS_DEBUG:
			return "DEBUG";
		case LogLevel::EASYS_TRACE:
			return "TRACE";
		default:
			return "NONE";
	}
}

struct log_location {
	std::string_view file_name_;
	std::string_view file_path_;
	std::string_view function_name_;
	std::string_view function_signature_;
	std::uint32_t line_;
	// std::uint32_t column_;

	constexpr log_location(std::string_view file, std::uint32_t line, std::string_view fullFuncName,
	                               std::string_view funcName) noexcept
	{
		// const auto& start = file.find_last_of("/") + 1;
		// const auto& start = file.find_last_of("\\") + 1;
		// const auto& start = std::min(file.find_last_of("/"), file.find_last_of("\\")) + 1;
		const auto& start = file.find_last_of("/") + 1;  // TODO: seems to suffice on win and linux?
		const auto& substr = file.substr(start, file.size() - start);

		file_path_ = file;
		file_name_ = substr;
		function_signature_ = fullFuncName;
		function_name_ = funcName;
		line_ = line;
	}

	constexpr explicit log_location(std::string_view funcName,
	                                        std::source_location l = std::source_location::current()) noexcept
	{
		log_location(l.file_name(), l.line(), l.function_name(), funcName);
	}

#if EASYS_LOG_VERBOSITY
	constexpr std::string_view file_name() const noexcept { return file_path_; }
	constexpr std::string_view function_name() const noexcept { return function_signature_; }
#else
	constexpr std::string_view file_name() const noexcept { return file_name_; }
	constexpr std::string_view function_name() const noexcept { return function_name_; }
#endif

	constexpr std::uint32_t line() const noexcept { return line_; }
	// constexpr std::uint32_t column() const noexcept { return column; }
};

// Format log message
inline void log_impl(LogLevel level, std::string_view message, log_location location)
{
	std::ostringstream oss;
	oss << "[" << get_timestamp() << "] "
	    << "[" << level_to_string(level) << "] "
	    << "[" << location.file_name() << ":" << location.line() << "] "
	    << "`" << location.function_name() << "`: " << message;

	auto final_message = oss.str();
	// auto final_message = oss.view(); better?

	if (level == LogLevel::EASYS_ERROR)
	{
		std::cerr << final_message << std::endl;
	} else
	{
		std::cout << final_message << std::endl;
	}

	FileWriter::write(final_message);
}

#define HERE Easys::log::log_location(__FILE__, __LINE__, __FUNCTION__, __func__)

// Base logging macro with compile-time filtering
#define EASYS_LOG_IMPL(level, msg) \
	do \
	{ \
		if constexpr (Easys::log::is_enabled<level>::value) \
		{ \
			::Easys::log::log_impl(level, msg, HERE); \
		} \
	} while (0)

// User-friendly logging macros
#define EASYS_LOG_ERROR(msg) EASYS_LOG_IMPL(Easys::log::LogLevel::EASYS_ERROR, msg)
#define EASYS_LOG_INFO(msg) EASYS_LOG_IMPL(Easys::log::LogLevel::EASYS_INFO, msg)
#define EASYS_LOG_DEBUG(msg) EASYS_LOG_IMPL(Easys::log::LogLevel::EASYS_DEBUG, msg)
#define EASYS_LOG_TRACE(msg) EASYS_LOG_IMPL(Easys::log::LogLevel::EASYS_TRACE, msg)

#define EASYS_E_STR(e) std::format("Entity: {}", e)
#define EASYS_EC_STR(e) std::format("Entity: {}, Component: {}", e, typeid(T).name())

class EntryExitLogger {
   public:
	EntryExitLogger(log_location location) : location_(location)
	{
		if constexpr (Easys::log::is_enabled<LogLevel::EASYS_TRACE>::value)
			log_impl(LogLevel::EASYS_TRACE, "Entry", location_);
	}

	~EntryExitLogger()
	{
		if constexpr (Easys::log::is_enabled<LogLevel::EASYS_TRACE>::value)
			log_impl(LogLevel::EASYS_TRACE, "Exit", location_);
	}

   private:
	log_location location_;
};

#define EASYS_LOG_ENTRY_EXIT Easys::log::EntryExitLogger eel(HERE)

}  // namespace Easys::log