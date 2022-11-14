#pragma once

#include <chrono>
#include <string>

namespace Acorn_Logging
{
    enum Severity_Level
    {
        SEVERITY_TRACE,
        SEVERITY_DEBUG,
        SEVERITY_INFO,
        SEVERITY_WARN,
        SEVERITY_ERROR,
        SEVERITY_FATAL
    };

    struct Log_Message
    {
        std::string message;

        unsigned int occurrence_line;
        std::string occurrence_file_name;
        std::chrono::time_point<std::chrono::system_clock> occurrence_time;

        Severity_Level severity_level;
    };
} // namespace Acorn_Logging