#pragma once

#include "../vulkan_header.hpp"
#include "device_wrapper.hpp"

#include <memory>
#include <string>
#include <vector>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Synchronization_Manager
        {
        public:
            struct Identifier
            {
                std::string label;
                unsigned int index;

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
            std::shared_ptr<Device> m_logical_device_ptr;

            std::vector<Semaphore> m_semaphores;
            std::vector<Fence> m_fences;

        private:
            unsigned int Get_Next_Semaphore_Index(std::string label);
            unsigned int Get_Next_Fence_Index(std::string label);

        public:
            Synchronization_Manager(std::shared_ptr<Device> logical_device_ptr);
            ~Synchronization_Manager();

            void Create_Semaphore(std::string label);
            void Create_Fence(std::string label);
            void Add_Fence(std::string label, VkFence fence);

            VkSemaphore* Get_Semaphore(Identifier identifier);
            VkFence* Get_Fence(Identifier identifier);
        };
    } // namespace Vulkan
} // namespace Cascade_Graphics