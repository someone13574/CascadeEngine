#pragma once

#include "instance_wrapper.hpp"
#include "queue_manager.hpp"
#include "surface_wrapper.hpp"
#include "vulkan_header.hpp"

#include <memory>
#include <optional>
#include <vector>

namespace Cascade_Graphics
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
            std::shared_ptr<Surface> m_surface_ptr;

        private:
            bool Is_Device_Suitable(VkPhysicalDevice physical_device, VkPhysicalDeviceProperties physical_device_properties, VkPhysicalDeviceFeatures physical_device_features);
            bool Check_Device_Extension_Support(VkPhysicalDevice physical_device);

            unsigned int Rate_Device(VkPhysicalDeviceProperties physical_device_properties);

        public:
            Physical_Device(std::shared_ptr<Instance> instance_ptr, std::shared_ptr<Queue_Manager> queue_manager_ptr, std::shared_ptr<Surface> surface_ptr);

        public:
            VkPhysicalDevice* Get_Physical_Device();
            std::vector<const char*> Get_Required_Extensions();
        };
    } // namespace Vulkan
} // namespace Cascade_Graphics