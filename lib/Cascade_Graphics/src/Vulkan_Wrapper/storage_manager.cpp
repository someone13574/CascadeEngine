#include "storage_manager.hpp"


#include "debug_tools.hpp"
#include <cstring>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Storage_Manager::Storage_Manager(std::shared_ptr<Device> logical_device_ptr, std::shared_ptr<Physical_Device> physical_device_ptr, std::shared_ptr<Queue_Manager> queue_manager_ptr)
            : m_logical_device_ptr(logical_device_ptr), m_physical_device_ptr(physical_device_ptr), m_queue_manager_ptr(queue_manager_ptr)
        {
        }

        Storage_Manager::~Storage_Manager()
        {
            LOG_INFO << "Vulkan: Cleaning up storage";

            for (unsigned int i = 0; i < m_buffer_resources.size(); i++)
            {
                Buffer_Resource* buffer_resource_ptr = &m_buffer_resources[i];

                LOG_TRACE << "Vulkan: Destroying buffer " << Get_Resource_String(buffer_resource_ptr->resource_id) << "'";

                vkDestroyBuffer(*m_logical_device_ptr->Get_Device(), buffer_resource_ptr->buffer, nullptr);
                vkFreeMemory(*m_logical_device_ptr->Get_Device(), buffer_resource_ptr->device_memory, nullptr);
            }
            m_buffer_resources.clear();

            for (unsigned int i = 0; i < m_image_resources.size(); i++)
            {
                Image_Resource* image_resource_ptr = &m_image_resources[i];

                if (!image_resource_ptr->is_swapchain_image)
                {
                    LOG_TRACE << "Vulkan: Destorying image '" << Get_Resource_String(image_resource_ptr->resource_id) << "'";

                    vkDestroyImage(*m_logical_device_ptr->Get_Device(), image_resource_ptr->image, nullptr);
                    vkDestroyImageView(*m_logical_device_ptr->Get_Device(), image_resource_ptr->image_view, nullptr);
                    vkFreeMemory(*m_logical_device_ptr->Get_Device(), image_resource_ptr->device_memory, nullptr);
                }
            }
            m_image_resources.clear();
            m_resource_groupings.clear();

            LOG_TRACE << "Vulkan: Finished cleaning up storage";
        }

        std::string Storage_Manager::Get_Resource_String(Resource_ID resource_id)
        {
            return resource_id.label.append("-").append(std::to_string(resource_id.index));
        }

        unsigned int Storage_Manager::Get_Buffer_Index(Resource_ID resource_id)
        {
            if (resource_id.resource_type == Resource_ID::BUFFER_RESOURCE)
            {
                for (unsigned int i = 0; i < m_buffer_resources.size(); i++)
                {
                    Resource_ID current_resource_id = m_buffer_resources[i].resource_id;

                    if (current_resource_id == resource_id)
                    {
                        return i;
                    }
                }
            }

            LOG_ERROR << "Vulkan: No buffers named '" << Get_Resource_String(resource_id) << "' exist";
            exit(EXIT_FAILURE);
        }

        unsigned int Storage_Manager::Get_Image_Index(Resource_ID resource_id)
        {
            if (resource_id.resource_type == Resource_ID::IMAGE_RESOURCE)
            {
                for (unsigned int i = 0; i < m_image_resources.size(); i++)
                {
                    Resource_ID current_resource_id = m_image_resources[i].resource_id;

                    if (current_resource_id == resource_id)
                    {
                        return i;
                    }
                }
            }

            LOG_ERROR << "Vulkan: No images named '" << Get_Resource_String(resource_id) << "' exist";
            exit(EXIT_FAILURE);
        }

        void Storage_Manager::Create_VkBuffer(Resource_ID resource_id)
        {
            LOG_TRACE << "Vulkan: Creating VkBuffer for " << Get_Resource_String(resource_id);

            Buffer_Resource* buffer_resource_ptr = Get_Buffer_Resource(resource_id);

            std::vector<unsigned int> unique_queues = m_queue_manager_ptr->Get_Unique_Queue_Families(buffer_resource_ptr->resource_queue_mask);

            VkBufferCreateInfo buffer_create_info = {};
            buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_create_info.pNext = nullptr;
            buffer_create_info.flags = 0;
            buffer_create_info.size = buffer_resource_ptr->buffer_size;
            buffer_create_info.usage = buffer_resource_ptr->buffer_usage;
            buffer_create_info.sharingMode = (unique_queues.size() == 1) ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
            buffer_create_info.queueFamilyIndexCount = unique_queues.size();
            buffer_create_info.pQueueFamilyIndices = unique_queues.data();

            VALIDATE_VKRESULT(vkCreateBuffer(*m_logical_device_ptr->Get_Device(), &buffer_create_info, nullptr, &buffer_resource_ptr->buffer), "Vulkan: Failed to create VkBuffer");
        }

        void Storage_Manager::Get_Buffer_Memory_Info(Resource_ID resource_id)
        {
            LOG_TRACE << "Vulkan: Getting memory infomation for " << Get_Resource_String(resource_id);

            Buffer_Resource* buffer_resource_ptr = Get_Buffer_Resource(resource_id);

            vkGetBufferMemoryRequirements(*m_logical_device_ptr->Get_Device(), buffer_resource_ptr->buffer, &buffer_resource_ptr->memory_requirements);

            VkPhysicalDeviceMemoryProperties device_memory_properties;
            vkGetPhysicalDeviceMemoryProperties(*m_physical_device_ptr->Get_Physical_Device(), &device_memory_properties);

            VkMemoryPropertyFlags required_memory_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            for (unsigned int i = 0; i < device_memory_properties.memoryTypeCount; i++)
            {
                if (buffer_resource_ptr->memory_requirements.memoryTypeBits & (1 << i))
                {
                    if ((device_memory_properties.memoryTypes[i].propertyFlags & required_memory_properties) == required_memory_properties)
                    {
                        buffer_resource_ptr->memory_type_index = i;
                    }
                }
            }
        }

        void Storage_Manager::Allocate_Buffer_Memory(Resource_ID resource_id)
        {
            LOG_TRACE << "Vulkan: Allocating memory for buffer " << Get_Resource_String(resource_id);

            Buffer_Resource* buffer_resource_ptr = Get_Buffer_Resource(resource_id);

            VkMemoryAllocateInfo memory_allocate_info = {};
            memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            memory_allocate_info.pNext = nullptr;
            memory_allocate_info.allocationSize = buffer_resource_ptr->memory_requirements.size;
            memory_allocate_info.memoryTypeIndex = buffer_resource_ptr->memory_type_index;

            VALIDATE_VKRESULT(vkAllocateMemory(*m_logical_device_ptr->Get_Device(), &memory_allocate_info, nullptr, &buffer_resource_ptr->device_memory), "Vulkan: Failed to allocate buffer memory");
            VALIDATE_VKRESULT(vkBindBufferMemory(*m_logical_device_ptr->Get_Device(), buffer_resource_ptr->buffer, buffer_resource_ptr->device_memory, 0), "Vulkan: Failed to bind buffer memory");
        }

        void Storage_Manager::Create_VkImage(Resource_ID resource_id)
        {
            LOG_TRACE << "Vulkan: Creating VkImage for image " << Get_Resource_String(resource_id);

            Image_Resource* image_resource_ptr = Get_Image_Resource(resource_id);

            std::vector<unsigned int> unique_queues = m_queue_manager_ptr->Get_Unique_Queue_Families(image_resource_ptr->resource_queue_mask);

            VkImageCreateInfo image_create_info = {};
            image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_create_info.pNext = nullptr;
            image_create_info.flags = 0;
            image_create_info.imageType = VK_IMAGE_TYPE_2D;
            image_create_info.format = image_resource_ptr->image_format;
            image_create_info.extent = {image_resource_ptr->image_size.width, image_resource_ptr->image_size.height};
            image_create_info.mipLevels = 1;
            image_create_info.arrayLayers = 1;
            image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
            image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
            image_create_info.usage = image_resource_ptr->image_usage;
            image_create_info.sharingMode = (unique_queues.size() == 1) ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
            image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            VALIDATE_VKRESULT(vkCreateImage(*m_logical_device_ptr->Get_Device(), &image_create_info, nullptr, &image_resource_ptr->image), "Vulkan: Failed to create VkImage");
        }

        void Storage_Manager::Get_Image_Memory_Info(Resource_ID resource_id)
        {
            LOG_TRACE << "Vulkan: Getting memory infomation for " << Get_Resource_String(resource_id);

            Image_Resource* image_resource_ptr = Get_Image_Resource(resource_id);

            vkGetImageMemoryRequirements(*m_logical_device_ptr->Get_Device(), image_resource_ptr->image, &image_resource_ptr->memory_requirements);

            VkPhysicalDeviceMemoryProperties device_memory_properties;
            vkGetPhysicalDeviceMemoryProperties(*m_physical_device_ptr->Get_Physical_Device(), &device_memory_properties);

            VkMemoryPropertyFlags required_memory_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            for (unsigned int i = 0; i < device_memory_properties.memoryTypeCount; i++)
            {
                if (image_resource_ptr->memory_requirements.memoryTypeBits & (1 << i))
                {
                    if ((device_memory_properties.memoryTypes[i].propertyFlags & required_memory_properties) == required_memory_properties)
                    {
                        image_resource_ptr->memory_type_index = i;
                    }
                }
            }
        }

        void Storage_Manager::Allocate_Image_Memory(Resource_ID resource_id)
        {
            LOG_TRACE << "Vulkan: Allocating memory for image " << Get_Resource_String(resource_id);

            Image_Resource* image_resource_ptr = Get_Image_Resource(resource_id);

            VkMemoryAllocateInfo memory_allocate_info = {};
            memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
            memory_allocate_info.pNext = nullptr;
            memory_allocate_info.allocationSize = image_resource_ptr->memory_requirements.size;
            memory_allocate_info.memoryTypeIndex = image_resource_ptr->memory_type_index;

            VALIDATE_VKRESULT(vkAllocateMemory(*m_logical_device_ptr->Get_Device(), &memory_allocate_info, nullptr, &image_resource_ptr->device_memory), "Vulkan: Failed to allocate image memory");
            VALIDATE_VKRESULT(vkBindImageMemory(*m_logical_device_ptr->Get_Device(), image_resource_ptr->image, image_resource_ptr->device_memory, 0), "Vulkan: Failed to bind image memory");
        }

        void Storage_Manager::Create_Image_View(Resource_ID resource_id)
        {
            LOG_TRACE << "Vulkan: Creating image view for image " << Get_Resource_String(resource_id);

            Image_Resource* image_resource_ptr = Get_Image_Resource(resource_id);

            VkImageViewCreateInfo image_view_create_info = {};
            image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_create_info.pNext = nullptr;
            image_view_create_info.flags = 0;
            image_view_create_info.image = image_resource_ptr->image;
            image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            image_view_create_info.format = image_resource_ptr->image_format;
            image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_view_create_info.subresourceRange.baseMipLevel = 0;
            image_view_create_info.subresourceRange.levelCount = 1;
            image_view_create_info.subresourceRange.baseArrayLayer = 0;
            image_view_create_info.subresourceRange.layerCount = 1;

            VALIDATE_VKRESULT(vkCreateImageView(*m_logical_device_ptr->Get_Device(), &image_view_create_info, nullptr, &image_resource_ptr->image_view), "Vulkan: Failed to create image view");
        }

        void Storage_Manager::Create_Buffer(std::string label, VkDeviceSize buffer_size, VkBufferUsageFlagBits buffer_usage, VkDescriptorType descriptor_type, unsigned int resource_queue_mask)
        {
            Resource_ID resource_id = {};
            resource_id.label = label;
            resource_id.index = 0;
            resource_id.resource_type = Resource_ID::Resource_Type::BUFFER_RESOURCE;

            while (true)
            {
                bool index_in_use = false;

                for (unsigned int i = 0; i < m_buffer_resources.size(); i++)
                {
                    index_in_use |= m_buffer_resources[i].resource_id == resource_id;
                }

                if (!index_in_use)
                {
                    break;
                }
                else
                {
                    resource_id.index++;
                }
            }

            LOG_INFO << "Vulkan: Creating buffer " << Get_Resource_String(resource_id);

            m_buffer_resources.resize(m_buffer_resources.size() + 1);
            m_buffer_resources.back() = {};
            m_buffer_resources.back().resource_id = resource_id;
            m_buffer_resources.back().buffer_size = buffer_size;
            m_buffer_resources.back().descriptor_type = descriptor_type;
            m_buffer_resources.back().buffer_usage = buffer_usage;
            m_buffer_resources.back().resource_queue_mask = resource_queue_mask;
            m_buffer_resources.back().buffer = VK_NULL_HANDLE;
            m_buffer_resources.back().device_memory = VK_NULL_HANDLE;
            m_buffer_resources.back().memory_type_index = 0;

            Create_VkBuffer(resource_id);
            Get_Buffer_Memory_Info(resource_id);
            Allocate_Buffer_Memory(resource_id);
        }

        void Storage_Manager::Create_Image(std::string label, VkFormat image_format, VkImageUsageFlags image_usage, VkDescriptorType descriptor_type, VkExtent2D image_size, unsigned int resource_queue_mask)
        {
            Resource_ID resource_id = {};
            resource_id.label = label;
            resource_id.index = 0;
            resource_id.resource_type = Resource_ID::Resource_Type::IMAGE_RESOURCE;

            while (true)
            {
                bool index_in_use = false;

                for (unsigned int i = 0; i < m_image_resources.size(); i++)
                {
                    index_in_use |= m_buffer_resources[i].resource_id == resource_id;
                }

                if (!index_in_use)
                {
                    break;
                }
                else
                {
                    resource_id.index++;
                }
            }

            LOG_INFO << "Vulkan: Creating image " << Get_Resource_String(resource_id);

            m_image_resources.resize(m_image_resources.size() + 1);
            m_image_resources.back() = {};
            m_image_resources.back().resource_id = resource_id;
            m_image_resources.back().is_swapchain_image = false;
            m_image_resources.back().image_format = image_format;
            m_image_resources.back().image_usage = image_usage;
            m_image_resources.back().descriptor_type = descriptor_type;
            m_image_resources.back().image_size = image_size;
            m_image_resources.back().resource_queue_mask = resource_queue_mask;
            m_image_resources.back().image = VK_NULL_HANDLE;
            m_image_resources.back().image_view = VK_NULL_HANDLE;
            m_image_resources.back().device_memory = VK_NULL_HANDLE;
            m_image_resources.back().memory_type_index = 0;

            Create_VkImage(resource_id);
            Get_Image_Memory_Info(resource_id);
            Allocate_Image_Memory(resource_id);
            Create_Image_View(resource_id);
        }

        void Storage_Manager::Create_Resource_Grouping(std::string label, std::vector<Resource_ID> resource_ids)
        {
            LOG_INFO << "Vulkan: Creating resource grouping with label '" << label << "'";

            for (unsigned int i = 0; i < m_resource_groupings.size(); i++)
            {
                if (m_resource_groupings[i].label == label)
                {
                    LOG_ERROR << "Vulkan: A resource grouping with that label already exists";
                    exit(EXIT_FAILURE);
                }
            }

            unsigned int buffer_resource_count = 0;
            unsigned int image_resource_count = 0;
            for (unsigned int i = 0; i < resource_ids.size(); i++)
            {
                bool resource_exists = false;
                if (resource_ids[i].resource_type == Resource_ID::BUFFER_RESOURCE)
                {
                    for (unsigned int j = 0; j < m_buffer_resources.size(); j++)
                    {
                        if (m_buffer_resources[j].resource_id == resource_ids[i])
                        {
                            resource_exists = true;
                        }
                    }

                    if (!resource_exists)
                    {
                        LOG_ERROR << "Vulkan: The buffer '" << Get_Resource_String(resource_ids[i]) << "' does not exist";
                        exit(EXIT_FAILURE);
                    }
                    buffer_resource_count++;
                }
                else if (resource_ids[i].resource_type == Resource_ID::IMAGE_RESOURCE)
                {
                    for (unsigned int j = 0; j < m_image_resources.size(); j++)
                    {
                        if (m_image_resources[j].resource_id == resource_ids[i])
                        {
                            resource_exists = true;
                        }
                    }

                    if (!resource_exists)
                    {
                        LOG_ERROR << "Vulkan: The image '" << Get_Resource_String(resource_ids[i]) << "' does not exist";
                        exit(EXIT_FAILURE);
                    }
                    image_resource_count++;
                }
            }

            m_resource_groupings.resize(m_resource_groupings.size() + 1);
            m_resource_groupings.back() = {};
            m_resource_groupings.back().label = label;
            m_resource_groupings.back().has_descriptor_set = false;
            m_resource_groupings.back().buffer_resource_count = buffer_resource_count;
            m_resource_groupings.back().image_resource_count = image_resource_count;
            m_resource_groupings.back().resource_ids = resource_ids;
        }

        void Storage_Manager::Add_Image(std::string label, Image_Resource image_resource)
        {
            Resource_ID resource_id = {};
            resource_id.label = label;
            resource_id.index = 0;
            resource_id.resource_type = Resource_ID::Resource_Type::IMAGE_RESOURCE;

            while (true)
            {
                bool index_in_use = false;

                for (unsigned int i = 0; i < m_image_resources.size(); i++)
                {
                    index_in_use |= m_image_resources[i].resource_id == resource_id;
                }

                if (!index_in_use)
                {
                    break;
                }
                else
                {
                    resource_id.index++;
                }
            }

            LOG_INFO << "Vulkan: Adding image resource to storage manager (provided resource_id was overwritten) with resource id " << Get_Resource_String(resource_id);

            image_resource.resource_id = resource_id;
            m_image_resources.push_back(image_resource);
        }

        void Storage_Manager::Resize_Buffer(Resource_ID resource_id, VkDeviceSize buffer_size)
        {
            LOG_TRACE << "Vulkan: Resizing buffer '" << Get_Resource_String(resource_id) << "'";

            Buffer_Resource* buffer_resource_ptr = Get_Buffer_Resource(resource_id);

            vkDestroyBuffer(*m_logical_device_ptr->Get_Device(), buffer_resource_ptr->buffer, nullptr);
            vkFreeMemory(*m_logical_device_ptr->Get_Device(), buffer_resource_ptr->device_memory, nullptr);

            buffer_resource_ptr->buffer_size = buffer_size;
            buffer_resource_ptr->buffer = VK_NULL_HANDLE;
            buffer_resource_ptr->device_memory = VK_NULL_HANDLE;
            buffer_resource_ptr->memory_type_index = 0;

            Create_VkBuffer(resource_id);
            Get_Buffer_Memory_Info(resource_id);
            Allocate_Buffer_Memory(resource_id);
        }

        void Storage_Manager::Upload_To_Buffer(Resource_ID resource_id, void* data, size_t data_size)
        {
            Buffer_Resource* buffer_resource_ptr = Get_Buffer_Resource(resource_id);

            void* mapped_memory;
            VALIDATE_VKRESULT(vkMapMemory(*m_logical_device_ptr->Get_Device(), buffer_resource_ptr->device_memory, 0, data_size, 0, &mapped_memory), "Vulkan: Failed to map memory");

            memcpy(mapped_memory, data, data_size);

            vkUnmapMemory(*m_logical_device_ptr->Get_Device(), buffer_resource_ptr->device_memory);
        }

        Storage_Manager::Buffer_Resource* Storage_Manager::Get_Buffer_Resource(Resource_ID resource_id)
        {
            for (unsigned int i = 0; i < m_buffer_resources.size(); i++)
            {
                if (m_buffer_resources[i].resource_id == resource_id)
                {
                    return &m_buffer_resources[i];
                }
            }

            LOG_ERROR << "Vulkan: Buffer '" << Get_Resource_String(resource_id) << "' doesn't exist";
            exit(EXIT_FAILURE);
        }

        Storage_Manager::Image_Resource* Storage_Manager::Get_Image_Resource(Resource_ID resource_id)
        {
            for (unsigned int i = 0; i < m_image_resources.size(); i++)
            {
                if (m_image_resources[i].resource_id == resource_id)
                {
                    return &m_image_resources[i];
                }
            }

            LOG_ERROR << "Vulkan: Image " << Get_Resource_String(resource_id) << " doesn't exist";
            exit(EXIT_FAILURE);
        }

        Storage_Manager::Resource_Grouping* Storage_Manager::Get_Resource_Grouping(std::string label)
        {
            for (unsigned int i = 0; i < m_resource_groupings.size(); i++)
            {
                if (m_resource_groupings[i].label == label)
                {
                    return &m_resource_groupings[i];
                }
            }

            LOG_ERROR << "Vulkan: The resource grouping '" << label << "' does not exist";
            exit(EXIT_FAILURE);
        }
    } // namespace Vulkan
} // namespace Cascade_Graphics