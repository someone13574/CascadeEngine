#include "surface_wrapper.hpp"

#include "cascade_logging.hpp"

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Surface::Surface(Window_Data window_data, std::shared_ptr<Instance> instance_ptr) : m_instance_ptr(instance_ptr)
        {
#if defined __linux__
            LOG_INFO << "Vulkan: Creating xcb window surface";

            VkXcbSurfaceCreateInfoKHR xcb_surface_create_info = {};
            xcb_surface_create_info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
            xcb_surface_create_info.pNext = nullptr;
            xcb_surface_create_info.flags = 0;
            xcb_surface_create_info.connection = window_data.connection_ptr;
            xcb_surface_create_info.window = *window_data.window_ptr;

            VkResult create_surface_result = vkCreateXcbSurfaceKHR(*(m_instance_ptr->Get_Instance()), &xcb_surface_create_info, nullptr, &m_surface);

            if (create_surface_result != VK_SUCCESS)
            {
                LOG_FATAL << "Vulkan: Failed to create xcb window surface";
                exit(EXIT_FAILURE);
            }

            LOG_TRACE << "Vulkan: Finished creating surface";
#elif defined _WIN32 || defined WIN32
            LOG_INFO << "Vulkan: Creating WIN32 window surface";

            VkWin32SurfaceCreateInfoKHR win32_surface_create_info = {};
            win32_surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            win32_surface_create_info.pNext = nullptr;
            win32_surface_create_info.flags = 0;
            win32_surface_create_info.hinstance = *window_data.hinstance_ptr;
            win32_surface_create_info.hwnd = *window_data.hwindow_ptr;

            VkResult create_surface_result = vkCreateWin32SurfaceKHR(*(m_instance_ptr->Get_Instance()), &win32_surface_create_info, nullptr, &m_surface);

            if (create_surface_result != VK_SUCCESS)
            {
                LOG_FATAL << "Vulkan: Failed to create WIN32 window surface";
                exit(EXIT_FAILURE);
            }

            LOG_TRACE << "Vulkan: Finished creating surface";
#endif
        }

        Surface::~Surface()
        {
            LOG_INFO << "Vulkan: Destroying surface";

            vkDestroySurfaceKHR(*(m_instance_ptr->Get_Instance()), m_surface, nullptr);

            LOG_TRACE << "Vulkan: Finished destroying surface";
        }

        VkSurfaceKHR* Surface::Get_Surface()
        {
            return &m_surface;
        }
    } // namespace Vulkan
} // namespace Cascade_Graphics