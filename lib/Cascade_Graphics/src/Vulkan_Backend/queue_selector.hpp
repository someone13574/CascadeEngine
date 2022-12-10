#pragma once

#include "../platform_info.hpp"
#include "physical_device.hpp"
#include "queue_data.hpp"
#include "vulkan_header.hpp"
#include <string>
#include <vector>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Queue_Selector
        {
        private:
            Platform_Info* m_platform_info_ptr;
            Physical_Device* m_pyhsical_device_ptr;

            bool m_sets_up_to_date = false;
            bool m_present_queue_required = false;

            std::vector<VkQueueFamilyProperties> m_queue_families;
            std::vector<bool> m_queue_family_present_support;

            std::vector<Queue_Requirement> m_queue_requirements;
            std::vector<Queue_Set> m_valid_queue_sets;

        private:
            void Get_Queue_Family_Present_Support();
            void Generate_Sets();

        public:
            Queue_Selector(Physical_Device* physical_device_ptr, Platform_Info* platform_info_ptr);

        public:
            Queue_Selector& Add_Queue_Requirement(std::string label, VkQueueFlagBits queue_type, uint32_t required_queue_count, float queue_priority);
            Queue_Selector& Require_Present_Queue();

            bool Meets_Requirements();
            Queue_Set Best();
        };
    } // namespace Vulkan
} // namespace Cascade_Graphics