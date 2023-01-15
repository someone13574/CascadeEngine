#include "buffer.hpp"

#include <acorn_logging.hpp>
#include <algorithm>
#include <vulkan/vk_enum_string_helper.h>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Buffer::Buffer(Device* device_ptr, VkBufferUsageFlags buffer_usage_flags, VkDeviceSize buffer_size, std::vector<Device_Queue_Requirement*> queue_requirements_with_access, VkMemoryPropertyFlags required_memory_properties, VkMemoryPropertyFlags preferred_memory_properties) :
            m_device_ptr(device_ptr)
        {
            LOG_DEBUG << "Graphics (Vulkan): Creating buffer";

            // Get queues which require access to the swapchain
            std::vector<uint32_t> buffer_access_queue_families;
            for (uint32_t queue_requirement_index = 0; queue_requirement_index < queue_requirements_with_access.size(); queue_requirement_index++)
            {
                for (uint32_t queue_index = 0; queue_index < queue_requirements_with_access[queue_requirement_index]->device_queues.size(); queue_index++)
                {
                    buffer_access_queue_families.push_back(queue_requirements_with_access[queue_requirement_index]->device_queues[queue_index].queue_family_index);
                }
            }

            std::sort(buffer_access_queue_families.begin(), buffer_access_queue_families.end());
            buffer_access_queue_families.erase(std::unique(buffer_access_queue_families.begin(), buffer_access_queue_families.end()), buffer_access_queue_families.end());

            // Create Buffer
            VkBufferCreateInfo buffer_create_info = {};
            buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_create_info.pNext = NULL;
            buffer_create_info.flags = 0;
            buffer_create_info.size = buffer_size;
            buffer_create_info.usage = buffer_usage_flags;
            buffer_create_info.sharingMode = (buffer_access_queue_families.size() == 1) ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
            buffer_create_info.queueFamilyIndexCount = buffer_access_queue_families.size();
            buffer_create_info.pQueueFamilyIndices = buffer_access_queue_families.data();

            VkResult create_buffer_result = vkCreateBuffer(m_device_ptr->Get(), &buffer_create_info, NULL, &m_buffer);
            if (create_buffer_result != VK_SUCCESS)
            {
                LOG_FATAL << "Graphics (Vulkan): Failed to create buffer with VkResult " << string_VkResult(create_buffer_result);
                exit(EXIT_FAILURE);
            }

            // Allocate and bind memory
            m_device_memory = m_device_ptr->Allocate_Buffer_Memory(m_buffer, required_memory_properties, preferred_memory_properties);
        }

        Buffer::~Buffer()
        {
            LOG_TRACE << "Graphics (Vulkan): Destroying buffer";

            vkDestroyBuffer(m_device_ptr->Get(), m_buffer, NULL);
            vkFreeMemory(m_device_ptr->Get(), m_device_memory, NULL);
        }
    }    // namespace Vulkan
}    // namespace Cascade_Graphics