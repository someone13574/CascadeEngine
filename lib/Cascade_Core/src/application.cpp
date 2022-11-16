#include "application.hpp"

#include <acorn_logging.hpp>

namespace Cascade_Core
{
    Application::Application()
    {
        LOG_INFO << "Core: Initializing Cascade";
    }

    Application::~Application()
    {
        LOG_INFO << "Core: Destroying application";
    }
} // namespace Cascade_Core