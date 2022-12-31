#pragma once

#include "vulkan_header.hpp"

#include <string>
#include <vector>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        struct Device_Queue
        {
            float priority;
            uint32_t queue_family_index;
            uint32_t index_in_queue_family;

            VkQueue queue;
            VkQueueFamilyProperties queue_family_properties;
        };

        struct Device_Queue_Requirement
        {
            std::string requirement_name;
            std::vector<Device_Queue> device_queues;
        };

        struct Device_Queues
        {
            std::vector<uint32_t> queue_family_usage;
            std::vector<std::vector<float>> queue_priorities;

            std::vector<Device_Queue_Requirement> device_queue_requirements;
        };
    } // namespace Vulkan
} // namespace Cascade_Graphics