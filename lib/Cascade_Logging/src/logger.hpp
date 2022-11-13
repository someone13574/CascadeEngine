#pragma once

#include "log_message.hpp"
#include "log_stream.hpp"

#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

namespace Cascade_Logging
{
    class Redirect
    {
    public:
        Redirect(){};

        template <class T>
        Redirect operator<<(T type)
        {
            (void)type;

            return *this;
        };
    };

    class Logger
    {
    private:
        std::queue<std::string> m_message_queue;
        std::thread m_print_thread;
        std::mutex m_queue_mutex;
        std::condition_variable m_condition_varable;

        bool m_loop_active = true;

    private:
        static void Print_Loop(Logger* instance);

    public:
        Logger();
        ~Logger();

        static Log_Stream Log(std::string file, uint32_t line, Severity_Level severity)
        {
            static Logger m_handler;
            return Log_Stream(m_handler, file, line, severity);
        }

        void Queue_Message(Log_Message message);
    };
} // namespace Cascade_Logging

#if defined LOG_LEVEL_ALL || defined LOG_LEVEL_TRACE || defined LOG_LEVEL_DEBUG || defined LOG_LEVEL_INFO || defined LOG_LEVEL_WARN || defined LOG_LEVEL_ERROR || defined LOG_LEVEL_FATAL
#define LOG_FATAL Cascade_Logging::Logger::Log(__FILE__, __LINE__, Cascade_Logging::Severity_Level::LEVEL_FATAL)
#else
#define LOG_FATAL Cascade_Logging::Redirect()
#endif

#if defined LOG_LEVEL_ALL || defined LOG_LEVEL_TRACE || defined LOG_LEVEL_DEBUG || defined LOG_LEVEL_INFO || defined LOG_LEVEL_WARN || defined LOG_LEVEL_ERROR
#define LOG_ERROR Cascade_Logging::Logger::Log(__FILE__, __LINE__, Cascade_Logging::Severity_Level::LEVEL_ERROR)
#else
#define LOG_ERROR Cascade_Logging::Redirect()
#endif

#if defined LOG_LEVEL_ALL || defined LOG_LEVEL_TRACE || defined LOG_LEVEL_DEBUG || defined LOG_LEVEL_INFO || defined LOG_LEVEL_WARN
#define LOG_WARN Cascade_Logging::Logger::Log(__FILE__, __LINE__, Cascade_Logging::Severity_Level::LEVEL_WARN)
#else
#define LOG_WARN Cascade_Logging::Redirect()
#endif

#if defined LOG_LEVEL_ALL || defined LOG_LEVEL_TRACE || defined LOG_LEVEL_DEBUG || defined LOG_LEVEL_INFO
#define LOG_INFO Cascade_Logging::Logger::Log(__FILE__, __LINE__, Cascade_Logging::Severity_Level::LEVEL_INFO)
#else
#define LOG_INFO Cascade_Logging::Redirect()
#endif

#if defined LOG_LEVEL_ALL || defined LOG_LEVEL_TRACE || defined LOG_LEVEL_DEBUG
#define LOG_DEBUG Cascade_Logging::Logger::Log(__FILE__, __LINE__, Cascade_Logging::Severity_Level::LEVEL_DEBUG)
#else
#define LOG_DEBUG Cascade_Logging::Redirect()
#endif

#if defined LOG_LEVEL_ALL || defined LOG_LEVEL_TRACE
#define LOG_TRACE Cascade_Logging::Logger::Log(__FILE__, __LINE__, Cascade_Logging::Severity_Level::LEVEL_TRACE)
#else
#define LOG_TRACE Cascade_Logging::Redirect()
#endif