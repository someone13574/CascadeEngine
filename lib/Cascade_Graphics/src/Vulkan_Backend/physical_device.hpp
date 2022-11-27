#pragma once

#include "vulkan_header.hpp"

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Physical_Device
        {
        private:
            VkPhysicalDevice m_physical_device;
            VkPhysicalDeviceProperties m_physical_device_properties;
            VkPhysicalDeviceFeatures m_physical_device_features;

        public:
            Physical_Device(VkPhysicalDevice physical_device);

        public:
            VkPhysicalDevice* Get();
            VkPhysicalDeviceProperties* Get_Properties();
            VkPhysicalDeviceFeatures* Get_Features();
        };
    } // namespace Vulkan
} // namespace Cascade_Graphics