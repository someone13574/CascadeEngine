#include "xcb_surface.hpp"

#include "../xcb_window_info.hpp"
#include "vkresult_translator.hpp"
#include <acorn_logging.hpp>

#ifdef __linux__
#include <xcb/xcb.h>
#endif

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        XCB_Surface::XCB_Surface(Window_Info* window_info_ptr, Instance* instance_ptr) : Surface::Surface(instance_ptr)
        {
#ifdef __linux__
            LOG_DEBUG << "Graphics (Vulkan): Creating XCB window surface";

            XCB_Window_Info* xcb_window_info_ptr = dynamic_cast<XCB_Window_Info*>(window_info_ptr);

            VkXcbSurfaceCreateInfoKHR surface_create_info = {};
            surface_create_info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
            surface_create_info.pNext = nullptr;
            surface_create_info.flags = 0;
            surface_create_info.connection = *reinterpret_cast<xcb_connection_t**>(xcb_window_info_ptr->Get_Connection());
            surface_create_info.window = *reinterpret_cast<xcb_window_t*>(xcb_window_info_ptr->Get_Window());

            VkResult surface_create_result = vkCreateXcbSurfaceKHR(*m_instance_ptr->Get(), &surface_create_info, nullptr, &m_surface);
            if (surface_create_result != VK_SUCCESS)
            {
                LOG_FATAL << "Graphics (Vulkan): Failed to create surface with VkResult " << surface_create_result << " (" << Translate_VkResult(surface_create_result) << ")";
                exit(EXIT_FAILURE);
            }
#endif

#ifdef _WIN32
            LOG_FATAL << "Graphics (Vulkan): Cannot create XCB surface on Windows";
            exit(EXIT_FAILURE);

            (void)window_info_ptr;
#endif
        }

        const char* XCB_Surface::Get_Surface_Extension_Name()
        {
#ifdef __linux__
            return VK_KHR_XCB_SURFACE_EXTENSION_NAME;
#endif

#ifdef _WIN32
            LOG_FATAL << "Graphics (Vulkan): Cannot get XCB surface extension name on Windows";
            exit(EXIT_FAILURE);
#endif
        }
    } // namespace Vulkan
} // namespace Cascade_Graphics