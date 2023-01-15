#include "win32_surface.hpp"

#include "../win32_platform_info.hpp"
#include <acorn_logging.hpp>
#include <vulkan/vk_enum_string_helper.h>

#ifdef _WIN32
    #include <windows.h>
    #include <windowsx.h>
#endif

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        WIN32_Surface::WIN32_Surface(Window_Info* window_info_ptr, Instance* instance_ptr) :
            Surface::Surface(instance_ptr)
        {
#ifdef _WIN32
            LOG_DEBUG << "Graphics (Vulkan): Creating WIN32 window surface";

            WIN32_Window_Info* win32_window_info_ptr = dynamic_cast<WIN32_Window_Info*>(window_info_ptr);

            VkWin32SurfaceCreateInfoKHR surface_create_info = {};
            surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            surface_create_info.pNext = nullptr;
            surface_create_info.flags = 0;
            surface_create_info.hinstance = *reinterpret_cast<HINSTANCE*>(win32_window_info_ptr->Get_Instance());
            surface_create_info.hwnd = *reinterpret_cast<HWND*>(win32_window_info_ptr->Get_Window());

            VkResult surface_create_result = vkCreateWin32SurfaceKHR(*m_instance_ptr->Get(), &surface_create_info, nullptr, &m_surface);
            if (surface_create_result != VK_SUCCESS)
            {
                LOG_FATAL << "Graphics (Vulkan): Failed to create surface with VkResult " << surface_create_result << " (" << string_VkResult(surface_create_result) << ")";
                exit(EXIT_FAILURE);
            }
#endif

#ifdef __linux__
            LOG_FATAL << "Graphics (Vulkan): Cannot create WIN32 surface on Linux";
            exit(EXIT_FAILURE);

            (void)window_info_ptr;
#endif
        }

        const char* WIN32_Surface::Get_Surface_Extension_Name()
        {
#ifdef _WIN32
            return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#endif

#ifdef __linux__
            LOG_FATAL << "Graphics (Vulkan): Cannot get WIN32 surface extension name on Linux";
            exit(EXIT_FAILURE);
#endif
        }
    }    // namespace Vulkan
}    // namespace Cascade_Graphics