#pragma once

#include "../vulkan_header.hpp"
#include "instance_wrapper.hpp"
#include "queue_wrapper.hpp"

#include <memory>
#include <optional>

namespace CascadeGraphics
{
    namespace Vulkan
    {
        class Queue_Manager;

        class Physical_Device
        {
        private:
            VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;

        private:
            std::shared_ptr<Queue_Manager> m_queue_manager_ptr;

        private:
            bool
            Is_Device_Suitable(VkPhysicalDevice physical_device, VkPhysicalDeviceProperties physical_device_properties, VkPhysicalDeviceFeatures physical_device_features);
            unsigned int Rate_Device(VkPhysicalDeviceProperties physical_device_properties);

        public:
            Physical_Device(std::shared_ptr<Instance> instance_ptr, std::shared_ptr<Queue_Manager> queue_manager_ptr);

        public:
            VkPhysicalDevice* Get_Physical_Device();
        };
    } // namespace Vulkan
} // namespace CascadeGraphics
