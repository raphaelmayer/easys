#pragma once

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <source_location>
#include <sstream>
#include <string_view>

// Configuration macros - user can override these before including
#ifndef EASYS_LOG_LEVEL    // Changed from ECS_LOG_LEVEL to EASYS_LOG_LEVEL
#define EASYS_LOG_LEVEL 2  // Default: INFO (0=NONE, 1=ERROR, 2=INFO, 3=DEBUG, 4=TRACE)
#endif

#ifndef EASYS_LOG_TO_FILE    // Changed from ECS_LOG_TO_FILE
#define EASYS_LOG_TO_FILE 0  // Default: disabled
#endif

#ifndef EASYS_LOG_FILE_PATH  // Changed from ECS_LOG_FILE_PATH
#define EASYS_LOG_FILE_PATH "easys_log.txt"
#endif

#ifndef EASYS_LOG_ENABLED    // Changed from ECS_LOG_ENABLED
#define EASYS_LOG_ENABLED 1  // Master switch
#endif

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

// Compile-time string formatting helper
template <typename... Args>
inline std::string format_to_string(Args&&... args)  // Changed from constexpr to inline
{
	if constexpr (sizeof...(args) == 0)
	{
		return "";
	} else
	{
		std::ostringstream oss;
		(oss << ... << std::forward<Args>(args));
		return oss.str();
	}
}

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

// Format log message
template <typename... Args>
inline void log_impl(LogLevel level, std::string_view funcName, std::string_view message, Args&&... args,
                     const std::source_location& location = std::source_location::current())
{
	auto formatted = format_to_string(std::forward<Args>(args)...);

	std::ostringstream oss;
	oss << "[" << get_timestamp() << "] "
	    << "[" << level_to_string(level) << "] "
	    << "[" << location.file_name() << ":" << location.line() << "] " 
		<< funcName << ": "
		<< message;

	if (!formatted.empty())
	{
		oss << ": " << formatted;
	}

	auto final_message = oss.str();

	// Console output
	if (level == LogLevel::EASYS_ERROR)
	{
		std::cerr << final_message << std::endl;
	} else
	{
		std::cout << final_message << std::endl;
	}

	// File output
	FileWriter::write(final_message);
}

// Base logging macro with compile-time filtering
#define EASYS_LOG_IMPL(level, ...) \
	do \
	{ \
		if constexpr (Easys::log::is_enabled<level>::value) \
		{ \
			::Easys::log::log_impl(level, __func__, __VA_ARGS__); \
		} \
	} while (0)

// User-friendly logging macros
#define EASYS_LOG_ERROR(...) EASYS_LOG_IMPL(Easys::log::LogLevel::EASYS_ERROR, __VA_ARGS__)
#define EASYS_LOG_INFO(...) EASYS_LOG_IMPL(Easys::log::LogLevel::EASYS_INFO, __VA_ARGS__)
#define EASYS_LOG_DEBUG(...) EASYS_LOG_IMPL(Easys::log::LogLevel::EASYS_DEBUG, __VA_ARGS__)
#define EASYS_LOG_TRACE(...) EASYS_LOG_IMPL(Easys::log::LogLevel::EASYS_TRACE, __VA_ARGS__)

}  // namespace Easys::log