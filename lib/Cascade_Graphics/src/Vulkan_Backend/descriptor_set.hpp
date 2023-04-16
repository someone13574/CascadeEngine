#pragma once

#ifdef __linux__
    #define VK_USE_PLATFORM_XCB_KHR
#elif defined _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "descriptor.hpp"
#include "device.hpp"
#include <vector>
#include <vulkan/vulkan.h>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Descriptor_Set
        {
        private:
            Device* m_device_ptr;

            std::vector<Descriptor> m_descriptors;
            VkDescriptorSet m_descriptor_set;
            VkDescriptorPool m_descriptor_pool;
            VkDescriptorSetLayout m_descriptor_set_layout;

        public:
            Descriptor_Set(Device* device_ptr, std::vector<Descriptor> descriptors);
            ~Descriptor_Set();

            VkDescriptorSet* Get();
            VkDescriptorSetLayout Get_Layout();

            void Update_Image_Descriptor(uint32_t descriptor_index, Image* image_ptr);
            void Update();
        };
    }    // namespace Vulkan
}    // namespace Cascade_Graphics