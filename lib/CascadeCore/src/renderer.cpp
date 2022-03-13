#include "renderer.hpp"

#include "cascade_graphics_debug.hpp"
#include "cascade_logging.hpp"

#include <vulkan/vulkan.hpp>

namespace CascadeCore
{
    Renderer::Renderer()
    {
        LOG_INFO << "Initializing renderer";

        CascadeGraphicsDebugging::Vulkan::Is_Vulkan_Supported();

        LOG_TRACE << "Renderer initialized";
    }

    Renderer::~Renderer()
    {
        LOG_TRACE << "Renderer cleanup started";

        LOG_TRACE << "Renderer cleanup finished";
    }
} // namespace CascadeCore
