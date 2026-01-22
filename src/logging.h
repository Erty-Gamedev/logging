#pragma once

#include <fstream>
#include <iostream>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <cstdarg>
#include <ranges>
#include "styling.h"


static std::string formattedDatetime(const char* fmt)
{
	std::stringstream buffer;
	time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	tm timeinfo{};
#ifdef _WIN32
	localtime_s(&timeinfo, &t);
#else
	localtime_r(&t, &timeinfo);
#endif
	buffer << std::put_time(&timeinfo, fmt);
	return buffer.str();
}


namespace Logging
{
	enum class LogLevel
	{
		Debug,
		Info,
		Log,
		Warning,
		Error,
	};

	static constexpr const char* getLogLevelName(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::Debug:   return "DEBUG";
		case LogLevel::Log:     return "";
		case LogLevel::Info:    return "INFO";
		case LogLevel::Warning: return "WARNING";
		case LogLevel::Error:   return "ERROR";
		}
		return "";
	}

#ifdef _DEBUG
	static inline constexpr LogLevel c_DefaultLogLevel = LogLevel::Debug;
#else
	static inline constexpr LogLevel c_DefaultLogLevel = LogLevel::Info;
#endif

	class LogHandler
	{
	protected:
		LogLevel m_loglevel;
	public:
		LogHandler() : m_loglevel(LogLevel::Info) {}
		explicit LogHandler(const LogLevel& loglevel) : m_loglevel(loglevel) {}
		void setLevel(const LogLevel& loglevel) { m_loglevel = loglevel; }
		[[nodiscard]] LogLevel getLevel() const { return m_loglevel; }
	};

	class ConsoleHandler : public LogHandler
	{
	public:
		using LogHandler::LogHandler;
		using LogHandler::setLevel;
		using LogHandler::getLevel;

		template <typename T> void log(const LogLevel& level, const T& message)
		{
			if (level < m_loglevel) { return; }

			std::ostream& os = (level > LogLevel::Info) ? std::cerr : std::cout;
			auto* pOs = (level > LogLevel::Info) ? stderr : stdout;

			const std::string levelName = std::string{ getLogLevelName(level) } + ':';

			if (!Styling::g_isVirtual)
			{
				if (level != LogLevel::Log) fprintf(pOs, "%-9s", levelName.c_str());
				os << message << std::endl;
				return;
			}

			if (level != LogLevel::Log)
			{
				os << Styling::style(Styling::bold);
				fprintf(pOs, "%-9s", levelName.c_str());
			}

			switch (level)
			{
			case LogLevel::Log:
				break;
			case LogLevel::Debug:
				os << Styling::style(Styling::debug);
				break;
			case LogLevel::Info:
				os << Styling::style(Styling::info);
				break;
			case LogLevel::Warning:
				os << Styling::style(Styling::warning);
				break;
			case LogLevel::Error:
				os << Styling::style(Styling::error);
				break;
			}

			os << message << Styling::style() << std::endl;
		}
	};

	class FileHandler : public LogHandler
	{
		bool m_fileError = false;
		bool m_logdirChecked = false;
		bool m_logfileChecked = false;
		std::filesystem::path m_logdir;
		std::ofstream m_logfile;
	public:
		FileHandler(std::filesystem::path logdir, const LogLevel& loglevel) : m_logdir(std::move(logdir)) { m_loglevel = loglevel; }
		FileHandler() : FileHandler("logs", LogLevel::Warning) {}
		~FileHandler() { if (m_logfile.is_open()) m_logfile.close(); }
		using LogHandler::setLevel;
		using LogHandler::getLevel;
		void setLogDir(const std::filesystem::path& logdir) { m_logdir = logdir; }

		template <typename T> void log(const LogLevel& level, const std::string& loggerName, const T& message)
		{
			if (level < m_loglevel || m_fileError) { return; }

			if (!m_logdirChecked && !std::filesystem::exists(m_logdir) && !std::filesystem::is_directory(m_logdir))
			{
				if (!std::filesystem::create_directories(m_logdir)) {
					std::cerr << "###  Log Error: Could not create log directory \""
						<< std::filesystem::absolute(m_logdir).string() << "\"  ###" << std::endl;
					return;
				}
			}
			m_logdirChecked = true;

			if (!m_logfileChecked)
			{
				std::filesystem::path filepath = m_logdir / formattedDatetime("log_%F.txt");
				m_logfile.open(filepath, std::ios::app);
				if (!m_logfile.is_open() || !m_logfile.good())
				{
					m_logfile.close();
					m_fileError = true;
					std::cerr << "###  Log Error: Could not create/open log file \""
						<< std::filesystem::absolute(filepath).string() << "\"  ###" << std::endl;
				}
				m_logfileChecked = true;
			}

			m_logfile << formattedDatetime("[%FT%T]") << getLogLevelName(level) << "|"
				<< loggerName << "|" << message << std::endl;
		}
	};

	class Logger
	{
	public:
		explicit Logger(std::string  name) : m_name(std::move(name)) {}
		Logger() : Logger("logger") {}
		Logger(const Logger&) = delete;

		void debug(const char* fmt, ...) const { va_list args; va_start(args, fmt); _log(LogLevel::Debug, fmt, args); va_end(args); }
		void log(const char* fmt, ...) const { va_list args; va_start(args, fmt); _log(LogLevel::Log, fmt, args); va_end(args); }
		void info(const char* fmt, ...) const { va_list args; va_start(args, fmt); _log(LogLevel::Info, fmt, args); va_end(args); }
		void warning(const char* fmt, ...) const { va_list args; va_start(args, fmt); _log(LogLevel::Warning, fmt, args); va_end(args); }
		void warn(const char* fmt, ...) const { va_list args; va_start(args, fmt); _log(LogLevel::Warning, fmt, args); va_end(args); }
		void error(const char* fmt, ...) const { va_list args; va_start(args, fmt); _log(LogLevel::Error, fmt, args); va_end(args); }
		template <typename T> void debug(T message) { _log(LogLevel::Debug, message); }
		template <typename T> void log(T message) { _log(LogLevel::Log, message); }
		template <typename T> void info(T message) { _log(LogLevel::Info, message); }
		template <typename T> void warning(T message) { _log(LogLevel::Warning, message); }
		template <typename T> void warn(T message) { _log(LogLevel::Warning, message); }
		template <typename T> void error(T message) { _log(LogLevel::Error, message); }

		[[nodiscard]] std::string getName() const { return m_name; }
		void setLevel(const LogLevel& loglevel) { m_loglevel = loglevel; }
		[[nodiscard]] LogLevel getLevel() const { return m_loglevel; }
		void setConsoleHandlerLevel(const LogLevel& loglevel) const { if (m_consoleHandler) m_consoleHandler->setLevel(loglevel); }
		void setFileHandlerLevel(const LogLevel& loglevel) const { if (m_fileHandler) m_fileHandler->setLevel(loglevel); }
		void setFileHandlerLogDir(const std::filesystem::path& logDir) const { m_fileHandler->setLogDir(logDir); }
		void setConsoleHandler(ConsoleHandler* handler) { m_consoleHandler = handler; }
		void setFileHandler(FileHandler* handler) { m_fileHandler = handler; }

		static Logger& getLogger(const std::string& loggerName)
		{
			if (s_loggers.contains(loggerName)) return *s_loggers[loggerName];
			s_loggers.insert(std::pair{ loggerName, std::make_unique<Logger>(loggerName) });
			return *s_loggers[loggerName];
		}
		static void setGlobalConsoleLevelDebug()
		{
			s_defaultConsoleHandler.setLevel(LogLevel::Debug);
			for (const auto &logger: s_loggers | std::views::values)
			{
				logger->setLevel(LogLevel::Debug);
				logger->setConsoleHandlerLevel(LogLevel::Debug);
			}
		}
	private:
		static inline std::unordered_map<std::string, std::unique_ptr<Logger>> s_loggers{};
		static inline ConsoleHandler s_defaultConsoleHandler{ c_DefaultLogLevel };
		static inline FileHandler s_defaultFileHandler;

		const std::string m_name;
		LogLevel m_loglevel = c_DefaultLogLevel;
		ConsoleHandler* m_consoleHandler = &s_defaultConsoleHandler;
		FileHandler* m_fileHandler = &s_defaultFileHandler;

		void _log(const LogLevel& level, const char* fmt, va_list args) const
		{
			if (level < m_loglevel) { return; }

			// Make a copy of the variable arguments list so we can test for length
			va_list args2;
			va_copy(args2, args);
			std::vector<char> buffer(vsnprintf(nullptr, 0, fmt, args2) + 1);
			va_end(args2);
			vsnprintf(&buffer[0], buffer.size(), fmt, args);
			const std::string message{ &buffer[0] };

			if (m_consoleHandler) { m_consoleHandler->log(level, message); }
			if (m_fileHandler) { m_fileHandler->log(level, m_name, message); }
		}

		template <typename T> void _log(const LogLevel& level, T message)
		{
			if (level < m_loglevel) { return; }
			if (m_consoleHandler) { m_consoleHandler->log(level, message); }
			if (m_fileHandler) { m_fileHandler->log(level, m_name, message); }
		}
	};
}
