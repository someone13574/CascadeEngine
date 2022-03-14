#pragma once

#include "../vulkan_header.hpp"
#include "physical_device_wrapper.hpp"

#include <memory>
#include <optional>

namespace CascadeGraphics
{
    namespace Vulkan
    {
        class Queue_Manager
        {
        public:
            struct Queue_Family_Indices
            {
                std::optional<unsigned int> m_graphics_index;
                std::optional<unsigned int> m_compute_index;
                std::optional<unsigned int> m_transfer_index;
                std::optional<unsigned int> m_sparse_binding_index;
                std::optional<unsigned int> m_protected_index;
            };

        private:
            Queue_Family_Indices m_queue_family_indices;
            bool m_queue_types_required[5] = {false, false, false, false, false};

        public:
            Queue_Manager(bool graphics_required, bool compute_required, bool transfer_required, bool sparse_binding_required, bool protected_required);
            ~Queue_Manager();

        public:
            bool Physical_Device_Has_Required_Queues(VkPhysicalDevice physical_device);
        };
    } // namespace Vulkan
} // namespace CascadeGraphics
