#include "log_stream.hpp"
#include "logger.hpp"

namespace Cascade_Logging
{
    Log_Stream::Log_Stream(Logger& logger, std::string file, uint32_t line, Severity_Level severity) : m_logger(logger), m_occurence_file(file), m_occurence_line(line), m_message_severity(severity)
    {
        m_occurence_time = std::chrono::system_clock::now();
    }

    Log_Stream::~Log_Stream()
    {
        Log_Message message = {};
        message.message = str();
        message.time = m_occurence_time;
        message.file = m_occurence_file;
        message.line = m_occurence_line;
        message.severity = m_message_severity;

        m_logger.Queue_Message(message);
    }
} // namespace Cascade_Logging
