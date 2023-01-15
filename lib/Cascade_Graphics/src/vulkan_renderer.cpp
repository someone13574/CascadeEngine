#include "vulkan_renderer.hpp"

#include "Vulkan_Backend/queue_data.hpp"
#include "Vulkan_Backend/swapchain_builder.hpp"
#include "Vulkan_Backend/win32_surface.hpp"
#include "Vulkan_Backend/xcb_surface.hpp"
#include "vulkan_graphics.hpp"
#include <acorn_logging.hpp>

namespace Cascade_Graphics
{
    Vulkan_Renderer::Vulkan_Renderer(Graphics* graphics_ptr, Platform platform, Window_Info* window_info_ptr) :
        m_graphics_ptr(graphics_ptr)
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

        m_swapchain_ptr = Vulkan::Swapchain_Builder(vulkan_graphics_ptr->m_physical_device_ptr, m_surface_ptr)
                              .Select_Image_Format(std::vector<VkSurfaceFormatKHR> {{VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}})
                              .Select_Image_Extent(window_info_ptr)
                              .Set_Swapchain_Image_Usage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT)
                              .Select_Present_Mode(std::vector<VkPresentModeKHR> {VK_PRESENT_MODE_FIFO_RELAXED_KHR, VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR})
                              .Build(vulkan_graphics_ptr->m_device_ptr);

        m_camera_buffer_ptr = new Vulkan::Buffer(vulkan_graphics_ptr->m_device_ptr, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, (VkDeviceSize)sizeof(uint32_t), std::vector<Vulkan::Device_Queue_Requirement*> {&vulkan_graphics_ptr->m_physical_device_ptr->Get_Device_Queues().device_queue_requirements[0]}, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_MEMORY_PROPERTY_HOST_CACHED_BIT);

        LOG_INFO << "Graphics: Finished initializing renderer with Vulkan backend";
    }

    Vulkan_Renderer::~Vulkan_Renderer()
    {
        LOG_INFO << "Graphics: Destroying Vulkan renderer objects";

        delete m_camera_buffer_ptr;
        delete m_swapchain_ptr;
        delete m_surface_ptr;

        LOG_INFO << "Graphics: Finished destroying Vulkan renderer objects";
    }
}    // namespace Cascade_Graphics