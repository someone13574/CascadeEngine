#pragma once

#ifdef __linux__
    #define VK_USE_PLATFORM_XCB_KHR
#elif defined _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "buffer.hpp"
#include "image.hpp"
#include <optional>
#include <vulkan/vulkan.h>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Descriptor_Set;

        class Descriptor
        {
            friend class Descriptor_Set;

        private:
            VkDescriptorType m_descriptor_type;

            std::optional<VkDescriptorImageInfo> m_descriptor_image_info;
            std::optional<VkDescriptorBufferInfo> m_descriptor_buffer_info;

        public:
            Descriptor(Buffer* buffer_ptr, VkDescriptorType descriptor_type);
            Descriptor(Image* image_ptr, VkDescriptorType descriptor_type);

            VkDescriptorType Get_Descriptor_Type();
            VkDescriptorImageInfo* Get_Image_Descriptor_Info();
            VkDescriptorBufferInfo* Get_Buffer_Descriptor_Info();
        };
    }    // namespace Vulkan
}    // namespace Cascade_Graphics