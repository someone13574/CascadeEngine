#include "logger.hpp"

#include <ctime>
#include <iomanip>
#include <iostream>

namespace Acorn_Logging
{
    Log_Stream::Log_Stream(Logger* logger_ptr, Severity_Level severity_level, unsigned int occurrence_line, std::string occurrence_file) : m_logger_ptr(logger_ptr), log_message_info({})
    {
        log_message_info.severity_level = severity_level;
        log_message_info.occurrence_line = occurrence_line;
        log_message_info.occurrence_file_name = occurrence_file;
        log_message_info.occurrence_time = std::chrono::system_clock::now();
    }

    Log_Stream::~Log_Stream()
    {
        log_message_info.message = str();

        std::lock_guard<std::mutex> queue_lock(m_logger_ptr->m_queue_mutex);
        m_logger_ptr->m_message_queue.push(std::move(log_message_info));
        m_logger_ptr->m_thread_notify.notify_all();
    }

    Logger::Logger()
    {
        m_loop_active = true;
        m_processing_thread = std::thread(Processing_Loop, this);
    }

    Logger::~Logger()
    {
        m_loop_active = false;
        m_thread_notify.notify_all();
        m_processing_thread.join();

        Dump_Queue(this);
    }

    void Logger::Dump_Queue(Logger* logger_ptr)
    {
        while (!logger_ptr->m_message_queue.empty())
        {
            Log_Message* log_message_ptr = &logger_ptr->m_message_queue.front();

            switch (log_message_ptr->severity_level)
            {
                case Severity_Level::SEVERITY_TRACE:
                    std::cout << "\033[2m[TRACE] ";
                    break;
                case Severity_Level::SEVERITY_DEBUG:
                    std::cout << "[DEBUG] ";
                    break;
                case Severity_Level::SEVERITY_INFO:
                    std::cout << "\033[34m[INFO]  ";
                    break;
                case Severity_Level::SEVERITY_WARN:
                    std::cout << "\033[33m[WARN]  ";
                    break;
                case Severity_Level::SEVERITY_ERROR:
                    std::cout << "\033[31m[ERROR] ";
                    break;
                case Severity_Level::SEVERITY_FATAL:
                    std::cout << "\033[31;1m[FATAL] ";
                    break;
                default:
                    break;
            }

            std::time_t time = std::chrono::system_clock::to_time_t(log_message_ptr->occurrence_time);

            std::tm local_time;
#if defined __linux__
            localtime_r(&time, &local_time);
#elif defined _WIN32 || defined WIN32
            localtime_s(&local_time, &time);
#endif

            std::cout << "[" << local_time.tm_year + 1900;
            std::cout << "-" << std::setw(2) << std::setfill('0') << local_time.tm_mon + 1;
            std::cout << "-" << std::setw(2) << std::setfill('0') << local_time.tm_mday << "]";
            std::cout << " [" << std::setw(2) << std::setfill('0') << local_time.tm_hour;
            std::cout << ":" << std::setw(2) << std::setfill('0') << local_time.tm_min;
            std::cout << ":" << std::setw(2) << std::setfill('0') << local_time.tm_sec;
            std::cout << "." << std::setw(2) << std::setfill('0')
                      << std::chrono::duration_cast<std::chrono::microseconds>(log_message_ptr->occurrence_time - std::chrono::time_point_cast<std::chrono::seconds>(log_message_ptr->occurrence_time)).count() << "] ";

            log_message_ptr->occurrence_file_name.erase(0, log_message_ptr->occurrence_file_name.find_last_of("/") + 1);
            log_message_ptr->occurrence_file_name.append(":");
            log_message_ptr->occurrence_file_name.append(std::to_string(log_message_ptr->occurrence_line));

            std::cout << "[" << log_message_ptr->occurrence_file_name << "]" << std::setfill(' ') << std::setw((log_message_ptr->occurrence_file_name.length() > 64) ? 0 : 64 - log_message_ptr->occurrence_file_name.length()) << "";

            std::cout << log_message_ptr->message << "\033[0m" << std::endl;
            logger_ptr->m_message_queue.pop();
        }
    }

    void Logger::Processing_Loop(Logger* logger_ptr)
    {
        std::unique_lock<std::mutex> queue_lock(logger_ptr->m_queue_mutex);

        while (logger_ptr->m_loop_active)
        {
            Dump_Queue(logger_ptr);

            logger_ptr->m_thread_notify.wait(queue_lock, [logger_ptr] { return !logger_ptr->m_loop_active || !logger_ptr->m_message_queue.empty(); });
        }
    }

    Log_Stream Logger::Log(Severity_Level severity_level, unsigned int occurrence_line, std::string occurrence_file)
    {
        static Logger logger_singleton;
        return Log_Stream(&logger_singleton, severity_level, occurrence_line, occurrence_file);
    }
} // namespace Acorn_Logging