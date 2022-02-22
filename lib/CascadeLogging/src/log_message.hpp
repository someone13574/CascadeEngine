#pragma once

#include <chrono>
#include <string>

namespace cascade_logging
{
    enum Severity_Level
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };

    struct Log_Message
    {
        std::string message;
        std::string file;
        unsigned int line;
        Severity_Level severity;
        std::chrono::time_point<std::chrono::system_clock> time;
    };
} // namespace cascade_logging
