#include "surface_wrapper.hpp"

#include "cascade_logging.hpp"

namespace CascadeGraphics
{
    namespace Vulkan
    {
        Surface::Surface(xcb_connection_t* connection_ptr, xcb_window_t* window_ptr, std::shared_ptr<Instance> instance_ptr) : m_instance_ptr(instance_ptr)
        {
            LOG_INFO << "Vulkan: creating xcb window surface";

            VkXcbSurfaceCreateInfoKHR xcb_surface_create_info = {};
            xcb_surface_create_info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
            xcb_surface_create_info.pNext = NULL;
            xcb_surface_create_info.flags = 0;
            xcb_surface_create_info.connection = connection_ptr;
            xcb_surface_create_info.window = *window_ptr;

            VkResult create_surface_result = vkCreateXcbSurfaceKHR(*(m_instance_ptr->Get_Instance()), &xcb_surface_create_info, nullptr, &m_surface);

            if (create_surface_result != VK_SUCCESS)
            {
                LOG_FATAL << "Vulkan: failed to create xcb window surface";
                exit(EXIT_FAILURE);
            }

            LOG_TRACE << "Vulkan: finished creating surface";
        }

        Surface::~Surface()
        {
            LOG_INFO << "Vulkan: destroying surface";

            vkDestroySurfaceKHR(*(m_instance_ptr->Get_Instance()), m_surface, nullptr);

            LOG_TRACE << "Vulkan: finished destroying surface";
        }
    } // namespace Vulkan
} // namespace CascadeGraphics