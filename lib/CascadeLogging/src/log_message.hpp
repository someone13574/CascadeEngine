#pragma once

#include <chrono>
#include <string>

namespace Cascade_Logging
{
    enum Severity_Level
    {
        LEVEL_TRACE,
        LEVEL_DEBUG,
        LEVEL_INFO,
        LEVEL_WARN,
        LEVEL_ERROR,
        LEVEL_FATAL
    };

    struct Log_Message
    {
        std::string message;
        std::string file;
        unsigned int line;
        Severity_Level severity;
        std::chrono::time_point<std::chrono::system_clock> time;
    };
} // namespace Cascade_Logging
