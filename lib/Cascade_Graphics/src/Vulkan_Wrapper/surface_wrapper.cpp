#include "surface_wrapper.hpp"

#include "debug_tools.hpp"


namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        Surface_Wrapper::Surface_Wrapper(std::shared_ptr<Instance_Wrapper> instance_wrapper_ptr, Window_Information window_data) : m_instance_wrapper_ptr(instance_wrapper_ptr)
        {
#if defined __linux__

            LOG_INFO << "Vulkan Backend: Creating XCB window surface";

            VkXcbSurfaceCreateInfoKHR xcb_surface_create_info = {};
            xcb_surface_create_info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
            xcb_surface_create_info.pNext = nullptr;
            xcb_surface_create_info.flags = 0;
            xcb_surface_create_info.connection = window_data.xcb_connection_ptr;
            xcb_surface_create_info.window = *window_data.xcb_window_ptr;

            VALIDATE_VKRESULT(vkCreateXcbSurfaceKHR(*m_instance_wrapper_ptr->Get_Instance(), &xcb_surface_create_info, nullptr, &m_surface), "Vulkan Backend: Failed to create XCB window surface");

#elif defined _WIN32 || defined WIN32

            LOG_INFO << "Vulkan Backend: Creating WIN32 window surface";

            VkWin32SurfaceCreateInfoKHR win32_surface_create_info = {};
            win32_surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            win32_surface_create_info.pNext = nullptr;
            win32_surface_create_info.flags = 0;
            win32_surface_create_info.hinstance = *window_data.hinstance_ptr;
            win32_surface_create_info.hwnd = *window_data.hwindow_ptr;

            VALIDATE_VKRESULT(vkCreateWin32SurfaceKHR(*m_instance_wrapper_ptr->Get_Instance(), &win32_surface_create_info, nullptr, &m_surface), "Vulkan Backend: Failed to create WIN32 window surface");

#endif
            LOG_TRACE << "Vulkan Backend: Finished creating surface";
        }

        Surface_Wrapper::~Surface_Wrapper()
        {
            LOG_INFO << "Vulkan Backend: Destroying surface";

            vkDestroySurfaceKHR(*m_instance_wrapper_ptr->Get_Instance(), m_surface, nullptr);

            LOG_TRACE << "Vulkan Backend: Finished destroying surface";
        }

        VkSurfaceKHR* Surface_Wrapper::Get_Surface()
        {
            return &m_surface;
        }
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics