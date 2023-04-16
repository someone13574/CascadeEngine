#pragma once

#ifdef __linux__
    #define VK_USE_PLATFORM_XCB_KHR
#elif defined _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "device.hpp"
#include <vulkan/vulkan.h>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Swapchain_Builder;

        class Image
        {
            friend class Swapchain_Builder;

        private:
            Device* m_device_ptr;

            VkFormat m_image_format;

            VkImage m_image;
            VkImageView m_image_view;
            VkSampler m_sampler;
            VkDeviceMemory m_device_memory;

        public:
            Image(Device* device_ptr, VkFormat image_format, VkExtent2D image_size, VkImageUsageFlags image_usage, std::vector<Device_Queue_Requirement*> queue_requirements_with_access, VkMemoryPropertyFlags required_memory_properties, VkMemoryPropertyFlags preferred_memory_properties);
            Image(VkImage image, VkImageView image_view);    // For swapchain images
            ~Image();

            VkImage Get();
            VkImageView Get_Image_View();
            VkSampler Get_Sampler();
        };
    }    // namespace Vulkan
}    // namespace Cascade_Graphics