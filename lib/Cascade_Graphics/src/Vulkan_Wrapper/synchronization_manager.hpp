#pragma once

#include "logical_device_wrapper.hpp"
#include "vulkan_header.hpp"
#include <memory>
#include <string>
#include <vector>


namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        class Synchronization_Manager
        {
        public:
            struct Identifier
            {
                std::string label;
                uint32_t index;

                bool operator==(Identifier other_identifier)
                {
                    return label == other_identifier.label && index == other_identifier.index;
                }
            };

        private:
            struct Semaphore
            {
                Identifier identifier;
                VkSemaphore semaphore;
            };

            struct Fence
            {
                Identifier identifier;
                VkFence fence;

                bool added;
            };

        private:
            std::vector<Semaphore> m_semaphores;
            std::vector<Fence> m_fences;

            std::shared_ptr<Logical_Device_Wrapper> m_logical_device_wrapper_ptr;

        private:
            uint32_t Get_Next_Semaphore_Index(std::string label);
            uint32_t Get_Next_Fence_Index(std::string label);

        public:
            Synchronization_Manager(std::shared_ptr<Logical_Device_Wrapper> logical_device_wrapper_ptr);
            ~Synchronization_Manager();

            void Create_Semaphore(std::string label);
            void Create_Fence(std::string label);
            void Add_Fence(std::string label, VkFence fence);

            VkSemaphore* Get_Semaphore(Identifier identifier);
            VkFence* Get_Fence(Identifier identifier);
        };
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics