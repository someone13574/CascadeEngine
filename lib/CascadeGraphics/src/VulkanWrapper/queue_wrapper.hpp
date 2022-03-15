#pragma once

#include "../vulkan_header.hpp"
#include "physical_device_wrapper.hpp"

#include <memory>
#include <optional>
#include <vector>

namespace CascadeGraphics
{
    namespace Vulkan
    {
        class Physical_Device;

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
            bool m_queue_family_indices_set = false;
            Queue_Family_Indices m_queue_family_indices;
            bool m_queue_types_required[5] = {false, false, false, false, false};
            std::vector<VkQueue> m_queues;

        public:
            Queue_Manager(bool graphics_required, bool compute_required, bool transfer_required, bool sparse_binding_required, bool protected_required);
            ~Queue_Manager();

        public:
            void Set_Queue_Family_Indices(std::shared_ptr<Physical_Device> physical_device_ptr);
            void Get_Device_Queue_Handles(VkDevice* device_ptr);

            bool Physical_Device_Has_Required_Queues(VkPhysicalDevice physical_device);
            Queue_Family_Indices Get_Queue_Family_Indices();
            bool Is_Feature_Enabled(unsigned int feature_index);
            std::vector<VkDeviceQueueCreateInfo> Generate_Queue_Create_Infos();
        };
    } // namespace Vulkan
} // namespace CascadeGraphics
