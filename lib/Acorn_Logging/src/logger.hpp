#pragma once

#include "log_message.hpp"
#include <condition_variable>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>

namespace Acorn_Logging
{
    class Disable
    {
    public:
        Disable() {};

        template<class T>
        Disable operator<<(T type)
        {
            return *this;
        }
    };

    class Logger;

    class Log_Stream : public std::ostringstream
    {
    private:
        Logger* m_logger_ptr;
        Log_Message log_message_info;

    public:
        Log_Stream(Logger* logger_ptr, Severity_Level severity_level, unsigned int occurrence_line, std::string occurrence_file);
        ~Log_Stream();
    };

    class Logger
    {
        friend class Log_Stream;

    private:
        std::queue<Log_Message> m_message_queue;

        bool m_loop_active;
        std::thread m_processing_thread;

        std::mutex m_queue_mutex;
        std::condition_variable m_thread_notify;

    private:
        static void Dump_Queue(Logger* logger_ptr);
        static void Processing_Loop(Logger* logger_ptr);

        Logger();
        ~Logger();

    public:
        static Log_Stream Log(Severity_Level severity_level, unsigned int occurrence_line, std::string occurrence_file);
    };
}    // namespace Acorn_Logging

#if defined LOG_LEVEL_ALL || defined LOG_LEVEL_TRACE

    #define LOG_TRACE Acorn_Logging::Logger::Log(Acorn_Logging::Severity_Level::ACORN_SEVERITY_TRACE, __LINE__, __FILE__)
    #define LOG_DEBUG Acorn_Logging::Logger::Log(Acorn_Logging::Severity_Level::ACORN_SEVERITY_DEBUG, __LINE__, __FILE__)
    #define LOG_INFO  Acorn_Logging::Logger::Log(Acorn_Logging::Severity_Level::ACORN_SEVERITY_INFO, __LINE__, __FILE__)
    #define LOG_WARN  Acorn_Logging::Logger::Log(Acorn_Logging::Severity_Level::ACORN_SEVERITY_WARN, __LINE__, __FILE__)
    #define LOG_ERROR Acorn_Logging::Logger::Log(Acorn_Logging::Severity_Level::ACORN_SEVERITY_ERROR, __LINE__, __FILE__)
    #define LOG_FATAL Acorn_Logging::Logger::Log(Acorn_Logging::Severity_Level::ACORN_SEVERITY_FATAL, __LINE__, __FILE__)

#elif defined LOG_LEVEL_DEBUG

    #define LOG_TRACE Acorn_Logging::Disable()
    #define LOG_DEBUG Acorn_Logging::Logger::Log(Acorn_Logging::Severity_Level::ACORN_SEVERITY_DEBUG, __LINE__, __FILE__)
    #define LOG_INFO  Acorn_Logging::Logger::Log(Acorn_Logging::Severity_Level::ACORN_SEVERITY_INFO, __LINE__, __FILE__)
    #define LOG_WARN  Acorn_Logging::Logger::Log(Acorn_Logging::Severity_Level::ACORN_SEVERITY_WARN, __LINE__, __FILE__)
    #define LOG_ERROR Acorn_Logging::Logger::Log(Acorn_Logging::Severity_Level::ACORN_SEVERITY_ERROR, __LINE__, __FILE__)
    #define LOG_FATAL Acorn_Logging::Logger::Log(Acorn_Logging::Severity_Level::ACORN_SEVERITY_FATAL, __LINE__, __FILE__)

#elif defined LOG_LEVEL_INFO

    #define LOG_TRACE Acorn_Logging::Disable()
    #define LOG_DEBUG Acorn_Logging::Disable()
    #define LOG_INFO  Acorn_Logging::Logger::Log(Acorn_Logging::Severity_Level::ACORN_SEVERITY_INFO, __LINE__, __FILE__)
    #define LOG_WARN  Acorn_Logging::Logger::Log(Acorn_Logging::Severity_Level::ACORN_SEVERITY_WARN, __LINE__, __FILE__)
    #define LOG_ERROR Acorn_Logging::Logger::Log(Acorn_Logging::Severity_Level::ACORN_SEVERITY_ERROR, __LINE__, __FILE__)
    #define LOG_FATAL Acorn_Logging::Logger::Log(Acorn_Logging::Severity_Level::ACORN_SEVERITY_FATAL, __LINE__, __FILE__)

#elif defined LOG_LEVEL_WARN

    #define LOG_TRACE Acorn_Logging::Disable()
    #define LOG_DEBUG Acorn_Logging::Disable()
    #define LOG_INFO  Acorn_Logging::Disable()
    #define LOG_WARN  Acorn_Logging::Logger::Log(Acorn_Logging::Severity_Level::ACORN_SEVERITY_WARN, __LINE__, __FILE__)
    #define LOG_ERROR Acorn_Logging::Logger::Log(Acorn_Logging::Severity_Level::ACORN_SEVERITY_ERROR, __LINE__, __FILE__)
    #define LOG_FATAL Acorn_Logging::Logger::Log(Acorn_Logging::Severity_Level::ACORN_SEVERITY_FATAL, __LINE__, __FILE__)

#elif defined LOG_LEVEL_ERROR

    #define LOG_TRACE Acorn_Logging::Disable()
    #define LOG_DEBUG Acorn_Logging::Disable()
    #define LOG_INFO  Acorn_Logging::Disable()
    #define LOG_WARN  Acorn_Logging::Disable()
    #define LOG_ERROR Acorn_Logging::Logger::Log(Acorn_Logging::Severity_Level::ACORN_SEVERITY_ERROR, __LINE__, __FILE__)
    #define LOG_FATAL Acorn_Logging::Logger::Log(Acorn_Logging::Severity_Level::ACORN_SEVERITY_FATAL, __LINE__, __FILE__)

#elif defined LOG_LEVEL_FATAL

    #define LOG_TRACE Acorn_Logging::Disable()
    #define LOG_DEBUG Acorn_Logging::Disable()
    #define LOG_INFO  Acorn_Logging::Disable()
    #define LOG_WARN  Acorn_Logging::Disable()
    #define LOG_ERROR Acorn_Logging::Disable()
    #define LOG_FATAL Acorn_Logging::Logger::Log(Acorn_Logging::Severity_Level::ACORN_SEVERITY_FATAL, __LINE__, __FILE__)

#else

    #define LOG_TRACE Acorn_Logging::Disable()
    #define LOG_DEBUG Acorn_Logging::Disable()
    #define LOG_INFO  Acorn_Logging::Disable()
    #define LOG_WARN  Acorn_Logging::Disable()
    #define LOG_ERROR Acorn_Logging::Disable()
    #define LOG_FATAL Acorn_Logging::Disable()

#endif