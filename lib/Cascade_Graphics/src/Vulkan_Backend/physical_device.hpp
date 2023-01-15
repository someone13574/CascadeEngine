#pragma once

#ifdef __linux__
    #define VK_USE_PLATFORM_XCB_KHR
#elif defined _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "queue_data.hpp"
#include <vector>
#include <vulkan/vulkan.h>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Physical_Device
        {
            friend class Physical_Device_Selector;

        private:
            VkPhysicalDevice m_physical_device;
            VkPhysicalDeviceProperties m_physical_device_properties;
            Device_Queues m_device_queues;
            std::vector<const char*> m_enabled_device_extensions;

        public:
            Physical_Device(VkPhysicalDevice physical_device);

        public:
            VkPhysicalDevice Get();
            VkPhysicalDeviceProperties Get_Properties();
            Device_Queues Get_Device_Queues();
            std::vector<const char*> Get_Device_Extensions();
        };
    }    // namespace Vulkan
}    // namespace Cascade_Graphics