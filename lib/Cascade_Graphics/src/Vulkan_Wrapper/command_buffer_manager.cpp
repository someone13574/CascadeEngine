#include "command_buffer_manager.hpp"

#include "debug_tools.hpp"


namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        Command_Buffer_Manager::Command_Buffer_Manager(std::shared_ptr<Descriptor_Set_Manager> descriptor_set_manager,
                                                       std::shared_ptr<Logical_Device_Wrapper> logical_device_wrapper_ptr,
                                                       std::shared_ptr<Pipeline_Manager> pipeline_manager_ptr,
                                                       std::shared_ptr<Storage_Manager> storage_manager_ptr)
            : m_descriptor_set_manager(descriptor_set_manager), m_logical_device_wrapper_ptr(logical_device_wrapper_ptr), m_pipeline_manager_ptr(pipeline_manager_ptr), m_storage_manager_ptr(storage_manager_ptr)
        {
        }

        Command_Buffer_Manager::~Command_Buffer_Manager()
        {
            LOG_INFO << "Vulkan Backend: Destroying command pools";

            for (uint32_t i = 0; i < m_command_pools.size(); i++)
            {
                vkDestroyCommandPool(*m_logical_device_wrapper_ptr->Get_Device(), m_command_pools[i].command_pool, nullptr);
            }

            LOG_TRACE << "Vulkan Backend: Finished destroying command pools";
        }

        void Command_Buffer_Manager::Create_Command_Pool(uint32_t queue_family)
        {
            LOG_INFO << "Vulkan Backend: Creating command pool with queue family " << queue_family;

            m_command_pools.resize(m_command_pools.size() + 1);
            m_command_pools.back() = {};
            m_command_pools.back().queue_family = queue_family;

            VkCommandPoolCreateInfo command_pool_create_info = {};
            command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            command_pool_create_info.pNext = nullptr;
            command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            command_pool_create_info.queueFamilyIndex = queue_family;

            VALIDATE_VKRESULT(vkCreateCommandPool(*m_logical_device_wrapper_ptr->Get_Device(), &command_pool_create_info, nullptr, &m_command_pools.back().command_pool), "Vulkan Backend: Failed to create command pool");

            LOG_TRACE << "Vulkan Backend: Finished creating command pool";
        }

        void Command_Buffer_Manager::Allocate_Command_Buffer(uint32_t command_buffer_index, uint32_t command_pool_index)
        {
            LOG_INFO << "Vulkan Backend: Allocating command buffer";

            VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
            command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            command_buffer_allocate_info.pNext = nullptr;
            command_buffer_allocate_info.commandPool = m_command_pools[command_pool_index].command_pool;
            command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            command_buffer_allocate_info.commandBufferCount = 1;

            VALIDATE_VKRESULT(vkAllocateCommandBuffers(*m_logical_device_wrapper_ptr->Get_Device(), &command_buffer_allocate_info, &m_command_buffers[command_buffer_index].command_buffer), "Vulkan Backend: Failed to allocate command buffer");

            LOG_TRACE << "Vulkan Backend: Finished allocating command buffer";
        }

        uint32_t Command_Buffer_Manager::Get_Command_Buffer_Index(Identifier identifier)
        {
            for (uint32_t i = 0; i < m_command_buffers.size(); i++)
            {
                if (m_command_buffers[i].identifier == identifier)
                {
                    return i;
                }
            }

            LOG_ERROR << "Vulkan Backend: No command buffer with identifier " << identifier.Get_Identifier_String() << " exists";
            exit(EXIT_FAILURE);
        }

        uint32_t Command_Buffer_Manager::Get_Next_Index(std::string label)
        {
            uint32_t next_index = 0;
            for (uint32_t i = 0; i < m_command_buffers.size(); i++)
            {
                if (label == m_command_buffers[i].identifier.label && next_index == m_command_buffers[i].identifier.index)
                {
                    next_index++;
                }
            }

            return next_index;
        }

        Identifier Command_Buffer_Manager::Add_Command_Buffer(std::string label, uint32_t queue_family, std::vector<Identifier> resource_group_identifiers, Identifier pipeline_identifier)
        {
            Identifier identifer = {};
            identifer.label = label;
            identifer.index = Get_Next_Index(label);

            LOG_INFO << "Vulkan Backend: Adding command buffer with identifier " << identifer.Get_Identifier_String();

            int32_t command_pool_index = -1;
            for (uint32_t i = 0; i < m_command_pools.size(); i++)
            {
                if (m_command_pools[i].queue_family == queue_family)
                {
                    LOG_TRACE << "Vulkan Backend: Using pre-existing command pool";

                    command_pool_index = i;
                }
            }

            if (command_pool_index == -1)
            {
                LOG_TRACE << "Vulkan Backend: Could not find pre-existing command pool";

                command_pool_index = static_cast<uint32_t>(m_command_pools.size());
                Create_Command_Pool(queue_family);
            }

            m_command_buffers.resize(m_command_buffers.size() + 1);
            m_command_buffers.back() = {};
            m_command_buffers.back().identifier = identifer;
            m_command_buffers.back().pipeline_identifier = pipeline_identifier;
            m_command_buffers.back().resource_group_identifiers = resource_group_identifiers;

            Allocate_Command_Buffer(static_cast<uint32_t>(m_command_buffers.size() - 1), command_pool_index);

            std::vector<Identifier> image_identifiers;
            for (uint32_t i = 0; i < resource_group_identifiers.size(); i++)
            {
                std::vector<Identifier> resource_group_image_identifiers = m_storage_manager_ptr->Get_Resource_Grouping(resource_group_identifiers[i])->image_identifiers;

                image_identifiers.insert(image_identifiers.end(), resource_group_image_identifiers.begin(), resource_group_image_identifiers.end());
            }

            for (uint32_t i = 0; i < image_identifiers.size(); i++)
            {
                m_command_buffers.back().image_resource_states.resize(m_command_buffers.back().image_resource_states.size() + 1);

                m_command_buffers.back().image_resource_states.back() = {};
                m_command_buffers.back().image_resource_states.back().identifier = image_identifiers[i];
                m_command_buffers.back().image_resource_states.back().previous_access_flags = 0;
                m_command_buffers.back().image_resource_states.back().current_access_flags = 0;
                m_command_buffers.back().image_resource_states.back().previous_image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
                m_command_buffers.back().image_resource_states.back().current_image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
                m_command_buffers.back().image_resource_states.back().previous_pipeline_stage_flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                m_command_buffers.back().image_resource_states.back().current_pipeline_stage_flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            }

            LOG_TRACE << "Vulkan Backend: Added command buffer";

            return identifer;
        }

        void Command_Buffer_Manager::Remove_Command_Buffer(Identifier identifier)
        {
            LOG_TRACE << "Vulkan Backend: Removing command buffer " << identifier.Get_Identifier_String();

            uint32_t command_buffer_index = Get_Command_Buffer_Index(identifier);

            m_command_buffers.erase(m_command_buffers.begin() + command_buffer_index);
        }

        void Command_Buffer_Manager::Reset_Command_Buffer(Identifier identifier)
        {
            uint32_t command_buffer_index = Get_Command_Buffer_Index(identifier);

            VALIDATE_VKRESULT(vkResetCommandBuffer(m_command_buffers[command_buffer_index].command_buffer, 0), "Vulkan Backend: Failed to reset command buffer");
        }

        void Command_Buffer_Manager::Begin_Recording(Identifier identifier, VkCommandBufferUsageFlagBits usage_flags)
        {
            LOG_INFO << "Vulkan Backend: Starting recording of command buffer " << identifier.Get_Identifier_String();

            uint32_t command_buffer_index = Get_Command_Buffer_Index(identifier);

            VkCommandBufferBeginInfo command_buffer_begin_info = {};
            command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            command_buffer_begin_info.pNext = nullptr;
            command_buffer_begin_info.flags = usage_flags;
            command_buffer_begin_info.pInheritanceInfo = nullptr;

            VALIDATE_VKRESULT(vkBeginCommandBuffer(m_command_buffers[command_buffer_index].command_buffer, &command_buffer_begin_info), "Vulkan Backend: Failed to begin command buffer recording");

            LOG_TRACE << "Vulkan Backend: Started command buffer recording";

            if (!(m_command_buffers[command_buffer_index].pipeline_identifier.label == "" && m_command_buffers[command_buffer_index].pipeline_identifier.index == 0))
            {
                LOG_TRACE << "Vulkan Backend: Binding pipeline";

                switch (m_pipeline_manager_ptr->Get_Pipeline_Data(m_command_buffers[command_buffer_index].pipeline_identifier)->type)
                {
                    case Pipeline_Manager::Pipeline_Type::COMPUTE:
                    {
                        vkCmdBindPipeline(m_command_buffers[command_buffer_index].command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_pipeline_manager_ptr->Get_Pipeline_Data(m_command_buffers[command_buffer_index].pipeline_identifier)->pipeline);
                        break;
                    }
                    default:
                    {
                        LOG_ERROR << "Vulkan Backend: Unknown pipeline type";
                        exit(EXIT_FAILURE);
                    }
                }

                LOG_TRACE << "Vulkan Backend: Binding descriptor set";

                switch (m_pipeline_manager_ptr->Get_Pipeline_Data(m_command_buffers[command_buffer_index].pipeline_identifier)->type)
                {
                    case Pipeline_Manager::Pipeline_Type::COMPUTE:
                    {
                        for (uint32_t i = 0; i < m_command_buffers[command_buffer_index].resource_group_identifiers.size(); i++)
                        {
                            if (m_storage_manager_ptr->Get_Resource_Grouping(m_command_buffers[command_buffer_index].resource_group_identifiers[i])->has_descriptor_set)
                            {
                                vkCmdBindDescriptorSets(m_command_buffers[command_buffer_index].command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE,
                                                        m_pipeline_manager_ptr->Get_Pipeline_Data(m_command_buffers[command_buffer_index].pipeline_identifier)->pipeline_layout, 0, 1,
                                                        &m_descriptor_set_manager->Get_Descriptor_Set_Data(m_command_buffers[command_buffer_index].resource_group_identifiers[i])->descriptor_set, 0, nullptr);
                            }
                        }
                        break;
                    }
                    default:
                    {
                        LOG_ERROR << "Vulkan Backend: Unknown pipeline type";
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }

        void Command_Buffer_Manager::End_Recording(Identifier identifier)
        {
            LOG_INFO << "Vulkan Backend: Ending recording of command buffer " << identifier.Get_Identifier_String();

            uint32_t command_buffer_index = Get_Command_Buffer_Index(identifier);

            vkEndCommandBuffer(m_command_buffers[command_buffer_index].command_buffer);
        }

        void Command_Buffer_Manager::Image_Memory_Barrier(Identifier identifier, Identifier resource_identifier, VkAccessFlags access_flags, VkImageLayout image_layout, VkPipelineStageFlags pipeline_stage_flags)
        {
            LOG_TRACE << "Vulkan Backend: Image memory barrier in command buffer " << identifier.Get_Identifier_String();

            uint32_t command_buffer_index = Get_Command_Buffer_Index(identifier);

            int32_t resource_index = -1;
            for (uint32_t i = 0; i < m_command_buffers[command_buffer_index].image_resource_states.size(); i++)
            {
                if (m_command_buffers[command_buffer_index].image_resource_states[i].identifier == resource_identifier)
                {
                    resource_index = i;
                }
            }

            if (resource_index == -1)
            {
                LOG_ERROR << "Vulkan Backend: No image resources with the identifier " << resource_identifier.Get_Identifier_String() << " exists in provided resource groupings";
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
            image_memory_barrier.image = m_storage_manager_ptr->Get_Image_Resource(resource_identifier)->image;
            image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_memory_barrier.subresourceRange.baseMipLevel = 0;
            image_memory_barrier.subresourceRange.levelCount = 1;
            image_memory_barrier.subresourceRange.baseArrayLayer = 0;
            image_memory_barrier.subresourceRange.layerCount = 1;

            vkCmdPipelineBarrier(m_command_buffers[command_buffer_index].command_buffer, m_command_buffers[command_buffer_index].image_resource_states[resource_index].previous_pipeline_stage_flags,
                                 m_command_buffers[command_buffer_index].image_resource_states[resource_index].current_pipeline_stage_flags, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);
        }

        void Command_Buffer_Manager::Dispatch_Compute_Shader(Identifier identifier, uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z)
        {
            LOG_TRACE << "Vulkan Backend: Dispatching compute shader in command buffer " << identifier.Get_Identifier_String();

            vkCmdDispatch(m_command_buffers[Get_Command_Buffer_Index(identifier)].command_buffer, group_count_x, group_count_y, group_count_z);
        }

        void Command_Buffer_Manager::Copy_Image(Identifier identifier, Identifier source_resource_identifier, Identifier destination_resource_identifier, uint32_t width, uint32_t height)
        {
            LOG_TRACE << "Vulkan Backend: Copying image " << source_resource_identifier.Get_Identifier_String() << " to " << destination_resource_identifier.Get_Identifier_String() << " in command buffer " << identifier.Get_Identifier_String();
            ;

            uint32_t command_buffer_index = Get_Command_Buffer_Index(identifier);

            int32_t source_index = -1;
            int32_t destination_index = -1;
            for (uint32_t i = 0; i < m_command_buffers[command_buffer_index].image_resource_states.size(); i++)
            {
                if (m_command_buffers[command_buffer_index].image_resource_states[i].identifier == source_resource_identifier)
                {
                    source_index = i;
                }
                if (m_command_buffers[command_buffer_index].image_resource_states[i].identifier == destination_resource_identifier)
                {
                    destination_index = i;
                }
            }

            if (source_index == -1 || destination_index == -1)
            {
                LOG_ERROR << "Vulkan Backend: Cannot copy to image not added to command buffer";
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

            vkCmdCopyImage(m_command_buffers[command_buffer_index].command_buffer, m_storage_manager_ptr->Get_Image_Resource(source_resource_identifier)->image,
                           m_command_buffers[command_buffer_index].image_resource_states[source_index].current_image_layout, m_storage_manager_ptr->Get_Image_Resource(destination_resource_identifier)->image,
                           m_command_buffers[command_buffer_index].image_resource_states[destination_index].current_image_layout, 1, &copy_region);
        }

        void Command_Buffer_Manager::Copy_Buffer(Identifier identifier, Identifier source_resource_identifier, Identifier destination_resource_identifier, VkDeviceSize src_offset, VkDeviceSize dst_offset, VkDeviceSize copy_size)
        {
            LOG_TRACE << "Vulkan Backend: Copying buffer " << source_resource_identifier.Get_Identifier_String() << " to " << destination_resource_identifier.Get_Identifier_String() << " in command buffer " << identifier.Get_Identifier_String();


            uint32_t command_buffer_index = Get_Command_Buffer_Index(identifier);

            VkBufferCopy buffer_copy_data = {};
            buffer_copy_data.srcOffset = src_offset;
            buffer_copy_data.dstOffset = dst_offset;
            buffer_copy_data.size = copy_size;

            vkCmdCopyBuffer(m_command_buffers[command_buffer_index].command_buffer, m_storage_manager_ptr->Get_Buffer_Resource(source_resource_identifier)->buffer, m_storage_manager_ptr->Get_Buffer_Resource(destination_resource_identifier)->buffer,
                            1, &buffer_copy_data);
        }

        VkCommandBuffer* Command_Buffer_Manager::Get_Command_Buffer(Identifier identifier)
        {
            return &m_command_buffers[Get_Command_Buffer_Index(identifier)].command_buffer;
        }
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics