#include "vulkan_graphics_factory.hpp"

#include "vulkan_graphics.hpp"
#include "vulkan_renderer.hpp"

namespace Cascade_Graphics
{
    Vulkan_Graphics_Factory::Vulkan_Graphics_Factory(Graphics_Platform graphics_platform) : Graphics_Factory::Graphics_Factory(graphics_platform)
    {
    }

    Graphics* Vulkan_Graphics_Factory::Create_Graphics() const
    {
        return new Vulkan_Graphics();
    }

    Renderer* Vulkan_Graphics_Factory::Create_Renderer(Graphics* graphics_ptr, Window_Info* window_info_ptr) const
    {
        return new Vulkan_Renderer(graphics_ptr, m_graphics_platform, window_info_ptr);
    }
} // namespace Cascade_Graphics