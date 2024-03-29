#ifndef _SPDLOG_LOGGER_
#define _SPDLOG_LOGGER_

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#else 
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#include <iostream>
#include <sstream>
#include <string>

#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1):__FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)
#endif

// define log suffix: [filename] [function:line] macro
#ifndef SUFFIX
#define SUFFIX(msg)  std::string(msg).append(" [")\
        .append(__FILENAME__).append("] [").append(__func__)\
        .append(":").append(std::to_string(__LINE__))\
        .append("]").c_str()
#endif

// must define before spdlog.h
#ifndef SPDLOG_TRACE_ON
#define SPDLOG_TRACE_ON
#endif

#ifndef SPDLOG_DEBUG_ON
#define SPDLOG_DEBUG_ON
#endif

#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"

template <typename T>
static std::string to_string(const T& n)
{
    std::ostringstream stm;
    stm << n;
    return stm.str();
}

static std::string getHomeDirectory() 
{
#ifdef _WIN32
    char* homeEnv = getenv("HOME");
    std::string homeDir;
    if (homeEnv == nullptr)
    {
        homeDir.append(getpwuid(getuid())->pw_dir);
    } else 
    {
        homeDir.append(homeEnv);
    }
#else
    std::string homeDir(getenv("HOME"));
#endif
    return homeDir;
}

static bool createDirectory(const char *dir)
{
	assert(dir != NULL);
    if (access(dir, 0) == -1) {
        std::cout << dir << " not exist, create it." << std::endl;
#ifdef _WIN32
        int flag = mkdir(dir);
#else
        int flag = mkdir(dir, 0777);
#endif
        if (flag == 0) {
            std::cout << "create directory:" << dir << " success." << std::endl;
            return true;
        } else {
            std::cout << "create directory:" << dir << " failed." << std::endl;
            return false;
        }
    }
	return true;
}

class Logger
{

public:
	static Logger& GetInstance() {
		static Logger m_instance;
		return m_instance;
	}

	auto GetLogger() { return m_logger; }

    void SetErrorLevel() {
		m_logger->set_level(spdlog::level::err);
    }

    void SetInfoLevel() {
		m_logger->set_level(spdlog::level::debug);
    }

private:
	Logger() {
        createDirectory("logs");

        // set async log
		//spdlog::set_async_mode(32768);  // power of 2
		std::vector<spdlog::sink_ptr> sinkList;
#ifdef _DEBUG
		auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		consoleSink->set_level(spdlog::level::debug);
		//consoleSink->set_pattern("[multi_sink_example] [%^%l%$] %v");
		consoleSink->set_pattern("[%m-%d %H:%M:%S.%e][%^%L%$][thread:%t]  %v");
		sinkList.push_back(consoleSink);
#endif
		auto basicSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/basicSink.log");
		basicSink->set_level(spdlog::level::debug);
		basicSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%5l%$][thread:%t]  %v");
		sinkList.push_back(basicSink);

        auto rotating = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/rotatingSink.log", 1024 * 1024, 5, false);
		rotating->set_level(spdlog::level::debug);
		rotating->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%5l%$][thread:%t]  %v");
		sinkList.push_back(rotating);

        spdlog::init_thread_pool(8192, 1);
		m_logger = std::make_shared<spdlog::async_logger>("both", begin(sinkList), end(sinkList), spdlog::thread_pool());
		//register it if you need to access it globally
		spdlog::register_logger(m_logger);

		// set log level
#ifdef _DEBUG
		m_logger->set_level(spdlog::level::trace);
#else
		m_logger->set_level(spdlog::level::err);
#endif
		//set logger format[%^%L%$]
		//m_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%5l]  %v");
		//m_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%5l%$]  %v");
        //when an error occurred, flush disk immediately
		m_logger->flush_on(spdlog::level::err);

		spdlog::flush_every(std::chrono::seconds(3));
	}

	~Logger() {
		spdlog::drop_all();
	}

	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;

private:
	std::shared_ptr<spdlog::async_logger> m_logger;
};

#define ALOG_LOGGER Logger::GetInstance()

#define LOG_TRACE(msg, ...) Logger::GetInstance().GetLogger()->trace(SUFFIX(msg), ##__VA_ARGS__)
#define LOG_DEBUG(msg, ...) Logger::GetInstance().GetLogger()->debug(SUFFIX(msg), ##__VA_ARGS__)
#define LOG_INFO(msg, ...) Logger::GetInstance().GetLogger()->info(SUFFIX(msg), ##__VA_ARGS__)
#define LOG_WARN(msg, ...) Logger::GetInstance().GetLogger()->warn(SUFFIX(msg), ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) Logger::GetInstance().GetLogger()->error(SUFFIX(msg), ##__VA_ARGS__)
#define LOG_CRITICAL(msg, ...) Logger::GetInstance().GetLogger()->critical(SUFFIX(msg), ##__VA_ARGS__)

#define criticalif(b, ...)                        \
    do {                                       \
        if ((b)) {                             \
           Logger::GetInstance().GetLogger()->critical(__VA_ARGS__); \
        }                                      \
    } while (0)

#ifdef WIN32  
#define errcode WSAGetLastError()
#endif
#endif
