#pragma once

#include "surface_wrapper.hpp"
#include "vulkan_header.hpp"
#include <map>
#include <memory>
#include <optional>
#include <vector>


namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        class Queue_Manager
        {
        public:
            enum Queue_Types
            {
                GRAPHICS_QUEUE = 0x01,
                COMPUTE_QUEUE = 0x02,
                TRANSFER_QUEUE = 0x04,
                SPARSE_BINDING_QUEUE = 0x08,
                PROTECTED_QUEUE = 0x10,
                PRESENT_QUEUE = 0x20
            };

            struct Queue_Family_Indices
            {
                uint32_t required_queues;

                std::optional<uint32_t> graphics_family_index;
                std::optional<uint32_t> compute_family_index;
                std::optional<uint32_t> transfer_family_index;
                std::optional<uint32_t> sparse_binding_family_index;
                std::optional<uint32_t> protected_family_index;
                std::optional<uint32_t> present_family_index;
            };

        private:
            uint32_t m_required_queues;
            Queue_Family_Indices m_queue_family_indices;
            std::map<uint32_t, VkQueue> m_queues;

            std::shared_ptr<Surface_Wrapper> m_surface_wrapper_ptr;

        public:
            Queue_Manager(std::shared_ptr<Surface_Wrapper> surface_wrapper_ptr, uint32_t required_queues);

        public:
            bool Has_Required_Queues(VkPhysicalDevice* physical_device_ptr);
            void Get_Queue_Families(VkPhysicalDevice* physical_device_ptr);
            void Get_Device_Queue_Handles(VkDevice* device_ptr);
            std::vector<VkDeviceQueueCreateInfo> Generate_Device_Queue_Create_Infos();

            VkQueue* Get_Queue(Queue_Types queue_type);
            uint32_t Get_Queue_Family_Index(Queue_Types queue_type);
            std::vector<uint32_t> Get_Unique_Queue_Families(uint32_t required_queues);
        };
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics