#pragma once


#include "surface_wrapper.hpp"
#include "vulkan_header.hpp"

#include <map>
#include <memory>
#include <optional>

namespace Cascade_Graphics
{
    namespace Vulkan
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
                unsigned int required_queues;

                std::optional<unsigned int> graphics_family_index;
                std::optional<unsigned int> compute_family_index;
                std::optional<unsigned int> transfer_family_index;
                std::optional<unsigned int> sparse_binding_family_index;
                std::optional<unsigned int> protected_family_index;
                std::optional<unsigned int> present_family_index;
            };

        private:
            std::shared_ptr<Surface> m_surface_ptr;

            unsigned int m_required_queues;
            std::map<unsigned int, VkQueue> m_queues;
            Queue_Family_Indices m_queue_family_indices;

        public:
            Queue_Manager(std::shared_ptr<Surface> surface_ptr, unsigned int required_queues);

        public:
            unsigned int Get_Required_Queue_Types();
            VkQueue* Get_Queue(Queue_Types queue_type);
            void Get_Device_Queue_Handles(VkDevice* device_ptr);
            unsigned int Get_Queue_Family_Index(Queue_Types queue_type);
            void Get_Queue_Families(VkPhysicalDevice* physical_device_ptr);
            bool Has_Required_Queues(VkPhysicalDevice* physical_device_ptr);
            std::vector<unsigned int> Get_Unique_Queue_Families(unsigned int required_queues);
            std::vector<VkDeviceQueueCreateInfo> Generate_Device_Queue_Create_Infos();
        };
    } // namespace Vulkan
} // namespace Cascade_Graphics