#include "synchronization_manager.hpp"

#include "../debug_tools.hpp"

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Synchronization_Manager::Synchronization_Manager(std::shared_ptr<Device> logical_device_ptr) : m_logical_device_ptr(logical_device_ptr)
        {
        }

        Synchronization_Manager::~Synchronization_Manager()
        {
            LOG_INFO << "Vulkan: Destroying synchonization objects";

            for (unsigned int i = 0; i < m_semaphores.size(); i++)
            {
                vkDestroySemaphore(*(m_logical_device_ptr->Get_Device()), m_semaphores[i].semaphore, nullptr);
            }

            for (unsigned int i = 0; i < m_fences.size(); i++)
            {
                if (!m_fences[i].added)
                {
                    vkDestroyFence(*(m_logical_device_ptr->Get_Device()), m_fences[i].fence, nullptr);
                }
            }

            LOG_TRACE << "Vulkan: Finished destroying synchronization objects";
        }

        unsigned int Synchronization_Manager::Get_Next_Semaphore_Index(std::string label)
        {
            unsigned int next_index = 0;
            for (unsigned int i = 0; i < m_semaphores.size(); i++)
            {
                if (label == m_semaphores[i].identifier.label && next_index == m_semaphores[i].identifier.index)
                {
                    next_index++;
                }
            }

            return next_index;
        }

        unsigned int Synchronization_Manager::Get_Next_Fence_Index(std::string label)
        {
            unsigned int next_index = 0;
            for (unsigned int i = 0; i < m_fences.size(); i++)
            {
                if (label == m_fences[i].identifier.label && next_index == m_fences[i].identifier.index)
                {
                    next_index++;
                }
            }

            return next_index;
        }

        void Synchronization_Manager::Create_Semaphore(std::string label)
        {
            Identifier identifier = {};
            identifier.label = label;
            identifier.index = Get_Next_Semaphore_Index(label);

            LOG_INFO << "Vulkan: Creating semaphore with identifier '" << identifier.label << "-" << identifier.index << "'";

            VkSemaphoreCreateInfo semaphore_create_info = {};
            semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            semaphore_create_info.pNext = nullptr;
            semaphore_create_info.flags = 0;

            m_semaphores.resize(m_semaphores.size() + 1);
            m_semaphores.back() = {};
            m_semaphores.back().identifier = identifier;

            VALIDATE_VKRESULT(vkCreateSemaphore(*(m_logical_device_ptr->Get_Device()), &semaphore_create_info, nullptr, &m_semaphores.back().semaphore), "Vulkan: Failed to create semaphore");

            LOG_TRACE << "Vulkan: Finished creating semaphore";
        }

        void Synchronization_Manager::Create_Fence(std::string label)
        {
            Identifier identifier = {};
            identifier.label = label;
            identifier.index = Get_Next_Fence_Index(label);

            LOG_INFO << "Vulkan: Creating fence with identifier '" << identifier.label << "-" << identifier.index << "'";

            VkFenceCreateInfo fence_create_info = {};
            fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fence_create_info.pNext = nullptr;
            fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            m_fences.resize(m_fences.size() + 1);
            m_fences.back() = {};
            m_fences.back().identifier = identifier;
            m_fences.back().added = false;

            VALIDATE_VKRESULT(vkCreateFence(*(m_logical_device_ptr->Get_Device()), &fence_create_info, nullptr, &m_fences.back().fence), "Vulkan: Failed to create fence");

            LOG_TRACE << "Vulkan: Finished creating fence";
        }

        void Synchronization_Manager::Add_Fence(std::string label, VkFence fence)
        {
            Identifier identifier = {};
            identifier.label = label;
            identifier.index = Get_Next_Fence_Index(label);

            LOG_INFO << "Vulkan: Adding fence with identifier '" << identifier.label << "-" << identifier.index << "'";

            m_fences.resize(m_fences.size() + 1);
            m_fences.back() = {};
            m_fences.back().identifier = identifier;
            m_fences.back().fence = fence;
            m_fences.back().added = true;

            LOG_TRACE << "Vulkan: Finished adding fence";
        }

        VkSemaphore* Synchronization_Manager::Get_Semaphore(Identifier identifier)
        {
            for (unsigned int i = 0; i < m_semaphores.size(); i++)
            {
                if (m_semaphores[i].identifier == identifier)
                {
                    return &m_semaphores[i].semaphore;
                }
            }

            LOG_ERROR << "Vulkan: Cannot find semaphore with identifier '" << identifier.label << "-" << identifier.index << "'";
            exit(EXIT_FAILURE);
        }

        VkFence* Synchronization_Manager::Get_Fence(Identifier identifier)
        {
            for (unsigned int i = 0; i < m_fences.size(); i++)
            {
                if (m_fences[i].identifier == identifier)
                {
                    return &m_fences[i].fence;
                }
            }

            LOG_ERROR << "Vulkan: Cannot find fence with identifier '" << identifier.label << "-" << identifier.index << "'";
            exit(EXIT_FAILURE);
        }
    } // namespace Vulkan
} // namespace Cascade_Graphics