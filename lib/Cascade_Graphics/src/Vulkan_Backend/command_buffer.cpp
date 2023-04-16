#include "command_buffer.hpp"

#include <acorn_logging.hpp>
#include <vulkan/vk_enum_string_helper.h>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Command_Buffer::Command_Buffer(Device* device_ptr, Compute_Pipeline* compute_pipeline_ptr, uint32_t command_buffer_count, uint32_t queue_family_index) :
            m_device_ptr(device_ptr), m_pipeline_ptr(compute_pipeline_ptr)
        {
            // Create command pool
            LOG_INFO << "Graphics (Vulkan): Creating command buffer grouping with size " << command_buffer_count << " with queue family " << queue_family_index;
            LOG_TRACE << "Graphics (Vulkan): Creating command pool";

            VkCommandPoolCreateInfo command_pool_create_info = {};
            command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            command_pool_create_info.pNext = NULL;
            command_pool_create_info.flags = 0;
            command_pool_create_info.queueFamilyIndex = queue_family_index;

            VkResult command_pool_create_result = vkCreateCommandPool(device_ptr->Get(), &command_pool_create_info, NULL, &m_command_pool);
            if (command_pool_create_result != VK_SUCCESS)
            {
                LOG_FATAL << "Graphics (Vulkan): Failed to create command pool with code " << command_pool_create_result << " (" << string_VkResult(command_pool_create_result) << ")";
                exit(EXIT_FAILURE);
            }

            // Allocate command buffers
            LOG_TRACE << "Graphics (Vulkan): Allocating command buffers";

            VkCommandBufferAllocateInfo command_buffers_allocate_info = {};
            command_buffers_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            command_buffers_allocate_info.pNext = NULL;
            command_buffers_allocate_info.commandPool = m_command_pool;
            command_buffers_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            command_buffers_allocate_info.commandBufferCount = command_buffer_count;

            m_command_buffers.resize(command_buffer_count);
            VkResult command_buffers_allocate_result = vkAllocateCommandBuffers(device_ptr->Get(), &command_buffers_allocate_info, m_command_buffers.data());
            if (command_buffers_allocate_result != VK_SUCCESS)
            {
                LOG_FATAL << "Graphics (Vulkan): Failed to allocate command buffer with code " << command_buffers_allocate_result << " (" << string_VkResult(command_buffers_allocate_result) << ")";
                exit(EXIT_FAILURE);
            }

            // Start recording command buffers and bind pipeline
            LOG_TRACE << "Graphics (Vulkan): Starting recording of command buffers";

            for (uint32_t command_buffer_index = 0; command_buffer_index < command_buffer_count; command_buffer_index++)
            {
                VkCommandBufferBeginInfo command_buffer_begin_info = {};
                command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                command_buffer_begin_info.pNext = NULL;
                command_buffer_begin_info.flags = 0;
                command_buffer_begin_info.pInheritanceInfo = NULL;

                VkResult command_buffer_begin_result = vkBeginCommandBuffer(m_command_buffers[command_buffer_index], &command_buffer_begin_info);
                if (command_buffer_begin_result != VK_SUCCESS)
                {
                    LOG_FATAL << "Graphics (Vulkan): Failed to begin command buffer " << command_buffer_index << " with code " << command_buffer_begin_result << " (" << string_VkResult(command_buffer_begin_result) << ")";
                    exit(EXIT_FAILURE);
                }

                vkCmdBindPipeline(m_command_buffers[command_buffer_index], VK_PIPELINE_BIND_POINT_COMPUTE, compute_pipeline_ptr->Get());
            }

            // Initialize command_buffer_images
            m_command_buffer_images.resize(command_buffer_count);
        }

        Command_Buffer::~Command_Buffer()
        {
            LOG_TRACE << "Graphics (Vulkan): Destroying command pool";
            vkDestroyCommandPool(m_device_ptr->Get(), m_command_pool, NULL);
        }

        Command_Buffer* Command_Buffer::Bind_Descriptor_Set(Descriptor_Set* descriptor_set_ptr, int32_t command_buffer_index)
        {
            if (command_buffer_index == -1)
            {
                for (uint32_t i = 0; i < m_command_buffers.size(); i++)
                {
                    vkCmdBindDescriptorSets(m_command_buffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, m_pipeline_ptr->Get_Layout(), 0, 1, descriptor_set_ptr->Get(), 0, NULL);
                }
            }
            else
            {
                vkCmdBindDescriptorSets(m_command_buffers[command_buffer_index], VK_PIPELINE_BIND_POINT_COMPUTE, m_pipeline_ptr->Get_Layout(), 0, 1, descriptor_set_ptr->Get(), 0, NULL);
            }

            return this;
        }

        Command_Buffer* Command_Buffer::Add_Image(Image* image_ptr, int32_t command_buffer_index)
        {
            if (command_buffer_index == -1)
            {
                for (uint32_t i = 0; i < m_command_buffers.size(); i++)
                {
                    m_command_buffer_images[i].push_back(std::make_tuple(image_ptr, Command_Buffer::Image_State {0, VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT}, Command_Buffer::Image_State {0, VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT}));
                }
            }
            else
            {
                m_command_buffer_images[command_buffer_index].push_back(std::make_tuple(image_ptr, Command_Buffer::Image_State {0, VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT}, Command_Buffer::Image_State {0, VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT}));
            }

            return this;
        }

        Command_Buffer* Command_Buffer::Image_Memory_Barrier(Image* image_ptr, VkAccessFlags access_flags, VkImageLayout image_layout, VkPipelineStageFlags pipeline_stage_flags)
        {
            LOG_TRACE << "Graphics (Vulkan): Recording image memory barrier in compute shader";

            bool image_found = false;
            for (uint32_t command_buffer_index = 0; command_buffer_index < m_command_buffers.size(); command_buffer_index++)
            {
                int32_t target_image_index = -1;
                for (uint32_t image_index = 0; image_index < m_command_buffer_images[command_buffer_index].size(); image_index++)
                {
                    if (std::get<0>(m_command_buffer_images[command_buffer_index][image_index]) == image_ptr)
                    {
                        target_image_index = image_index;
                        image_found = true;
                        break;
                    }
                }
                if (target_image_index == -1)
                {
                    break;
                }

                std::get<1>(m_command_buffer_images[command_buffer_index][target_image_index]) = std::get<2>(m_command_buffer_images[command_buffer_index][target_image_index]);
                std::get<2>(m_command_buffer_images[command_buffer_index][target_image_index]).access_flags = access_flags;
                std::get<2>(m_command_buffer_images[command_buffer_index][target_image_index]).image_layout = image_layout;
                std::get<2>(m_command_buffer_images[command_buffer_index][target_image_index]).pipeline_stage_flags = pipeline_stage_flags;

                VkImageMemoryBarrier image_memory_barrier = {};
                image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                image_memory_barrier.pNext = NULL;
                image_memory_barrier.srcAccessMask = std::get<1>(m_command_buffer_images[command_buffer_index][target_image_index]).access_flags;
                image_memory_barrier.dstAccessMask = std::get<2>(m_command_buffer_images[command_buffer_index][target_image_index]).access_flags;
                image_memory_barrier.oldLayout = std::get<1>(m_command_buffer_images[command_buffer_index][target_image_index]).image_layout;
                image_memory_barrier.newLayout = std::get<2>(m_command_buffer_images[command_buffer_index][target_image_index]).image_layout;
                image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                image_memory_barrier.image = std::get<0>(m_command_buffer_images[command_buffer_index][target_image_index])->Get();
                image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                image_memory_barrier.subresourceRange.baseMipLevel = 0;
                image_memory_barrier.subresourceRange.levelCount = 1;
                image_memory_barrier.subresourceRange.baseArrayLayer = 0;
                image_memory_barrier.subresourceRange.layerCount = 1;

                vkCmdPipelineBarrier(m_command_buffers[command_buffer_index], std::get<1>(m_command_buffer_images[command_buffer_index][target_image_index]).pipeline_stage_flags, std::get<2>(m_command_buffer_images[command_buffer_index][target_image_index]).pipeline_stage_flags, 0, 0, NULL, 0, NULL, 1, &image_memory_barrier);
            }

            if (!image_found)
            {
                LOG_WARN << "Graphics (Vulkan): A memory barrier was created for an unadded image";
            }

            return this;
        }

        Command_Buffer* Command_Buffer::Dispatch_Compute_Shader(uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z)
        {
            for (uint32_t command_buffer_index = 0; command_buffer_index < m_command_buffers.size(); command_buffer_index++)
            {
                LOG_TRACE << "Graphics (Vulkan): Recording compute shader dispatch for buffer " << command_buffer_index;

                vkCmdDispatch(m_command_buffers[command_buffer_index], group_count_x, group_count_y, group_count_z);
            }

            return this;
        }

        Command_Buffer* Command_Buffer::Copy_Image(Image* src_image_ptr, Image* dst_image_ptr, uint32_t width, uint32_t height)
        {
            LOG_TRACE << "Graphics (Vulkan): Recording image copy operation";

            bool images_found = false;
            for (uint32_t command_buffer_index = 0; command_buffer_index < m_command_buffers.size(); command_buffer_index++)
            {
                int32_t src_image_index = -1;
                int32_t dst_image_index = -1;
                for (uint32_t image_index = 0; image_index < m_command_buffer_images[command_buffer_index].size(); image_index++)
                {
                    if (std::get<0>(m_command_buffer_images[command_buffer_index][image_index]) == src_image_ptr)
                    {
                        src_image_index = image_index;
                    }
                    if (std::get<0>(m_command_buffer_images[command_buffer_index][image_index]) == dst_image_ptr)
                    {
                        dst_image_index = image_index;
                        break;
                    }
                }
                if (src_image_index == -1 || dst_image_index == -1)
                {
                    break;
                }
                else
                {
                    images_found = true;
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

                vkCmdCopyImage(m_command_buffers[command_buffer_index], std::get<0>(m_command_buffer_images[command_buffer_index][src_image_index])->Get(), std::get<2>(m_command_buffer_images[command_buffer_index][src_image_index]).image_layout, std::get<0>(m_command_buffer_images[command_buffer_index][dst_image_index])->Get(), std::get<2>(m_command_buffer_images[command_buffer_index][dst_image_index]).image_layout, 1, &copy_region);
            }

            if (!images_found)
            {
                LOG_WARN << "Graphics (Vulkan): The source or destionation image could not be found for copy operation recording";
            }

            return this;
        }

        Command_Buffer* Command_Buffer::Finish_Recording()
        {
            for (uint32_t command_buffer_index = 0; command_buffer_index < m_command_buffers.size(); command_buffer_index++)
            {
                LOG_TRACE << "Graphics (Vulkan): Finishing recording for buffer " << command_buffer_index;

                VkResult end_command_buffer_result = vkEndCommandBuffer(m_command_buffers[command_buffer_index]);
                if (end_command_buffer_result != VK_SUCCESS)
                {
                    LOG_FATAL << "Graphics (Vulkan): Failed to end recording of command buffer " << command_buffer_index;
                    exit(EXIT_FAILURE);
                }
            }

            return this;
        }

        VkCommandBuffer* Command_Buffer::Get(uint32_t index)
        {
            return &m_command_buffers[index];
        }
    }    // namespace Vulkan
}    // namespace Cascade_Graphics