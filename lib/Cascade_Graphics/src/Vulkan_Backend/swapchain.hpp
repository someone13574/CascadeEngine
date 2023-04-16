#pragma once

#ifdef __linux__
    #define VK_USE_PLATFORM_XCB_KHR
#elif defined _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "device.hpp"
#include "image.hpp"
#include <vector>
#include <vulkan/vulkan.h>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Swapchain_Builder;

        class Swapchain
        {
            friend class Swapchain_Builder;

        private:
            Device* m_device_ptr;

            VkSwapchainKHR m_swapchain;
            std::vector<VkImage> m_swapchain_images;
            std::vector<VkImageView> m_swapchain_image_views;

            uint32_t m_image_count;
            VkExtent2D m_image_extent;
            VkSurfaceFormatKHR m_surface_format;

        private:
            Swapchain(Device* device_ptr, uint32_t image_count, VkExtent2D image_extent, VkSurfaceFormatKHR surface_format);

        public:
            ~Swapchain();

            VkSwapchainKHR Get();
            VkImage Get_Image(uint32_t image_index);
            Image* Get_Image_Object(uint32_t image_index);

            uint32_t Get_Image_Count();
            VkExtent2D Get_Image_Extent();
            VkSurfaceFormatKHR Get_Surface_Format();
        };
    }    // namespace Vulkan
}    // namespace Cascade_Graphics