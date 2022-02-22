#pragma once

#include "log_message.hpp"
#include "log_stream.hpp"

#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

namespace cascade_logging
{
    class Redirect
    {
      public:
        Redirect(){};

        template <class T>
        Redirect operator<<(T type)
        {
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

        static Log_Stream Log(std::string file, unsigned int line, Severity_Level severity)
        {
            static Logger m_handler;
            return Log_Stream(m_handler, file, line, severity);
        }

        void Queue_Message(Log_Message message);
    };
} // namespace cascade_logging

#if defined LOG_LEVEL_ALL || defined LOG_LEVEL_TRACE || defined LOG_LEVEL_DEBUG || defined LOG_LEVEL_INFO || defined LOG_LEVEL_WARN || defined LOG_LEVEL_ERROR || defined LOG_LEVEL_FATAL
#define LOG_FATAL cascade_logging::Logger::Log(__FILE__, __LINE__, cascade_logging::Severity_Level::FATAL)
#else
#define LOG_FATAL cascade_logging::Redirect()
#endif

#if defined LOG_LEVEL_ALL || defined LOG_LEVEL_TRACE || defined LOG_LEVEL_DEBUG || defined LOG_LEVEL_INFO || defined LOG_LEVEL_WARN || defined LOG_LEVEL_ERROR
#define LOG_ERROR cascade_logging::Logger::Log(__FILE__, __LINE__, cascade_logging::Severity_Level::ERROR)
#else
#define LOG_ERROR cascade_logging::Redirect()
#endif

#if defined LOG_LEVEL_ALL || defined LOG_LEVEL_TRACE || defined LOG_LEVEL_DEBUG || defined LOG_LEVEL_INFO || defined LOG_LEVEL_WARN
#define LOG_WARN cascade_logging::Logger::Log(__FILE__, __LINE__, cascade_logging::Severity_Level::WARN)
#else
#define LOG_WARN cascade_logging::Redirect()
#endif

#if defined LOG_LEVEL_ALL || defined LOG_LEVEL_TRACE || defined LOG_LEVEL_DEBUG || defined LOG_LEVEL_INFO
#define LOG_INFO cascade_logging::Logger::Log(__FILE__, __LINE__, cascade_logging::Severity_Level::INFO)
#else
#define LOG_INFO cascade_logging::Redirect()
#endif

#if defined LOG_LEVEL_ALL || defined LOG_LEVEL_TRACE || defined LOG_LEVEL_DEBUG
#define LOG_DEBUG cascade_logging::Logger::Log(__FILE__, __LINE__, cascade_logging::Severity_Level::DEBUG)
#else
#define LOG_DEBUG cascade_logging::Redirect()
#endif

#if defined LOG_LEVEL_ALL || defined LOG_LEVEL_TRACE
#define LOG_TRACE cascade_logging::Logger::Log(__FILE__, __LINE__, cascade_logging::Severity_Level::TRACE)
#else
#define LOG_TRACE cascade_logging::Redirect()
#endif
