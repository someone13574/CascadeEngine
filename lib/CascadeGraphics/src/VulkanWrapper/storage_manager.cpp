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

            for (unsigned int i = 0; i < m_images.size(); i++)
            {
                LOG_TRACE << "Vulkan: destorying image " << m_images[i].label << "-" << m_images[i].id;
                vkDestroyImage(*(m_logical_device_ptr->Get_Device()), m_images[i].image, nullptr);
                vkDestroyImageView(*(m_logical_device_ptr->Get_Device()), m_images[i].image_view, nullptr);
                vkFreeMemory(*(m_logical_device_ptr->Get_Device()), m_images[i].image_memory, nullptr);
            }
            m_images.clear();

            LOG_TRACE << "Vulkan: finished cleaning up storage";
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

        std::vector<unsigned int> Storage_Manager::Get_Queue_Families(Resouce_Queue_Families resouce_queue_families)
        {
            std::set<unsigned int> queue_families = {};

            if (resouce_queue_families.use_graphics && m_queue_manager_ptr->Get_Queue_Family_Indices().m_graphics_index.has_value())
            {
                queue_families.insert(m_queue_manager_ptr->Get_Queue_Family_Indices().m_graphics_index.value());
            }
            if (resouce_queue_families.use_compute && m_queue_manager_ptr->Get_Queue_Family_Indices().m_compute_index.has_value())
            {
                queue_families.insert(m_queue_manager_ptr->Get_Queue_Family_Indices().m_compute_index.value());
            }
            if (resouce_queue_families.use_transfer && m_queue_manager_ptr->Get_Queue_Family_Indices().m_transfer_index.has_value())
            {
                queue_families.insert(m_queue_manager_ptr->Get_Queue_Family_Indices().m_transfer_index.value());
            }
            if (resouce_queue_families.use_sparse_binding && m_queue_manager_ptr->Get_Queue_Family_Indices().m_sparse_binding_index.has_value())
            {
                queue_families.insert(m_queue_manager_ptr->Get_Queue_Family_Indices().m_sparse_binding_index.value());
            }
            if (resouce_queue_families.use_protected && m_queue_manager_ptr->Get_Queue_Family_Indices().m_protected_index.has_value())
            {
                queue_families.insert(m_queue_manager_ptr->Get_Queue_Family_Indices().m_protected_index.value());
            }
            if (resouce_queue_families.use_present && m_queue_manager_ptr->Get_Queue_Family_Indices().m_present_index.has_value())
            {
                queue_families.insert(m_queue_manager_ptr->Get_Queue_Family_Indices().m_present_index.value());
            }

            std::vector<unsigned int> queue_families_vector(queue_families.begin(), queue_families.end());
            return queue_families_vector;
        }

        void Storage_Manager::Create_Buffer(const char* label, VkDeviceSize buffer_size, VkBufferUsageFlagBits buffer_usage, Resouce_Queue_Families resouce_queue_families)
        {
            std::vector<unsigned int> queue_families = Get_Queue_Families(resouce_queue_families);

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
            buffer_create_info.sharingMode = (queue_families.size() == 1) ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
            buffer_create_info.queueFamilyIndexCount = queue_families.size();
            buffer_create_info.pQueueFamilyIndices = queue_families.data();

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

        void Storage_Manager::Create_Image(const char* label, VkFormat image_format, VkImageUsageFlags image_usage, VkExtent2D image_size, Resouce_Queue_Families resouce_queue_families)
        {
            std::vector<unsigned int> queue_families = Get_Queue_Families(resouce_queue_families);

            unsigned int image_id = Get_Next_Image_Id(label);

            m_images.resize(m_images.size() + 1);
            m_images.back() = {};
            m_images.back().label = label;
            m_images.back().id = image_id;

            LOG_INFO << "Vulkan: creating image " << label << "-" << image_id;

            VkImageCreateInfo image_create_info = {};
            image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_create_info.pNext = NULL;
            image_create_info.flags = 0;
            image_create_info.imageType = VK_IMAGE_TYPE_2D;
            image_create_info.format = image_format;
            image_create_info.extent = {image_size.width, image_size.height, 1};
            image_create_info.mipLevels = 1;
            image_create_info.arrayLayers = 1;
            image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
            image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
            image_create_info.usage = image_usage;
            image_create_info.sharingMode = (queue_families.size() == 1) ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
            image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            VALIDATE_VKRESULT(vkCreateImage(*(m_logical_device_ptr->Get_Device()), &image_create_info, nullptr, &m_images.back().image), "Vulkan: failed to create image");

            VkMemoryRequirements memory_requirements;
            vkGetImageMemoryRequirements(*(m_logical_device_ptr->Get_Device()), m_images.back().image, &memory_requirements);

            VkPhysicalDeviceMemoryProperties memory_properties;
            vkGetPhysicalDeviceMemoryProperties(*(m_physical_device_ptr->Get_Physical_Device()), &memory_properties);

            unsigned int memory_type_index = 0;
            VkMemoryPropertyFlags memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
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

            VALIDATE_VKRESULT(vkAllocateMemory(*(m_logical_device_ptr->Get_Device()), &memory_allocate_info, nullptr, &m_images.back().image_memory), "Vulkan: failed to allocate image memory");
            VALIDATE_VKRESULT(vkBindImageMemory(*(m_logical_device_ptr->Get_Device()), m_images.back().image, m_images.back().image_memory, 0), "Vulkan: fail to bind image memory");

            VkImageViewCreateInfo image_view_create_info = {};
            image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_create_info.pNext = NULL;
            image_view_create_info.flags = 0;
            image_view_create_info.image = m_images.back().image;
            image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            image_view_create_info.format = image_format;
            image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_view_create_info.subresourceRange.baseMipLevel = 0;
            image_view_create_info.subresourceRange.levelCount = 1;
            image_view_create_info.subresourceRange.baseArrayLayer = 0;
            image_view_create_info.subresourceRange.layerCount = 1;

            VALIDATE_VKRESULT(vkCreateImageView(*(m_logical_device_ptr->Get_Device()), &image_view_create_info, nullptr, &m_images.back().image_view), "Vulkan: failed to create image view");

            LOG_TRACE << "Vulkan: finished creating image " << label << "-" << image_id;
        }
    } // namespace Vulkan
} // namespace CascadeGraphics