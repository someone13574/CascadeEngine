#include "image.hpp"

#include <acorn_logging.hpp>
#include <algorithm>
#include <vulkan/vk_enum_string_helper.h>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Image::Image(Device* device_ptr, VkFormat image_format, VkExtent2D image_size, VkImageUsageFlags image_usage, std::vector<Device_Queue_Requirement*> queue_requirements_with_access, VkMemoryPropertyFlags required_memory_properties, VkMemoryPropertyFlags preferred_memory_properties) :
            m_device_ptr(device_ptr), m_image_format(image_format)
        {
            LOG_DEBUG << "Graphics (Vulkan): Creating image";

            // Get queues which require access to the image
            std::vector<uint32_t> image_access_queue_families;
            for (uint32_t queue_requirement_index = 0; queue_requirement_index < queue_requirements_with_access.size(); queue_requirement_index++)
            {
                for (uint32_t queue_index = 0; queue_index < queue_requirements_with_access[queue_requirement_index]->device_queues.size(); queue_index++)
                {
                    image_access_queue_families.push_back(queue_requirements_with_access[queue_requirement_index]->device_queues[queue_index].queue_family_index);
                }
            }

            std::sort(image_access_queue_families.begin(), image_access_queue_families.end());
            image_access_queue_families.erase(std::unique(image_access_queue_families.begin(), image_access_queue_families.end()), image_access_queue_families.end());

            // Create image
            VkImageCreateInfo image_create_info = {};
            image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_create_info.pNext = nullptr;
            image_create_info.flags = 0;
            image_create_info.imageType = VK_IMAGE_TYPE_2D;
            image_create_info.format = m_image_format;
            image_create_info.extent = {image_size.width, image_size.height, 1};
            image_create_info.mipLevels = 1;
            image_create_info.arrayLayers = 1;
            image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
            image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
            image_create_info.usage = image_usage;
            image_create_info.sharingMode = (image_access_queue_families.size() == 1) ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
            image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            VkResult create_image_result = vkCreateImage(device_ptr->Get(), &image_create_info, NULL, &m_image);
            if (create_image_result != VK_SUCCESS)
            {
                LOG_FATAL << "Graphics (Vulkan): Failed to create VkImage with code " << create_image_result << " (" << string_VkResult(create_image_result) << ")";
                exit(EXIT_FAILURE);
            }

            // Allocate and bind memory
            m_device_memory = m_device_ptr->Allocate_Image_Memory(m_image, required_memory_properties, preferred_memory_properties);

            // Create image view
            VkImageViewCreateInfo image_view_create_info = {};
            image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_create_info.pNext = nullptr;
            image_view_create_info.flags = 0;
            image_view_create_info.image = m_image;
            image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            image_view_create_info.format = m_image_format;
            image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_view_create_info.subresourceRange.baseMipLevel = 0;
            image_view_create_info.subresourceRange.levelCount = 1;
            image_view_create_info.subresourceRange.baseArrayLayer = 0;
            image_view_create_info.subresourceRange.layerCount = 1;

            VkResult create_image_view_result = vkCreateImageView(device_ptr->Get(), &image_view_create_info, NULL, &m_image_view);
            if (create_image_view_result != VK_SUCCESS)
            {
                LOG_FATAL << "Graphics (Vulkan): Failed to create image view with code " << create_image_view_result << " (" << string_VkResult(create_image_view_result) << ")";
                exit(EXIT_FAILURE);
            }

            // Create sampler    TODO: Use parameters to set properties
            VkSamplerCreateInfo sampler_create_info = {};
            sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            sampler_create_info.pNext = NULL;
            sampler_create_info.flags = 0;
            sampler_create_info.magFilter = VK_FILTER_NEAREST;
            sampler_create_info.minFilter = VK_FILTER_NEAREST;
            sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
            sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            sampler_create_info.mipLodBias = 0.0;
            sampler_create_info.anisotropyEnable = VK_FALSE;
            sampler_create_info.maxAnisotropy = 0.0;
            sampler_create_info.compareEnable = VK_FALSE;
            sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
            sampler_create_info.minLod = 0.0;
            sampler_create_info.maxLod = 0.0;
            sampler_create_info.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
            sampler_create_info.unnormalizedCoordinates = VK_FALSE;

            VkResult create_sampler_result = vkCreateSampler(device_ptr->Get(), &sampler_create_info, NULL, &m_sampler);
            if (create_image_result != VK_SUCCESS)
            {
                LOG_FATAL << "Graphics (Vulkan): Failed to create sampler with code " << create_sampler_result << " (" << string_VkResult(create_sampler_result) << ")";
                exit(EXIT_FAILURE);
            }
        }

        Image::Image(VkImage image, VkImageView image_view) :
            m_image(image), m_image_view(image_view)
        {
        }

        Image::~Image()
        {
            LOG_TRACE << "Graphics (Vulkan): Destroying image";

            vkDestroyImageView(m_device_ptr->Get(), m_image_view, NULL);
            vkDestroyImage(m_device_ptr->Get(), m_image, NULL);
            vkFreeMemory(m_device_ptr->Get(), m_device_memory, NULL);
            vkDestroySampler(m_device_ptr->Get(), m_sampler, NULL);
        }

        VkImage Image::Get()
        {
            return m_image;
        }

        VkImageView Image::Get_Image_View()
        {
            return m_image_view;
        }

        VkSampler Image::Get_Sampler()
        {
            return m_sampler;
        }
    }    // namespace Vulkan
}    // namespace Cascade_Graphics