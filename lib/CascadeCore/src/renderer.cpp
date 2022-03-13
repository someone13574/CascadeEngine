#include "renderer.hpp"

#include "cascade_graphics.hpp"
#include "cascade_graphics_debug.hpp"
#include "cascade_logging.hpp"

namespace CascadeCore
{
    Renderer::Renderer()
    {
        LOG_INFO << "Initializing renderer";

        CascadeGraphicsDebugging::Vulkan::Is_Vulkan_Supported();

        CascadeGraphics::Vulkan::Instance("Application name", 0);

        LOG_TRACE << "Renderer initialized";
    }

    Renderer::~Renderer()
    {
        LOG_TRACE << "Renderer cleanup started";

        LOG_TRACE << "Renderer cleanup finished";
    }
} // namespace CascadeCore
