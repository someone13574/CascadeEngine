#include "command_buffer_manager.hpp"

#include "../debug_tools.hpp"

namespace CascadeGraphics
{
    namespace Vulkan
    {
        Command_Buffer_Manager::Command_Buffer_Manager(std::shared_ptr<Resource_Grouping_Manager> resource_grouping_manager_ptr,
                                                       std::shared_ptr<Device> logical_device_ptr,
                                                       std::shared_ptr<Pipeline_Manager> pipeline_manager_ptr,
                                                       std::shared_ptr<Storage_Manager> storage_manager_ptr)
            : m_resource_grouping_manager_ptr(resource_grouping_manager_ptr), m_logical_device_ptr(logical_device_ptr), m_storage_manager_ptr(storage_manager_ptr), m_pipeline_manager_ptr(pipeline_manager_ptr)
        {
        }

        Command_Buffer_Manager::~Command_Buffer_Manager()
        {
            LOG_INFO << "Vulkan: destroying command pools";

            for (unsigned int i = 0; i < m_command_pools.size(); i++)
            {
                vkDestroyCommandPool(*(m_logical_device_ptr->Get_Device()), m_command_pools[i].command_pool, nullptr);
            }

            LOG_TRACE << "Vulkan: finished destroying command pools";
        }

        void Command_Buffer_Manager::Create_Command_Pool(unsigned int queue_family)
        {
            LOG_INFO << "Vulkan: creating command pool with queue family " << queue_family;

            m_command_pools.resize(m_command_pools.size() + 1);
            m_command_pools.back() = {};
            m_command_pools.back().queue_family = queue_family;

            VkCommandPoolCreateInfo command_pool_create_info = {};
            command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            command_pool_create_info.pNext = nullptr;
            command_pool_create_info.flags = 0;
            command_pool_create_info.queueFamilyIndex = queue_family;

            VALIDATE_VKRESULT(vkCreateCommandPool(*(m_logical_device_ptr->Get_Device()), &command_pool_create_info, nullptr, &m_command_pools.back().command_pool), "Vulkan: failed to create command pool");

            LOG_TRACE << "Vulkan: finished creating command pool";
        }

        void Command_Buffer_Manager::Allocate_Command_Buffer(unsigned int command_buffer_index, unsigned int command_pool_index)
        {
            LOG_INFO << "Vulkan: allocating command buffer";

            VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
            command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            command_buffer_allocate_info.pNext = nullptr;
            command_buffer_allocate_info.commandPool = m_command_pools[command_pool_index].command_pool;
            command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            command_buffer_allocate_info.commandBufferCount = 1; // Not efficient

            VALIDATE_VKRESULT(vkAllocateCommandBuffers(*(m_logical_device_ptr->Get_Device()), &command_buffer_allocate_info, &m_command_buffers[command_buffer_index].command_buffer), "Vulkan: failed to allocate command buffer");

            LOG_TRACE << "Vulkan: finised allocating command buffer";
        }

        unsigned int Command_Buffer_Manager::Get_Command_Buffer_Index(std::string label)
        {
            for (unsigned int i = 0; i < m_command_buffers.size(); i++)
            {
                if (m_command_buffers[i].label == label)
                {
                    return i;
                }
            }

            LOG_ERROR << "Vulkan: no command buffer with label '" << label << "' exists";
            exit(EXIT_FAILURE);
        }

        void Command_Buffer_Manager::Add_Command_Buffer(std::string label, unsigned int queue_family, std::string resource_group_label, std::string pipeline_label)
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
            m_command_buffers.back().label = label;
            m_command_buffers.back().descriptor_set_label = resource_group_label;
            m_command_buffers.back().pipeline_label = pipeline_label;

            Allocate_Command_Buffer(m_command_buffers.size() - 1, command_pool_index);

            std::vector<Storage_Manager::Resource_ID> resource_identifiers = m_resource_grouping_manager_ptr->Get_Resources(resource_group_label);
            for (unsigned int i = 0; i < resource_identifiers.size(); i++)
            {
                if (resource_identifiers[i].type == Storage_Manager::IMAGE)
                {
                    m_command_buffers.back().image_resource_states.resize(m_command_buffers.back().image_resource_states.size() + 1);

                    m_command_buffers.back().image_resource_states.back() = {};
                    m_command_buffers.back().image_resource_states.back().resource_id = resource_identifiers[i];
                    m_command_buffers.back().image_resource_states.back().previous_access_flags = 0;
                    m_command_buffers.back().image_resource_states.back().current_access_flags = 0;
                    m_command_buffers.back().image_resource_states.back().previous_image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
                    m_command_buffers.back().image_resource_states.back().current_image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
                    m_command_buffers.back().image_resource_states.back().previous_pipeline_stage_flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                    m_command_buffers.back().image_resource_states.back().current_pipeline_stage_flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                }
            }

            LOG_TRACE << "Vulkan: added command buffer";
        }

        void Command_Buffer_Manager::Begin_Recording(std::string label, VkCommandBufferUsageFlagBits usage_flags)
        {
            LOG_INFO << "Vulkan: starting recording of command buffer " << label;

            unsigned int command_buffer_index = Get_Command_Buffer_Index(label);

            VkCommandBufferBeginInfo command_buffer_begin_info = {};
            command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            command_buffer_begin_info.pNext = nullptr;
            command_buffer_begin_info.flags = usage_flags;
            command_buffer_begin_info.pInheritanceInfo = nullptr;

            VALIDATE_VKRESULT(vkBeginCommandBuffer(m_command_buffers[command_buffer_index].command_buffer, &command_buffer_begin_info), "Vulkan: failed to begin command buffer recording");

            LOG_TRACE << "Vulkan: started command buffer recording";

            LOG_TRACE << "Vulkan: binding pipeline";

            switch (m_pipeline_manager_ptr->Get_Pipeline_Type(m_command_buffers[command_buffer_index].pipeline_label))
            {
                case Pipeline_Manager::Pipeline_Type::COMPUTE:
                {
                    vkCmdBindPipeline(m_command_buffers[command_buffer_index].command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, *m_pipeline_manager_ptr->Get_Pipeline(m_command_buffers[command_buffer_index].pipeline_label));
                    break;
                }
                default:
                {
                    LOG_ERROR << "Vulkan: unknown pipeline type";
                    exit(EXIT_FAILURE);
                }
            }

            LOG_TRACE << "Vulkan: binding descriptor set";

            switch (m_pipeline_manager_ptr->Get_Pipeline_Type(m_command_buffers[command_buffer_index].pipeline_label))
            {
                case Pipeline_Manager::Pipeline_Type::COMPUTE:
                {
                    vkCmdBindDescriptorSets(m_command_buffers[command_buffer_index].command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, *m_pipeline_manager_ptr->Get_Pipeline_Layout(m_command_buffers[command_buffer_index].pipeline_label), 0, 1,
                                            m_resource_grouping_manager_ptr->Get_Descriptor_Set(m_command_buffers[command_buffer_index].descriptor_set_label), 0, nullptr);
                    break;
                }
                default:
                {
                    LOG_ERROR << "Vulkan: unknown pipeline type";
                    exit(EXIT_FAILURE);
                }
            }
        }

        void Command_Buffer_Manager::End_Recording(std::string label)
        {
            LOG_INFO << "Vulkan: ending recording of command buffer " << label;

            unsigned int command_buffer_index = Get_Command_Buffer_Index(label);

            vkEndCommandBuffer(m_command_buffers[command_buffer_index].command_buffer);
        }

        void Command_Buffer_Manager::Image_Memory_Barrier(std::string command_buffer_label, Storage_Manager::Resource_ID resource_id, VkAccessFlags access_flags, VkImageLayout image_layout, VkPipelineStageFlags pipeline_stage_flags)
        {
            LOG_TRACE << "Vulkan: image memory barrier in command buffer '" << command_buffer_label << "'";

            if (resource_id.type != Storage_Manager::Resource_Type::IMAGE)
            {
                LOG_ERROR << "Vulkan: resource type must be an image";
                exit(EXIT_FAILURE);
            }

            unsigned int command_buffer_index = Get_Command_Buffer_Index(command_buffer_label);

            int resource_index = -1;
            for (unsigned int i = 0; i < m_command_buffers[command_buffer_index].image_resource_states.size(); i++)
            {
                if (m_command_buffers[command_buffer_index].image_resource_states[i].resource_id == resource_id)
                {
                    resource_index = i;
                }
            }

            if (resource_index == -1)
            {
                LOG_ERROR << "Vulkan: no image resources with the id '" << resource_id.label << "-" << resource_id.index << "' exist";
                exit(EXIT_FAILURE);
            }

            m_command_buffers[command_buffer_index].image_resource_states[resource_index].previous_access_flags = m_command_buffers[command_buffer_index].image_resource_states[resource_index].current_access_flags;
            m_command_buffers[command_buffer_index].image_resource_states[resource_index].previous_image_layout = m_command_buffers[command_buffer_index].image_resource_states[resource_index].current_image_layout;
            m_command_buffers[command_buffer_index].image_resource_states[resource_index].previous_pipeline_stage_flags = m_command_buffers[command_buffer_index].image_resource_states[resource_index].current_pipeline_stage_flags;

            m_command_buffers[command_buffer_index].image_resource_states[resource_index].current_access_flags = access_flags;
            m_command_buffers[command_buffer_index].image_resource_states[resource_index].current_image_layout = image_layout;
            m_command_buffers[command_buffer_index].image_resource_states[resource_index].current_pipeline_stage_flags = pipeline_stage_flags;

            VkImageMemoryBarrier image_memory_barrier = {};
            image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            image_memory_barrier.pNext = nullptr;
            image_memory_barrier.srcAccessMask = m_command_buffers[command_buffer_index].image_resource_states[resource_index].previous_access_flags;
            image_memory_barrier.dstAccessMask = m_command_buffers[command_buffer_index].image_resource_states[resource_index].current_access_flags;
            image_memory_barrier.oldLayout = m_command_buffers[command_buffer_index].image_resource_states[resource_index].previous_image_layout;
            image_memory_barrier.newLayout = m_command_buffers[command_buffer_index].image_resource_states[resource_index].current_image_layout;
            image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            image_memory_barrier.image = *m_storage_manager_ptr->Get_Image(resource_id);
            image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_memory_barrier.subresourceRange.baseMipLevel = 0;
            image_memory_barrier.subresourceRange.levelCount = 1;
            image_memory_barrier.subresourceRange.baseArrayLayer = 0;
            image_memory_barrier.subresourceRange.layerCount = 1;

            vkCmdPipelineBarrier(m_command_buffers[command_buffer_index].command_buffer, m_command_buffers[command_buffer_index].image_resource_states[resource_index].previous_pipeline_stage_flags,
                                 m_command_buffers[command_buffer_index].image_resource_states[resource_index].current_pipeline_stage_flags, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);
        }

        void Command_Buffer_Manager::Dispatch_Compute_Shader(std::string command_buffer_label, unsigned int group_count_x, unsigned int group_count_y, unsigned int group_count_z)
        {
            LOG_TRACE << "Vulkan: dispatching compute shader in command buffer " << command_buffer_label;

            vkCmdDispatch(m_command_buffers[Get_Command_Buffer_Index(command_buffer_label)].command_buffer, group_count_x, group_count_y, group_count_z);
        }
    } // namespace Vulkan
} // namespace CascadeGraphics