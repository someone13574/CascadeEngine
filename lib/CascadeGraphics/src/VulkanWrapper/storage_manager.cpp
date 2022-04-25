#include "storage_manager.hpp"

#include "../debug_tools.hpp"

#include <cstring>
#include <set>
#include <sstream>

namespace CascadeGraphics
{
    namespace Vulkan
    {
        Storage_Manager::Storage_Manager(std::shared_ptr<Device> logical_device_ptr, std::shared_ptr<Physical_Device> physical_device_ptr, std::shared_ptr<Queue_Manager> queue_manager_ptr)
            : m_logical_device_ptr(logical_device_ptr), m_physical_device_ptr(physical_device_ptr), m_queue_manager_ptr(queue_manager_ptr)
        {
            LOG_INFO << "Vulkan: creating storage manager";
        }

        Storage_Manager::~Storage_Manager()
        {
            LOG_INFO << "Vulkan: cleaning up storage";

            for (unsigned int i = 0; i < m_buffers.size(); i++)
            {
                LOG_TRACE << "Vulkan: destroying buffer " << m_buffers[i].label << "-" << m_buffers[i].id;
                vkDestroyBuffer(*(m_logical_device_ptr->Get_Device()), m_buffers[i].buffer, nullptr);
                vkFreeMemory(*(m_logical_device_ptr->Get_Device()), m_buffers[i].buffer_memory, nullptr);
            }
            m_buffers.clear();
        }

        unsigned int Storage_Manager::Get_Next_Buffer_Id(const char* label)
        {
            unsigned int count = 0;

            for (unsigned int i = 0; i < m_buffers.size(); i++)
            {
                if (strcmp(label, m_buffers[i].label) == 0)
                {
                    count++;
                }
            }

            return count;
        }

        unsigned int Storage_Manager::Get_Next_Image_Id(const char* label)
        {
            unsigned int count = 0;

            for (unsigned int i = 0; i < m_images.size(); i++)
            {
                if (strcmp(label, m_images[i].label) == 0)
                {
                    count++;
                }
            }

            return count;
        }

        void Storage_Manager::Create_Buffer(const char* label, VkDeviceSize buffer_size, VkBufferUsageFlagBits buffer_usage, Resouce_Queue_Families queue_family_usage)
        {
            std::set<unsigned int> queue_families = {};

            if (queue_family_usage.use_graphics && m_queue_manager_ptr->Get_Queue_Family_Indices().m_graphics_index.has_value())
            {
                queue_families.insert(m_queue_manager_ptr->Get_Queue_Family_Indices().m_graphics_index.value());
            }
            if (queue_family_usage.use_compute && m_queue_manager_ptr->Get_Queue_Family_Indices().m_compute_index.has_value())
            {
                queue_families.insert(m_queue_manager_ptr->Get_Queue_Family_Indices().m_compute_index.value());
            }
            if (queue_family_usage.use_transfer && m_queue_manager_ptr->Get_Queue_Family_Indices().m_transfer_index.has_value())
            {
                queue_families.insert(m_queue_manager_ptr->Get_Queue_Family_Indices().m_transfer_index.value());
            }
            if (queue_family_usage.use_sparse_binding && m_queue_manager_ptr->Get_Queue_Family_Indices().m_sparse_binding_index.has_value())
            {
                queue_families.insert(m_queue_manager_ptr->Get_Queue_Family_Indices().m_sparse_binding_index.value());
            }
            if (queue_family_usage.use_protected && m_queue_manager_ptr->Get_Queue_Family_Indices().m_protected_index.has_value())
            {
                queue_families.insert(m_queue_manager_ptr->Get_Queue_Family_Indices().m_protected_index.value());
            }
            if (queue_family_usage.use_present && m_queue_manager_ptr->Get_Queue_Family_Indices().m_present_index.has_value())
            {
                queue_families.insert(m_queue_manager_ptr->Get_Queue_Family_Indices().m_present_index.value());
            }
            std::vector<unsigned int> queue_families_vector(queue_families.begin(), queue_families.end());

            unsigned int buffer_id = Get_Next_Buffer_Id(label);
            m_buffers.resize(m_buffers.size() + 1);

            m_buffers.back() = {};
            m_buffers.back().label = label;
            m_buffers.back().id = buffer_id;

            LOG_INFO << "Vulkan: creating buffer " << label << "-" << buffer_id;

            VkBufferCreateInfo buffer_create_info = {};
            buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_create_info.pNext = NULL;
            buffer_create_info.flags = 0;
            buffer_create_info.size = buffer_size;
            buffer_create_info.usage = buffer_usage;
            buffer_create_info.sharingMode = (queue_families_vector.size() == 1) ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
            buffer_create_info.queueFamilyIndexCount = queue_families_vector.size();
            buffer_create_info.pQueueFamilyIndices = queue_families_vector.data();

            VALIDATE_VKRESULT(vkCreateBuffer(*(m_logical_device_ptr->Get_Device()), &buffer_create_info, nullptr, &m_buffers.back().buffer), "Vulkan: failed to create buffer");

            VkMemoryRequirements memory_requirements;
            vkGetBufferMemoryRequirements(*(m_logical_device_ptr->Get_Device()), m_buffers.back().buffer, &memory_requirements);

            VkPhysicalDeviceMemoryProperties memory_properties;
            vkGetPhysicalDeviceMemoryProperties(*(m_physical_device_ptr->Get_Physical_Device()), &memory_properties);

            unsigned int memory_type_index = 0;
            VkMemoryPropertyFlags memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            for (unsigned int i = 0; i < memory_properties.memoryTypeCount; i++)
            {
                if ((memory_requirements.memoryTypeBits & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & memory_property_flags) == memory_property_flags)
                {
                    memory_type_index = i;
                }
            }

            VkMemoryAllocateInfo memory_allocate_info = {};
            memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            memory_allocate_info.pNext = NULL;
            memory_allocate_info.allocationSize = memory_requirements.size;
            memory_allocate_info.memoryTypeIndex = memory_type_index;

            VALIDATE_VKRESULT(vkAllocateMemory(*(m_logical_device_ptr->Get_Device()), &memory_allocate_info, nullptr, &m_buffers.back().buffer_memory), "Vulkan: failed to allocate buffer memory");
            VALIDATE_VKRESULT(vkBindBufferMemory(*(m_logical_device_ptr->Get_Device()), m_buffers.back().buffer, m_buffers.back().buffer_memory, 0), "Vulkan: fail to bind buffer memory");

            LOG_TRACE << "Vulkan: finished creating buffer " << label << "-" << buffer_id;
        }
    } // namespace Vulkan
} // namespace CascadeGraphics