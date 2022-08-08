#include "command_buffer_manager.hpp"

#include "debug_tools.hpp"

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Command_Buffer_Manager::Command_Buffer_Manager(std::shared_ptr<Descriptor_Set_Manager> descriptor_set_manager,
                                                       std::shared_ptr<Device> logical_device_ptr,
                                                       std::shared_ptr<Pipeline_Manager> pipeline_manager_ptr,
                                                       std::shared_ptr<Storage_Manager> storage_manager_ptr)
            : m_descriptor_set_manager(descriptor_set_manager), m_logical_device_ptr(logical_device_ptr), m_storage_manager_ptr(storage_manager_ptr), m_pipeline_manager_ptr(pipeline_manager_ptr)
        {
        }

        Command_Buffer_Manager::~Command_Buffer_Manager()
        {
            LOG_INFO << "Vulkan: Destroying command pools";

            for (unsigned int i = 0; i < m_command_pools.size(); i++)
            {
                vkDestroyCommandPool(*m_logical_device_ptr->Get_Device(), m_command_pools[i].command_pool, nullptr);
            }

            LOG_TRACE << "Vulkan: Finished destroying command pools";
        }

        void Command_Buffer_Manager::Create_Command_Pool(unsigned int queue_family)
        {
            LOG_INFO << "Vulkan: Creating command pool with queue family " << queue_family;

            m_command_pools.resize(m_command_pools.size() + 1);
            m_command_pools.back() = {};
            m_command_pools.back().queue_family = queue_family;

            VkCommandPoolCreateInfo command_pool_create_info = {};
            command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            command_pool_create_info.pNext = nullptr;
            command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            command_pool_create_info.queueFamilyIndex = queue_family;

            VALIDATE_VKRESULT(vkCreateCommandPool(*m_logical_device_ptr->Get_Device(), &command_pool_create_info, nullptr, &m_command_pools.back().command_pool), "Vulkan: Failed to create command pool");

            LOG_TRACE << "Vulkan: Finished creating command pool";
        }

        void Command_Buffer_Manager::Allocate_Command_Buffer(unsigned int command_buffer_index, unsigned int command_pool_index)
        {
            LOG_INFO << "Vulkan: Allocating command buffer";

            VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
            command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            command_buffer_allocate_info.pNext = nullptr;
            command_buffer_allocate_info.commandPool = m_command_pools[command_pool_index].command_pool;
            command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            command_buffer_allocate_info.commandBufferCount = 1; // Not efficient

            VALIDATE_VKRESULT(vkAllocateCommandBuffers(*m_logical_device_ptr->Get_Device(), &command_buffer_allocate_info, &m_command_buffers[command_buffer_index].command_buffer), "Vulkan: Failed to allocate command buffer");

            LOG_TRACE << "Vulkan: Finished allocating command buffer";
        }

        unsigned int Command_Buffer_Manager::Get_Command_Buffer_Index(Identifier identifier)
        {
            for (unsigned int i = 0; i < m_command_buffers.size(); i++)
            {
                if (m_command_buffers[i].identifier == identifier)
                {
                    return i;
                }
            }

            LOG_ERROR << "Vulkan: No command buffer with identifier '" << identifier.label << "-" << identifier.index << "' exists";
            exit(EXIT_FAILURE);
        }

        unsigned int Command_Buffer_Manager::Get_Next_Index(std::string label)
        {
            unsigned int next_index = 0;
            for (unsigned int i = 0; i < m_command_buffers.size(); i++)
            {
                if (label == m_command_buffers[i].identifier.label && next_index == m_command_buffers[i].identifier.index)
                {
                    next_index++;
                }
            }

            return next_index;
        }

        void Command_Buffer_Manager::Add_Command_Buffer(std::string label, unsigned int queue_family, std::vector<std::string> resource_group_labels, std::string pipeline_label)
        {
            Identifier identifer = {};
            identifer.label = label;
            identifer.index = Get_Next_Index(label);

            LOG_INFO << "Vulkan: Adding command buffer with identifier '" << identifer.label << "-" << identifer.index << "'";

            uint32_t command_pool_index = -1;
            for (unsigned int i = 0; i < m_command_pools.size(); i++)
            {
                if (m_command_pools[i].queue_family == queue_family)
                {
                    LOG_TRACE << "Vulkan: Using pre-existing command pool";

                    command_pool_index = i;
                }
            }

            if (command_pool_index == -1)
            {
                LOG_TRACE << "Vulkan: Could not find pre-existing command pool";

                command_pool_index = static_cast<uint32_t>(m_command_pools.size());
                Create_Command_Pool(queue_family);
            }

            m_command_buffers.resize(m_command_buffers.size() + 1);
            m_command_buffers.back() = {};
            m_command_buffers.back().identifier = identifer;
            m_command_buffers.back().resource_group_labels = resource_group_labels;
            m_command_buffers.back().pipeline_label = pipeline_label;

            Allocate_Command_Buffer(static_cast<uint32_t>(m_command_buffers.size() - 1), command_pool_index);

            std::vector<Resource_ID> resource_identifiers;
            for (unsigned int i = 0; i < resource_group_labels.size(); i++)
            {
                std::vector<Resource_ID> resource_group_resources = m_storage_manager_ptr->Get_Resource_Grouping(resource_group_labels[i])->resource_ids;
                resource_identifiers.insert(resource_identifiers.end(), resource_group_resources.begin(), resource_group_resources.end());
            }

            for (unsigned int i = 0; i < resource_identifiers.size(); i++)
            {
                if (resource_identifiers[i].resource_type == Resource_ID::Resource_Type::IMAGE_RESOURCE)
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

            LOG_TRACE << "Vulkan: Added command buffer";
        }

        void Command_Buffer_Manager::Remove_Command_Buffer(Identifier identifier)
        {
            unsigned int command_buffer_index = Get_Command_Buffer_Index(identifier);

            m_command_buffers.erase(m_command_buffers.begin() + command_buffer_index);
        }

        void Command_Buffer_Manager::Reset_Command_Buffer(Identifier identifier)
        {
            unsigned int command_buffer_index = Get_Command_Buffer_Index(identifier);

            VALIDATE_VKRESULT(vkResetCommandBuffer(m_command_buffers[command_buffer_index].command_buffer, 0), "Vulkan: Failed to reset command buffer");
        }

        void Command_Buffer_Manager::Begin_Recording(Identifier identifier, VkCommandBufferUsageFlagBits usage_flags)
        {
            LOG_INFO << "Vulkan: Starting recording of command buffer '" << identifier.label << "-" << identifier.index << "'";

            unsigned int command_buffer_index = Get_Command_Buffer_Index(identifier);

            VkCommandBufferBeginInfo command_buffer_begin_info = {};
            command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            command_buffer_begin_info.pNext = nullptr;
            command_buffer_begin_info.flags = usage_flags;
            command_buffer_begin_info.pInheritanceInfo = nullptr;

            VALIDATE_VKRESULT(vkBeginCommandBuffer(m_command_buffers[command_buffer_index].command_buffer, &command_buffer_begin_info), "Vulkan: Failed to begin command buffer recording");

            LOG_TRACE << "Vulkan: Started command buffer recording";

            if (m_command_buffers[command_buffer_index].pipeline_label != "")
            {
                LOG_TRACE << "Vulkan: Binding pipeline";

                switch (m_pipeline_manager_ptr->Get_Pipeline_Type(m_command_buffers[command_buffer_index].pipeline_label))
                {
                    case Pipeline_Manager::Pipeline_Type::COMPUTE:
                    {
                        vkCmdBindPipeline(m_command_buffers[command_buffer_index].command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, *m_pipeline_manager_ptr->Get_Pipeline(m_command_buffers[command_buffer_index].pipeline_label));
                        break;
                    }
                    default:
                    {
                        LOG_ERROR << "Vulkan: Unknown pipeline type";
                        exit(EXIT_FAILURE);
                    }
                }

                LOG_TRACE << "Vulkan: Binding descriptor set";

                switch (m_pipeline_manager_ptr->Get_Pipeline_Type(m_command_buffers[command_buffer_index].pipeline_label))
                {
                    case Pipeline_Manager::Pipeline_Type::COMPUTE:
                    {
                        for (unsigned int i = 0; i < m_command_buffers[command_buffer_index].resource_group_labels.size(); i++)
                        {
                            if (m_storage_manager_ptr->Get_Resource_Grouping(m_command_buffers[command_buffer_index].resource_group_labels[i])->has_descriptor_set)
                            {
                                vkCmdBindDescriptorSets(m_command_buffers[command_buffer_index].command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, *m_pipeline_manager_ptr->Get_Pipeline_Layout(m_command_buffers[command_buffer_index].pipeline_label), 0,
                                                        1, &m_descriptor_set_manager->Get_Descriptor_Set_Data(m_command_buffers[command_buffer_index].resource_group_labels[i])->descriptor_set, 0, nullptr);
                            }
                        }
                        break;
                    }
                    default:
                    {
                        LOG_ERROR << "Vulkan: Unknown pipeline type";
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }

        void Command_Buffer_Manager::End_Recording(Identifier identifier)
        {
            LOG_INFO << "Vulkan: Ending recording of command buffer '" << identifier.label << "-" << identifier.index << "'";

            unsigned int command_buffer_index = Get_Command_Buffer_Index(identifier);

            vkEndCommandBuffer(m_command_buffers[command_buffer_index].command_buffer);
        }

        void Command_Buffer_Manager::Image_Memory_Barrier(Identifier identifier, Resource_ID resource_id, VkAccessFlags access_flags, VkImageLayout image_layout, VkPipelineStageFlags pipeline_stage_flags)
        {
            LOG_TRACE << "Vulkan: Image memory barrier in command buffer '" << identifier.label << "-" << identifier.index << "'";

            if (resource_id.resource_type != Resource_ID::IMAGE_RESOURCE)
            {
                LOG_ERROR << "Vulkan: Resource type must be an image";
                exit(EXIT_FAILURE);
            }

            unsigned int command_buffer_index = Get_Command_Buffer_Index(identifier);

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
                LOG_ERROR << "Vulkan: No image resources with the id '" << resource_id.label << "-" << resource_id.index << "' exists in provided resource groupings";
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
            image_memory_barrier.image = m_storage_manager_ptr->Get_Image_Resource(resource_id)->image;
            image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_memory_barrier.subresourceRange.baseMipLevel = 0;
            image_memory_barrier.subresourceRange.levelCount = 1;
            image_memory_barrier.subresourceRange.baseArrayLayer = 0;
            image_memory_barrier.subresourceRange.layerCount = 1;

            vkCmdPipelineBarrier(m_command_buffers[command_buffer_index].command_buffer, m_command_buffers[command_buffer_index].image_resource_states[resource_index].previous_pipeline_stage_flags,
                                 m_command_buffers[command_buffer_index].image_resource_states[resource_index].current_pipeline_stage_flags, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);
        }

        void Command_Buffer_Manager::Dispatch_Compute_Shader(Identifier identifier, unsigned int group_count_x, unsigned int group_count_y, unsigned int group_count_z)
        {
            LOG_TRACE << "Vulkan: Dispatching compute shader in command buffer '" << identifier.label << "-" << identifier.index << "'";

            vkCmdDispatch(m_command_buffers[Get_Command_Buffer_Index(identifier)].command_buffer, group_count_x, group_count_y, group_count_z);
        }

        void Command_Buffer_Manager::Copy_Image(Identifier identifier, Resource_ID source_resource_id, Resource_ID destination_resource_id, unsigned int width, unsigned int height)
        {
            LOG_TRACE << "Vulkan: Copying image " << source_resource_id.label << "-" << source_resource_id.index << " to " << destination_resource_id.label << "-" << destination_resource_id.index << " in command buffer '" << identifier.label << "-"
                      << identifier.index << "'";

            unsigned int command_buffer_index = Get_Command_Buffer_Index(identifier);

            int source_index = -1;
            int destination_index = -1;
            for (unsigned int i = 0; i < m_command_buffers[command_buffer_index].image_resource_states.size(); i++)
            {
                if (m_command_buffers[command_buffer_index].image_resource_states[i].resource_id == source_resource_id)
                {
                    source_index = i;
                }
                if (m_command_buffers[command_buffer_index].image_resource_states[i].resource_id == destination_resource_id)
                {
                    destination_index = i;
                }
            }

            if (source_index == -1 || destination_index == -1)
            {
                LOG_ERROR << "Vulkan: Cannot copy to image not added to command buffer";
                exit(EXIT_FAILURE);
            }

            VkOffset3D region_offset = {};
            region_offset.x = 0;
            region_offset.y = 0;
            region_offset.z = 0;

            VkExtent3D region_extent = {};
            region_extent.width = width;
            region_extent.height = height;
            region_extent.depth = 1;

            VkImageCopy copy_region = {};
            copy_region.srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
            copy_region.srcOffset = region_offset;
            copy_region.dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
            copy_region.dstOffset = region_offset;
            copy_region.extent = region_extent;

            vkCmdCopyImage(m_command_buffers[command_buffer_index].command_buffer, m_storage_manager_ptr->Get_Image_Resource(source_resource_id)->image, m_command_buffers[command_buffer_index].image_resource_states[source_index].current_image_layout,
                           m_storage_manager_ptr->Get_Image_Resource(destination_resource_id)->image, m_command_buffers[command_buffer_index].image_resource_states[destination_index].current_image_layout, 1, &copy_region);
        }

        void Command_Buffer_Manager::Copy_Buffer(Identifier identifier, Resource_ID source, Resource_ID destination, VkDeviceSize src_offset, VkDeviceSize dst_offset, VkDeviceSize copy_size)
        {
            LOG_TRACE << "Vulkan: Copying buffer '" << m_storage_manager_ptr->Get_Resource_String(source) << "' to '" << m_storage_manager_ptr->Get_Resource_String(destination) << "'";

            unsigned int command_buffer_index = Get_Command_Buffer_Index(identifier);

            VkBufferCopy buffer_copy_data = {};
            buffer_copy_data.srcOffset = src_offset;
            buffer_copy_data.dstOffset = dst_offset;
            buffer_copy_data.size = copy_size;

            vkCmdCopyBuffer(m_command_buffers[command_buffer_index].command_buffer, m_storage_manager_ptr->Get_Buffer_Resource(source)->buffer, m_storage_manager_ptr->Get_Buffer_Resource(destination)->buffer, 1, &buffer_copy_data);
        }

        VkCommandBuffer* Command_Buffer_Manager::Get_Command_Buffer(Identifier identifier)
        {
            return &m_command_buffers[Get_Command_Buffer_Index(identifier)].command_buffer;
        }
    } // namespace Vulkan
} // namespace Cascade_Graphics