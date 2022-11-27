#pragma once

#include "physical_device.hpp"
#include "vulkan_header.hpp"
#include <vector>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Queue_Selector
        {
        private:
            struct Queue_Family_Filter_Info
            {
                uint32_t queue_family_index;
                VkQueueFamilyProperties queue_family_properties;
            };

        private:
            std::vector<Queue_Family_Filter_Info> m_queue_filter_infos;

            Physical_Device* m_physical_device_ptr;

        public:
            Queue_Selector(Physical_Device* physical_device_ptr);

        public:
            Queue_Selector& Require_Queue_Type(VkQueueFlagBits queue_type);
            bool Meets_Requirements();
        };
    } // namespace Vulkan
} // namespace Cascade_Graphics