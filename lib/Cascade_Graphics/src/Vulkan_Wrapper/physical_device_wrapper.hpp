#pragma once

#include "instance_wrapper.hpp"
#include "queue_manager.hpp"
#include "surface_wrapper.hpp"
#include "vulkan_header.hpp"
#include <memory>
#include <set>


namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        class Queue_Manager;

        class Physical_Device_Wrapper
        {
        private:
            VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
            std::set<const char*> m_required_extensions;

            std::shared_ptr<Queue_Manager> m_queue_manager_ptr;
            std::shared_ptr<Surface> m_surface_ptr;

        private:
            bool Does_Device_Meet_Requirements(VkPhysicalDevice physical_device, VkPhysicalDeviceProperties physical_device_properties, VkPhysicalDeviceFeatures physical_device_features);
            bool Check_Device_Extension_Support(VkPhysicalDevice physical_device);

            uint32_t Rate_Physical_Device(VkPhysicalDeviceProperties physical_device_properties);

        public:
            Physical_Device_Wrapper(std::shared_ptr<Instance_Wrapper> instance_wrapper_ptr, std::shared_ptr<Queue_Manager> queue_manager_ptr, std::shared_ptr<Surface> surface_ptr, std::set<const char*> required_extensions);

        public:
            VkPhysicalDevice* Get_Physical_Device();
            std::set<const char*> Get_Required_Extensions();
        };
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics
