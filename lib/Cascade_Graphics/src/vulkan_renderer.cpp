#include "vulkan_renderer.hpp"

#include "Vulkan_Backend/xcb_surface.hpp"
#include "vulkan_graphics.hpp"
#include <acorn_logging.hpp>

namespace Cascade_Graphics
{
    Vulkan_Renderer::Vulkan_Renderer(Graphics* graphics_ptr, Graphics_Platform graphics_platform, Window_Info* window_info_ptr) : m_graphics_ptr(graphics_ptr)
    {
        LOG_INFO << "Graphics: Initializing renderer with Vulkan backend";

        Vulkan_Graphics* vulkan_graphics_ptr = static_cast<Vulkan_Graphics*>(graphics_ptr);

        if (graphics_platform == Graphics_Platform::LINUX_XCB)
        {
            m_surface_ptr = new Vulkan::XCB_Surface(window_info_ptr, vulkan_graphics_ptr->m_instance_ptr);
        }

        LOG_INFO << "Graphics: Finished initializing renderer with Vulkan backend";

        (void)graphics_platform;
        (void)window_info_ptr;
    }

    Vulkan_Renderer::~Vulkan_Renderer()
    {
        LOG_INFO << "Graphics: Destroying Vulkan renderer objects";

        delete m_surface_ptr;

        LOG_INFO << "Graphics: Finished destroying Vulkan renderer objects";
    }
} // namespace Cascade_Graphics