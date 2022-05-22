#include "application.hpp"

#include "cascade_logging.hpp"

namespace Cascade_Core
{
    Application::Application(Application_Info application_info) : m_application_info(application_info)
    {
        LOG_DEBUG << "Core: Started application '" << application_info.title << "' v" << application_info.major_version << "." << application_info.minor_version;
    }
} // namespace Cascade_Core