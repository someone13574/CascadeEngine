#include "vulkan_renderer.hpp"

#include "Vulkan_Backend/win32_surface.hpp"
#include "Vulkan_Backend/xcb_surface.hpp"
#include "vulkan_graphics.hpp"
#include <acorn_logging.hpp>

namespace Cascade_Graphics
{
    Vulkan_Renderer::Vulkan_Renderer(Graphics* graphics_ptr, Platform platform, Window_Info* window_info_ptr) : m_graphics_ptr(graphics_ptr)
    {
        LOG_INFO << "Graphics: Initializing renderer with Vulkan backend";

        Vulkan_Graphics* vulkan_graphics_ptr = static_cast<Vulkan_Graphics*>(graphics_ptr);

        if (platform == Platform::LINUX_XCB)
        {
            m_surface_ptr = new Vulkan::XCB_Surface(window_info_ptr, vulkan_graphics_ptr->m_instance_ptr);
        }
        else if (platform == Platform::WINDOWS_WIN32)
        {
            m_surface_ptr = new Vulkan::WIN32_Surface(window_info_ptr, vulkan_graphics_ptr->m_instance_ptr);
        }
        else
        {
            LOG_FATAL << "Graphics (Vulkan): Unknown platform";
            exit(EXIT_FAILURE);
        }

        LOG_INFO << "Graphics: Finished initializing renderer with Vulkan backend";
    }

    Vulkan_Renderer::~Vulkan_Renderer()
    {
        LOG_INFO << "Graphics: Destroying Vulkan renderer objects";

        delete m_surface_ptr;

        LOG_INFO << "Graphics: Finished destroying Vulkan renderer objects";
    }
} // namespace Cascade_Graphics