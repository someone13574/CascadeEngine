#include "swapchain.hpp"
#include <acorn_logging.hpp>
#include <cassert>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Swapchain::Swapchain(Device* device_ptr, uint32_t image_count, VkSurfaceFormatKHR surface_format) :
            m_device_ptr(device_ptr), m_image_count(image_count), m_surface_format(surface_format) {}

        Swapchain::~Swapchain()
        {
            LOG_TRACE << "Graphics (Vulkan): Destroying swapchain";

            for (uint32_t image_view_index = 0; image_view_index < m_image_count; image_view_index++)
            {
                vkDestroyImageView(*m_device_ptr->Get(), m_swapchain_image_views[image_view_index], NULL);
            }

            vkDestroySwapchainKHR(*m_device_ptr->Get(), m_swapchain, NULL);
        }

        VkSwapchainKHR* Swapchain::Get()
        {
            return &m_swapchain;
        }

        VkImage* Swapchain::Get_Image(uint32_t image_index)
        {
            assert(image_index < m_image_count && "Graphics (Vulkan): Image index out of range");

            return &m_swapchain_images[image_index];
        }

        uint32_t Swapchain::Get_Image_Count()
        {
            return m_image_count;
        }

        VkSurfaceFormatKHR Swapchain::Get_Surface_Format()
        {
            return m_surface_format;
        }
    }    // namespace Vulkan
}    // namespace Cascade_Graphics