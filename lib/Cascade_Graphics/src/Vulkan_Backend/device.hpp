#pragma once

#ifdef __linux__
    #define VK_USE_PLATFORM_XCB_KHR
#elif defined _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "physical_device.hpp"
#include "queue_data.hpp"
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Device
        {
        private:
            Physical_Device* m_physical_device_ptr;

            VkDevice m_device;
            Device_Queues m_device_queues;

        public:
            Device(Physical_Device* physical_device_ptr);
            ~Device();

        private:
            std::vector<VkDeviceQueueCreateInfo> Get_Queue_Create_Information();

        public:
            VkDeviceMemory Allocate_Buffer_Memory(VkBuffer buffer, VkMemoryPropertyFlags required_memory_properties, VkMemoryPropertyFlags preferred_memory_properties);

            VkDevice Get();
            Device_Queues* Get_Device_Queues();
        };
    }    // namespace Vulkan
}    // namespace Cascade_Graphics