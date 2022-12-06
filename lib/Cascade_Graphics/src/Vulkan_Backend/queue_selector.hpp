#pragma once

#include "physical_device.hpp"
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
            struct Queue_Requirement
            {
                std::string requirement_label;

                VkQueueFlagBits queue_type;
                uint32_t required_queue_count;
            };

            struct Queue_Set
            {
                std::vector<uint32_t> queue_family_usage;
                std::vector<std::vector<std::pair<uint32_t, uint32_t>>> queue_providers;
            };

        private:
            std::vector<VkQueueFamilyProperties> m_queue_families;
            std::vector<Queue_Requirement> m_queue_requirements;
            std::vector<Queue_Set> m_valid_queue_sets;

        private:
            void Generate_Sets();

        public:
            Queue_Selector(Physical_Device* physical_device_ptr);

        public:
            Queue_Selector& Add_Queue_Requirement(std::string label, VkQueueFlagBits queue_type, uint32_t required_queue_count);
            bool Meets_Requirements();
        };
    } // namespace Vulkan
} // namespace Cascade_Graphics