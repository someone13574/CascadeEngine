#include "synchronization_manager.hpp"

#include "debug_tools.hpp"


namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        Synchronization_Manager::Synchronization_Manager(std::shared_ptr<Logical_Device_Wrapper> logical_device_wrapper_ptr) : m_logical_device_wrapper_ptr(logical_device_wrapper_ptr)
        {
        }

        Synchronization_Manager::~Synchronization_Manager()
        {
            LOG_INFO << "Vulkan Backend: Destroying synchonization objects";

            for (uint32_t i = 0; i < m_semaphores.size(); i++)
            {
                vkDestroySemaphore(*m_logical_device_wrapper_ptr->Get_Device(), m_semaphores[i].semaphore, nullptr);
            }

            for (uint32_t i = 0; i < m_fences.size(); i++)
            {
                if (!m_fences[i].added)
                {
                    vkDestroyFence(*m_logical_device_wrapper_ptr->Get_Device(), m_fences[i].fence, nullptr);
                }
            }

            LOG_TRACE << "Vulkan Backend: Finished destroying synchronization objects";
        }

        uint32_t Synchronization_Manager::Get_Next_Semaphore_Index(std::string label)
        {
            uint32_t next_index = 0;
            for (uint32_t i = 0; i < m_semaphores.size(); i++)
            {
                if (label == m_semaphores[i].identifier.label && next_index == m_semaphores[i].identifier.index)
                {
                    next_index++;
                }
            }

            return next_index;
        }

        uint32_t Synchronization_Manager::Get_Next_Fence_Index(std::string label)
        {
            uint32_t next_index = 0;
            for (uint32_t i = 0; i < m_fences.size(); i++)
            {
                if (label == m_fences[i].identifier.label && next_index == m_fences[i].identifier.index)
                {
                    next_index++;
                }
            }

            return next_index;
        }

        Identifier Synchronization_Manager::Create_Semaphore(std::string label)
        {
            Identifier identifier = {};
            identifier.label = label;
            identifier.index = Get_Next_Semaphore_Index(label);

            LOG_INFO << "Vulkan Backend: Creating semaphore with identifier " << identifier.Get_Identifier_String();

            VkSemaphoreCreateInfo semaphore_create_info = {};
            semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            semaphore_create_info.pNext = nullptr;
            semaphore_create_info.flags = 0;

            m_semaphores.resize(m_semaphores.size() + 1);
            m_semaphores.back() = {};
            m_semaphores.back().identifier = identifier;

            VALIDATE_VKRESULT(vkCreateSemaphore(*m_logical_device_wrapper_ptr->Get_Device(), &semaphore_create_info, nullptr, &m_semaphores.back().semaphore), "Vulkan Backend: Failed to create semaphore");

            LOG_TRACE << "Vulkan Backend: Finished creating semaphore";

            return identifier;
        }

        Identifier Synchronization_Manager::Create_Fence(std::string label)
        {
            Identifier identifier = {};
            identifier.label = label;
            identifier.index = Get_Next_Fence_Index(label);

            LOG_INFO << "Vulkan Backend: Creating fence with identifier " << identifier.Get_Identifier_String();

            VkFenceCreateInfo fence_create_info = {};
            fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fence_create_info.pNext = nullptr;
            fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            m_fences.resize(m_fences.size() + 1);
            m_fences.back() = {};
            m_fences.back().identifier = identifier;
            m_fences.back().added = false;

            VALIDATE_VKRESULT(vkCreateFence(*m_logical_device_wrapper_ptr->Get_Device(), &fence_create_info, nullptr, &m_fences.back().fence), "Vulkan Backend: Failed to create fence");

            LOG_TRACE << "Vulkan Backend: Finished creating fence";

            return identifier;
        }

        Identifier Synchronization_Manager::Add_Fence(std::string label, VkFence fence)
        {
            Identifier identifier = {};
            identifier.label = label;
            identifier.index = Get_Next_Fence_Index(label);

            LOG_INFO << "Vulkan Backend: Adding fence with identifier " << identifier.Get_Identifier_String();

            m_fences.resize(m_fences.size() + 1);
            m_fences.back() = {};
            m_fences.back().identifier = identifier;
            m_fences.back().fence = fence;
            m_fences.back().added = true;

            LOG_TRACE << "Vulkan Backend: Finished adding fence";

            return identifier;
        }

        void Synchronization_Manager::Destroy_Semaphore(Identifier identifier)
        {
            LOG_TRACE << "Vulkan Backend: Destroying semaphore " << identifier.Get_Identifier_String();

            for (uint32_t i = 0; i < m_semaphores.size(); i++)
            {
                if (m_semaphores[i].identifier == identifier)
                {
                    vkDestroySemaphore(*m_logical_device_wrapper_ptr->Get_Device(), m_semaphores[i].semaphore, nullptr);
                    m_semaphores.erase(m_semaphores.begin() + i);
                    return;
                }
            }

            LOG_ERROR << "Vulkan Backend: Cannot find semaphore with identifier " << identifier.Get_Identifier_String();
            exit(EXIT_FAILURE);
        }

        void Synchronization_Manager::Destroy_Fence(Identifier identifier)
        {
            LOG_TRACE << "Vulkan Backend: Destroying fence " << identifier.Get_Identifier_String();

            for (uint32_t i = 0; i < m_fences.size(); i++)
            {
                if (m_fences[i].identifier == identifier)
                {
                    if (!m_fences[i].added)
                    {
                        vkDestroyFence(*m_logical_device_wrapper_ptr->Get_Device(), m_fences[i].fence, nullptr);
                    }
                    m_fences.erase(m_fences.begin() + i);
                    return;
                }
            }

            LOG_ERROR << "Vulkan Backend: Cannot find fence with identifier " << identifier.Get_Identifier_String();
            exit(EXIT_FAILURE);
        }

        VkSemaphore* Synchronization_Manager::Get_Semaphore(Identifier identifier)
        {
            for (uint32_t i = 0; i < m_semaphores.size(); i++)
            {
                if (m_semaphores[i].identifier == identifier)
                {
                    return &m_semaphores[i].semaphore;
                }
            }

            LOG_ERROR << "Vulkan Backend: Cannot find semaphore with identifier " << identifier.Get_Identifier_String();
            exit(EXIT_FAILURE);
        }

        VkFence* Synchronization_Manager::Get_Fence(Identifier identifier)
        {
            for (uint32_t i = 0; i < m_fences.size(); i++)
            {
                if (m_fences[i].identifier == identifier)
                {
                    return &m_fences[i].fence;
                }
            }

            LOG_ERROR << "Vulkan Backend: Cannot find fence with identifier " << identifier.Get_Identifier_String();
            exit(EXIT_FAILURE);
        }
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics