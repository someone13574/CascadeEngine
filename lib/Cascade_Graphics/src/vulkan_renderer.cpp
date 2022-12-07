#include "vulkan_renderer.hpp"

#include <acorn_logging.hpp>

namespace Cascade_Graphics
{
    Vulkan_Renderer::Vulkan_Renderer(Graphics* graphics_ptr) : m_graphics_ptr(graphics_ptr)
    {
        LOG_INFO << "Graphics: Initializing renderer with Vulkan backend";

        LOG_INFO << "Graphics: Finished initializing renderer with Vulkan backend";
    }

    Vulkan_Renderer::~Vulkan_Renderer()
    {
    }
} // namespace Cascade_Graphics