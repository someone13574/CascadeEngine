#pragma once

#include "log_message.hpp"

#include <chrono>
#include <sstream>

namespace cascade_logging
{
    class Logger;

    class Log_Stream : public std::ostringstream
    {
    private:
        Logger&                                            m_logger;
        std::string                                        m_occurence_file;
        unsigned int                                       m_occurence_line;
        Severity_Level                                     m_message_severity;
        std::chrono::time_point<std::chrono::system_clock> m_occurence_time;

    public:
        Log_Stream(Logger& logger, std::string file, unsigned int line, Severity_Level severity);
        ~Log_Stream();
    };
}    // namespace cascade_logging
