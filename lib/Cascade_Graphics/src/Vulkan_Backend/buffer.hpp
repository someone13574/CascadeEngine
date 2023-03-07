#pragma once

#ifdef __linux__
    #define VK_USE_PLATFORM_XCB_KHR
#elif defined _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "device.hpp"
#include "queue_data.hpp"
#include <vector>
#include <vulkan/vulkan.h>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Buffer
        {
        private:
            Device* m_device_ptr;

            VkBuffer m_buffer;
            VkDeviceMemory m_device_memory;

        public:
            Buffer(Device* device_ptr, VkBufferUsageFlags buffer_usage_flags, VkDeviceSize buffer_size, std::vector<Device_Queue_Requirement*> queue_requirements_with_access, VkMemoryPropertyFlags required_memory_properties, VkMemoryPropertyFlags preferred_memory_properties);
            ~Buffer();

            VkBuffer Get_Buffer();
        };
    }    // namespace Vulkan
}    // namespace Cascade_Graphics