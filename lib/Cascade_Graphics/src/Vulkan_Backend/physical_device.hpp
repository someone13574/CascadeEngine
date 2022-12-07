#pragma once

#include "queue_data.hpp"
#include "vulkan_header.hpp"
#include <vector>

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
            VkPhysicalDeviceFeatures m_physical_device_features;
            Queue_Set m_queue_set;
            std::vector<const char*> m_enabled_device_extensions;

        public:
            Physical_Device(VkPhysicalDevice physical_device);

        public:
            VkPhysicalDevice* Get();
            VkPhysicalDeviceProperties* Get_Properties();
            VkPhysicalDeviceFeatures* Get_Features();
            Queue_Set Get_Queue_Set();
            std::vector<const char*> Get_Device_Extensions();
        };
    } // namespace Vulkan
} // namespace Cascade_Graphics