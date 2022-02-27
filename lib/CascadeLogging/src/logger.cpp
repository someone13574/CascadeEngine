#include "logger.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <time.h>
#include <utility>

#if defined(_MSC_VER)
#define localtime_r(T, Tm) (localtime_s(Tm, T) ? NULL : Tm)
#endif

#define FILL_LENGTH 42

namespace cascade_logging
{
    Logger::Logger()
    {
        m_print_thread = std::thread(Print_Loop, this);
    }
    Logger::~Logger()
    {
        m_loop_active = false;
        m_condition_varable.notify_all();
        m_print_thread.join();

        while (!m_message_queue.empty())
        {
            std::cout << m_message_queue.front() << std::endl;
            m_message_queue.pop();
        }
    }

    void Logger::Print_Loop(Logger* instance)
    {
        std::unique_lock<std::mutex> lock_guard(instance->m_queue_mutex);
        while (instance->m_loop_active)
        {
            while (!instance->m_message_queue.empty())
            {
                std::cout << instance->m_message_queue.front() << std::endl;
                instance->m_message_queue.pop();
            }
            instance->m_condition_varable.wait(lock_guard, [instance] { return !instance->m_loop_active || !instance->m_message_queue.empty(); });
        }
    }

    void Logger::Queue_Message(Log_Message message)
    {
        std::string final_message = "";

        // Format time
        time_t time = std::chrono::system_clock::to_time_t(message.time);

        std::chrono::seconds::rep milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(message.time.time_since_epoch()).count() % 1000;

        tm local_time;
        localtime_r(&time, &local_time);

        std::ostringstream time_string_stream;
        time_string_stream << "[";
        time_string_stream << local_time.tm_year + 1900 << "-";
        time_string_stream << std::setfill('0') << std::setw(2) << local_time.tm_mon << "-";
        time_string_stream << std::setfill('0') << std::setw(2) << local_time.tm_mday << "] [";
        time_string_stream << std::setfill('0') << std::setw(2) << local_time.tm_hour << ":";
        time_string_stream << std::setfill('0') << std::setw(2) << local_time.tm_min << ":";
        time_string_stream << std::setfill('0') << std::setw(2) << local_time.tm_sec << ".";
        time_string_stream << std::setfill('0') << std::setw(3) << milliseconds << "] ";

        final_message += time_string_stream.str();

        // Add correct log level
        switch (message.severity)
        {
            case Severity_Level::LEVEL_FATAL:
                final_message += "[fatal] ";
                break;
            case Severity_Level::LEVEL_ERROR:
                final_message += "[error] ";
                break;
            case Severity_Level::LEVEL_WARN:
                final_message += "[warn]  ";
                break;
            case Severity_Level::LEVEL_INFO:
                final_message += "[info]  ";
                break;
            case Severity_Level::LEVEL_DEBUG:
                final_message += "[debug] ";
                break;
            case Severity_Level::LEVEL_TRACE:
                final_message += "[trace] ";
                break;
        }

        // Convert file path to file name and line
        message.file.erase(0, message.file.find_last_of("/") + 1);

        message.file = "[" + message.file + ":" + std::to_string(message.line) + "]";

        // Fill spaces after location
        message.file.insert(message.file.cend(), (message.file.length() > FILL_LENGTH) ? 0 : (FILL_LENGTH - message.file.length()), ' ');
        final_message += message.file;

        // Add message
        final_message += message.message;

        std::lock_guard<std::mutex> lock(m_queue_mutex);
        m_message_queue.push(std::move(final_message));
        m_condition_varable.notify_all();
    }
} // namespace cascade_logging
