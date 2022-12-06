#pragma once

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