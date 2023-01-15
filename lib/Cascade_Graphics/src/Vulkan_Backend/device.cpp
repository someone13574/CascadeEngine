#include "device.hpp"

#include <acorn_logging.hpp>
#include <bits/stdc++.h>
#include <vulkan/vk_enum_string_helper.h>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Device::Device(Physical_Device* physical_device_ptr) :
            m_physical_device_ptr(physical_device_ptr)
        {
            LOG_INFO << "Graphics (Vulkan): Creating device from physical device '" << physical_device_ptr->Get_Properties()->deviceName << "'";

            m_device_queues = physical_device_ptr->Get_Device_Queues();
            std::vector<VkDeviceQueueCreateInfo> device_queue_create_informations = Get_Queue_Create_Information();
            std::vector<const char*> enabled_device_extensions = physical_device_ptr->Get_Device_Extensions();

            VkDeviceCreateInfo device_create_info = {};
            device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            device_create_info.pNext = NULL;
            device_create_info.flags = 0;
            device_create_info.queueCreateInfoCount = static_cast<uint32_t>(device_queue_create_informations.size());
            device_create_info.pQueueCreateInfos = device_queue_create_informations.data();
            device_create_info.enabledLayerCount = 0;
            device_create_info.ppEnabledLayerNames = NULL;
            device_create_info.enabledExtensionCount = static_cast<uint32_t>(enabled_device_extensions.size());
            device_create_info.ppEnabledExtensionNames = enabled_device_extensions.data();
            device_create_info.pEnabledFeatures = NULL;

            // Create device
            VkResult create_device_result = vkCreateDevice(*physical_device_ptr->Get(), &device_create_info, NULL, &m_device);
            if (create_device_result != VK_SUCCESS)
            {
                LOG_FATAL << "Graphics (Vulkan): Failed to create device with VkResult " << create_device_result << " (" << string_VkResult(create_device_result) << ")";
                exit(EXIT_FAILURE);
            }

            // Get device queue handles
            for (uint32_t requirement_index = 0; requirement_index < m_device_queues.device_queue_requirements.size(); requirement_index++)
            {
                for (uint32_t queue_index = 0; queue_index < m_device_queues.device_queue_requirements[requirement_index].device_queues.size(); queue_index++)
                {
                    Device_Queue* device_queue_ptr = &m_device_queues.device_queue_requirements[requirement_index].device_queues[queue_index];
                    vkGetDeviceQueue(m_device, device_queue_ptr->queue_family_index, device_queue_ptr->index_in_queue_family, &device_queue_ptr->queue);
                }
            }
        }

        Device::~Device()
        {
            LOG_DEBUG << "Graphics (Vulkan): Destroying device";

            vkDestroyDevice(m_device, NULL);
        }

        std::vector<VkDeviceQueueCreateInfo> Device::Get_Queue_Create_Information()
        {
            LOG_TRACE << "Graphics (Vulkan): Generating device queue create information";

            std::vector<VkDeviceQueueCreateInfo> device_queue_create_informations;

            for (uint32_t queue_family_index = 0; queue_family_index < m_device_queues.queue_family_usage.size(); queue_family_index++)
            {
                if (m_device_queues.queue_family_usage[queue_family_index] != 0)
                {
                    VkDeviceQueueCreateInfo device_queue_create_info = {};
                    device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                    device_queue_create_info.pNext = NULL;
                    device_queue_create_info.flags = 0;
                    device_queue_create_info.queueFamilyIndex = queue_family_index;
                    device_queue_create_info.queueCount = 1;
                    device_queue_create_info.pQueuePriorities = m_device_queues.queue_priorities[queue_family_index].data();

                    device_queue_create_informations.push_back(device_queue_create_info);
                }
            }

            return device_queue_create_informations;
        }

        VkDeviceMemory Device::Allocate_Buffer_Memory(VkBuffer* buffer_ptr, VkMemoryPropertyFlags required_memory_properties, VkMemoryPropertyFlags preferred_memory_properties)
        {
            LOG_TRACE << "Graphics (Vulkan): Allocating buffer memory";

            // Get physical device memory properties
            VkPhysicalDeviceMemoryBudgetPropertiesEXT memory_budget_properties = {};
            memory_budget_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT;
            memory_budget_properties.pNext = NULL;

            VkPhysicalDeviceMemoryProperties2 memory_properties = {};
            memory_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
            memory_properties.pNext = &memory_budget_properties;

            vkGetPhysicalDeviceMemoryProperties2(*m_physical_device_ptr->Get(), &memory_properties);

            // Get buffer memory requirements
            VkMemoryDedicatedRequirements memory_dedicated_requirements = {};
            memory_dedicated_requirements.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS;
            memory_dedicated_requirements.pNext = NULL;

            VkMemoryRequirements2 memory_requirements = {};
            memory_requirements.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
            memory_requirements.pNext = &memory_dedicated_requirements;

            VkBufferMemoryRequirementsInfo2 buffer_memory_requirements = {};
            buffer_memory_requirements.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2;
            buffer_memory_requirements.pNext = NULL;
            buffer_memory_requirements.buffer = *buffer_ptr;

            vkGetBufferMemoryRequirements2(m_device, &buffer_memory_requirements, &memory_requirements);

            // Select best memory type
            int32_t best_memory_type_index = -1;
            int32_t best_memory_type_score = -1;
            VkDeviceSize best_memory_type_remaining_space = 0;

            for (uint32_t memory_type_index = 0; memory_type_index < memory_properties.memoryProperties.memoryTypeCount; memory_type_index++)
            {
                // Check if the memory type is supported for this resource
                if (!(memory_requirements.memoryRequirements.memoryTypeBits & (1 << memory_type_index)))
                {
                    continue;
                }

                // Check that all required properties are present
                if ((memory_properties.memoryProperties.memoryTypes[memory_type_index].propertyFlags & required_memory_properties) != required_memory_properties)
                {
                    continue;
                }

                // Check that there is sufficient memory budget remaining
                VkDeviceSize remaining_heap_budget = memory_budget_properties.heapBudget[memory_properties.memoryProperties.memoryTypes[memory_type_index].heapIndex];
                if (remaining_heap_budget < memory_requirements.memoryRequirements.size)
                {
                    continue;
                }

                // Rate memory type based on properties
                int32_t memory_type_properties_score = std::bitset<32>(memory_properties.memoryProperties.memoryTypes[memory_type_index].propertyFlags & preferred_memory_properties).count();
                if (memory_type_properties_score < best_memory_type_score)
                {
                    continue;
                }
                else if (memory_type_properties_score == best_memory_type_score)
                {
                    // Break tie with remaining heap space
                    if (remaining_heap_budget < best_memory_type_remaining_space)
                    {
                        continue;
                    }
                }

                // Select this memory type as new best
                best_memory_type_index = memory_type_index;
                best_memory_type_score = memory_type_properties_score;
                best_memory_type_remaining_space = remaining_heap_budget;
            }

            // Check that a valid memory type was found
            if (best_memory_type_index == -1)
            {
                LOG_FATAL << "Graphics (Vulkan): A memory type meeting the required property flags of " << string_VkMemoryPropertyFlags(required_memory_properties) << " was not found";
                exit(EXIT_FAILURE);
            }

            // Select dedicated or normal allocation
            if (memory_dedicated_requirements.prefersDedicatedAllocation || memory_dedicated_requirements.requiresDedicatedAllocation)
            {
                LOG_TRACE << "Graphics (Vulkan): Using dedicated allocation";

                // Dedicated allocation
                VkMemoryDedicatedAllocateInfo memory_dedicated_allocate_info = {};
                memory_dedicated_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO;
                memory_dedicated_allocate_info.pNext = NULL;
                memory_dedicated_allocate_info.image = VK_NULL_HANDLE;
                memory_dedicated_allocate_info.buffer = *buffer_ptr;

                VkMemoryAllocateInfo memory_allocate_info = {};
                memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                memory_allocate_info.pNext = &memory_dedicated_allocate_info;
                memory_allocate_info.allocationSize = memory_requirements.memoryRequirements.size;
                memory_allocate_info.memoryTypeIndex = best_memory_type_index;

                VkDeviceMemory device_memory;

                VkResult memory_allocate_result = vkAllocateMemory(m_device, &memory_allocate_info, NULL, &device_memory);
                if (memory_allocate_result != VK_SUCCESS)
                {
                    LOG_FATAL << "Graphics (Vulkan): Failed to allocate buffer memory with VkResult " << memory_allocate_result << " (" << string_VkResult(memory_allocate_result) << ")";
                    exit(EXIT_FAILURE);
                }

                LOG_TRACE << "Graphics (Vulkan): Successfully allocated buffer memory. Binding...";

                // Bind allocated memory to buffer
                VkResult bind_buffer_memory_result = vkBindBufferMemory(m_device, *buffer_ptr, device_memory, 0);
                if (bind_buffer_memory_result != VK_SUCCESS)
                {
                    LOG_FATAL << "Graphics (Vulkan): Failed to bind memory to buffer with VkResult " << bind_buffer_memory_result << " (" << string_VkResult(bind_buffer_memory_result) << ")";
                    exit(EXIT_FAILURE);
                }

                LOG_TRACE << "Graphics (Vulkan): Finished binding memory to buffer";

                return device_memory;
            }
            else
            {
                LOG_TRACE << "Graphics (Vulkan): Using standard allocation";

                // Normal allocation
                VkMemoryAllocateInfo memory_allocate_info = {};
                memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                memory_allocate_info.pNext = NULL;
                memory_allocate_info.allocationSize = memory_requirements.memoryRequirements.size;
                memory_allocate_info.memoryTypeIndex = best_memory_type_index;

                VkDeviceMemory device_memory;

                VkResult memory_allocate_result = vkAllocateMemory(m_device, &memory_allocate_info, NULL, &device_memory);
                if (memory_allocate_result != VK_SUCCESS)
                {
                    LOG_FATAL << "Graphics (Vulkan): Failed to allocate buffer memory with VkResult " << memory_allocate_result << " (" << string_VkResult(memory_allocate_result) << ")";
                    exit(EXIT_FAILURE);
                }

                LOG_TRACE << "Graphics (Vulkan): Successfully allocated buffer memory. Binding...";

                // Bind allocated memory to buffer
                VkResult bind_buffer_memory_result = vkBindBufferMemory(m_device, *buffer_ptr, device_memory, 0);
                if (bind_buffer_memory_result != VK_SUCCESS)
                {
                    LOG_FATAL << "Graphics (Vulkan): Failed to bind memory to buffer with VkResult " << bind_buffer_memory_result << " (" << string_VkResult(bind_buffer_memory_result) << ")";
                    exit(EXIT_FAILURE);
                }

                LOG_TRACE << "Graphics (Vulkan): Finished binding memory to buffer";

                return device_memory;
            }
        }

        VkDevice* Device::Get()
        {
            return &m_device;
        }

        Device_Queues* Device::Get_Device_Queues()
        {
            return &m_device_queues;
        }
    }    // namespace Vulkan
}    // namespace Cascade_Graphics