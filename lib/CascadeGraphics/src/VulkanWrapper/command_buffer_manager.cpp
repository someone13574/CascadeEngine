#include "command_buffer_manager.hpp"

#include "../debug_tools.hpp"

namespace CascadeGraphics
{
    namespace Vulkan
    {
        Command_Buffer_Manager::Command_Buffer_Manager()
        {
        }

        void Command_Buffer_Manager::Create_Command_Pool(unsigned int queue_family)
        {
            LOG_INFO << "Vulkan: creating command pool with queue family " << queue_family;

            m_command_pools.resize(m_command_pools.size());
            m_command_pools.back() = {};
            m_command_pools.back().queue_family = queue_family;

            VkCommandPoolCreateInfo command_pool_create_info = {};
            command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            command_pool_create_info.pNext = NULL;
            command_pool_create_info.flags = 0;
            command_pool_create_info.queueFamilyIndex = queue_family;

            VALIDATE_VKRESULT(vkCreateCommandPool(*(m_logical_device_ptr->Get_Device()), &command_pool_create_info, nullptr, &m_command_pools.back().command_pool),
                              "Vulkan: failed to create command pool");

            LOG_TRACE << "Vulkan: finished creating command pool";
        }

        void Command_Buffer_Manager::Allocate_Command_Buffer(unsigned int command_buffer_index, unsigned int command_pool_index)
        {
            LOG_INFO << "Vulkan: allocating command buffer";

            VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
            command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            command_buffer_allocate_info.pNext = NULL;
            command_buffer_allocate_info.commandPool = m_command_pools[command_pool_index].command_pool;
            command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            command_buffer_allocate_info.commandBufferCount = 1; // Not efficient

            VALIDATE_VKRESULT(vkAllocateCommandBuffers(*(m_logical_device_ptr->Get_Device()), &command_buffer_allocate_info, &m_command_buffers[command_buffer_index].command_buffer),
                              "Vulkan: failed to allocate command buffer");

            LOG_TRACE << "Vulkan: finised allocating command buffer";
        }

        void Command_Buffer_Manager::Add_Command_Buffer(unsigned int queue_family)
        {
            LOG_INFO << "Vulkan: adding command buffer";

            int command_pool_index = -1;
            for (unsigned int i = 0; i < m_command_pools.size(); i++)
            {
                if (m_command_pools[i].queue_family == queue_family)
                {
                    LOG_TRACE << "Vulkan: using pre-existing command pool";

                    command_pool_index = i;
                }
            }

            if (command_pool_index == -1)
            {
                LOG_TRACE << "Vulkan: could not find pre-existing command pool";

                command_pool_index = m_command_pools.size();
                Create_Command_Pool(queue_family);
            }

            m_command_buffers.resize(m_command_buffers.size() + 1);
            m_command_buffers.back() = {};

            Allocate_Command_Buffer(m_command_buffers.size() - 1, command_pool_index);
        }
    } // namespace Vulkan
} // namespace CascadeGraphics