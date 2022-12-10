#include "vulkan_graphics_factory.hpp"

#include "vulkan_graphics.hpp"
#include "vulkan_renderer.hpp"

namespace Cascade_Graphics
{
    Vulkan_Graphics_Factory::Vulkan_Graphics_Factory(Window_Platform window_platform) : Graphics_Factory::Graphics_Factory(window_platform)
    {
    }

    Graphics* Vulkan_Graphics_Factory::Create_Graphics() const
    {
        return new Vulkan_Graphics(m_window_platform);
    }

    Renderer* Vulkan_Graphics_Factory::Create_Renderer(Graphics* graphics_ptr, Window_Info* window_info_ptr) const
    {
        return new Vulkan_Renderer(graphics_ptr, m_window_platform, window_info_ptr);
    }
} // namespace Cascade_Graphics