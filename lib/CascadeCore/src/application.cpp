#include "application.hpp"

#include "cascade_logging.hpp"

namespace CascadeCore
{
    Application::Application(std::string name, std::string version) : m_application_name(name), m_application_version(version)
    {
        LOG_INFO << "Starting log\n";
        LOG_INFO << "Application:   " << m_application_name;
        LOG_INFO << "Version:       " << m_application_version << "\n";
    }
} // namespace CascadeCore