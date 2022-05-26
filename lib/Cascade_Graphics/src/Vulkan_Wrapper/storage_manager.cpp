#include "storage_manager.hpp"

#include "../debug_tools.hpp"

#include <cstring>
#include <set>
#include <sstream>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Storage_Manager::Storage_Manager(std::shared_ptr<Device> logical_device_ptr, std::shared_ptr<Physical_Device> physical_device_ptr, std::shared_ptr<Queue_Manager> queue_manager_ptr, std::shared_ptr<Swapchain> swapchain_ptr)
            : m_logical_device_ptr(logical_device_ptr), m_physical_device_ptr(physical_device_ptr), m_queue_manager_ptr(queue_manager_ptr), m_swapchain_ptr(swapchain_ptr)
        {
        }

        Storage_Manager::~Storage_Manager()
        {
            LOG_INFO << "Vulkan: cleaning up storage";

            for (unsigned int i = 0; i < m_buffers.size(); i++)
            {
                LOG_TRACE << "Vulkan: destroying buffer " << m_buffers[i].resource_id.label << "-" << m_buffers[i].resource_id.index;

                vkDestroyBuffer(*(m_logical_device_ptr->Get_Device()), m_buffers[i].buffer, nullptr);
                vkFreeMemory(*(m_logical_device_ptr->Get_Device()), m_buffers[i].buffer_memory, nullptr);
            }
            m_buffers.clear();

            for (unsigned int i = 0; i < m_images.size(); i++)
            {
                if (m_images[i].resource_id.type == Resource_Type::IMAGE)
                {
                    LOG_TRACE << "Vulkan: destorying image " << m_images[i].resource_id.label << "-" << m_images[i].resource_id.index;

                    vkDestroyImage(*(m_logical_device_ptr->Get_Device()), m_images[i].image_info->image, nullptr);
                    vkDestroyImageView(*(m_logical_device_ptr->Get_Device()), m_images[i].image_info->image_view, nullptr);
                    vkFreeMemory(*(m_logical_device_ptr->Get_Device()), m_images[i].image_info->image_memory, nullptr);
                }
            }
            m_images.clear();

            LOG_TRACE << "Vulkan: finished cleaning up storage";
        }

        unsigned int Storage_Manager::Get_Next_Buffer_Id(std::string label)
        {
            unsigned int count = 0;

            for (unsigned int i = 0; i < m_buffers.size(); i++)
            {
                if (label == m_buffers[i].resource_id.label)
                {
                    count++;
                }
            }

            return count;
        }

        unsigned int Storage_Manager::Get_Next_Image_Id(std::string label)
        {
            unsigned int count = 0;

            for (unsigned int i = 0; i < m_images.size(); i++)
            {
                if (label == m_images[i].resource_id.label)
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

        void Storage_Manager::Create_Buffer_From_ID(Resource_ID resource_id, VkDeviceSize buffer_size, VkBufferUsageFlagBits buffer_usage, VkDescriptorType buffer_type, Resouce_Queue_Families resouce_queue_families)
        {
            std::vector<unsigned int> queue_families = Get_Queue_Families(resouce_queue_families);

            m_buffers.resize(m_buffers.size() + 1);
            m_buffers.back() = {};
            m_buffers.back().resource_id = resource_id;
            m_buffers.back().descriptor_type = buffer_type;
            m_buffers.back().buffer_usage = buffer_usage;
            m_buffers.back().resource_queue_families = resouce_queue_families;

            LOG_INFO << "Vulkan: creating buffer " << resource_id.label << "-" << resource_id.index;

            VkBufferCreateInfo buffer_create_info = {};
            buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_create_info.pNext = nullptr;
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
            memory_allocate_info.pNext = nullptr;
            memory_allocate_info.allocationSize = memory_requirements.size;
            memory_allocate_info.memoryTypeIndex = memory_type_index;

            VALIDATE_VKRESULT(vkAllocateMemory(*(m_logical_device_ptr->Get_Device()), &memory_allocate_info, nullptr, &m_buffers.back().buffer_memory), "Vulkan: failed to allocate buffer memory");
            VALIDATE_VKRESULT(vkBindBufferMemory(*(m_logical_device_ptr->Get_Device()), m_buffers.back().buffer, m_buffers.back().buffer_memory, 0), "Vulkan: fail to bind buffer memory");

            LOG_TRACE << "Vulkan: finished creating buffer " << resource_id.label << "-" << resource_id.index;
        }

        void Storage_Manager::Create_Buffer(std::string label, VkDeviceSize buffer_size, VkBufferUsageFlagBits buffer_usage, VkDescriptorType buffer_type, Resouce_Queue_Families resouce_queue_families)
        {
            Resource_ID resource_id = {};
            resource_id.label = label;
            resource_id.index = Get_Next_Buffer_Id(label);
            resource_id.type = Resource_Type::BUFFER;

            Create_Buffer_From_ID(resource_id, buffer_size, buffer_usage, buffer_type, resouce_queue_families);
        }

        void Storage_Manager::Create_Image(std::string label, VkFormat image_format, VkImageUsageFlags image_usage, VkDescriptorType image_type, VkExtent2D image_size, Resouce_Queue_Families resouce_queue_families)
        {
            std::vector<unsigned int> queue_families = Get_Queue_Families(resouce_queue_families);

            unsigned int image_id = Get_Next_Image_Id(label);

            Image_Info image_info = {};
            image_info.descriptor_type = image_type;

            m_images.resize(m_images.size() + 1);
            m_images.back() = {};
            m_images.back().resource_id.index = image_id;
            m_images.back().resource_id.label = label;
            m_images.back().resource_id.type = Resource_Type::IMAGE;

            LOG_INFO << "Vulkan: creating image " << label << "-" << image_id;

            VkImageCreateInfo image_create_info = {};
            image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_create_info.pNext = nullptr;
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

            VALIDATE_VKRESULT(vkCreateImage(*(m_logical_device_ptr->Get_Device()), &image_create_info, nullptr, &image_info.image), "Vulkan: failed to create image");

            VkMemoryRequirements memory_requirements;
            vkGetImageMemoryRequirements(*(m_logical_device_ptr->Get_Device()), image_info.image, &memory_requirements);

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
            memory_allocate_info.pNext = nullptr;
            memory_allocate_info.allocationSize = memory_requirements.size;
            memory_allocate_info.memoryTypeIndex = memory_type_index;

            VALIDATE_VKRESULT(vkAllocateMemory(*(m_logical_device_ptr->Get_Device()), &memory_allocate_info, nullptr, &image_info.image_memory), "Vulkan: failed to allocate image memory");
            VALIDATE_VKRESULT(vkBindImageMemory(*(m_logical_device_ptr->Get_Device()), image_info.image, image_info.image_memory, 0), "Vulkan: fail to bind image memory");

            VkImageViewCreateInfo image_view_create_info = {};
            image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_create_info.pNext = nullptr;
            image_view_create_info.flags = 0;
            image_view_create_info.image = image_info.image;
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

            VALIDATE_VKRESULT(vkCreateImageView(*(m_logical_device_ptr->Get_Device()), &image_view_create_info, nullptr, &image_info.image_view), "Vulkan: failed to create image view");

            m_images.back().image_info = image_info;

            LOG_TRACE << "Vulkan: finished creating image " << label << "-" << image_id;
        }

        void Storage_Manager::Add_Swapchain(std::string label)
        {
            LOG_INFO << "Vulkan: adding swapchain's images to storage manager under label " << label;

            for (unsigned int i = 0; i < m_images.size(); i++)
            {
                if (m_images[i].resource_id.label == label)
                {
                    LOG_ERROR << "Vulkan: cannot have pre-existing resources share label '" << label << "' with swapchain";
                    exit(EXIT_FAILURE);
                }
            }

            for (unsigned int i = 0; i < m_swapchain_ptr->Get_Swapchain_Image_Count(); i++)
            {
                m_images.resize(m_images.size() + 1);
                m_images.back() = {};
                m_images.back().resource_id.index = i;
                m_images.back().resource_id.label = label;
                m_images.back().resource_id.type = Resource_Type::SWAPCHAIN_IMAGE;

                LOG_TRACE << "Vulkan: adding swapchain image " << i;
            }

            LOG_TRACE << "Vulkan: finished adding swapchain's images";
        }

        VkImage* Storage_Manager::Get_Image(Resource_ID resource_id)
        {
            for (unsigned int i = 0; i < m_images.size(); i++)
            {
                if (m_images[i].resource_id == resource_id)
                {
                    if (m_images[i].resource_id.type == Resource_Type::IMAGE)
                    {
                        return &m_images[i].image_info->image;
                    }
                    else if (m_images[i].resource_id.type == Resource_Type::SWAPCHAIN_IMAGE)
                    {
                        return m_swapchain_ptr->Get_Swapchain_Image(resource_id.index);
                    }
                }
            }


            LOG_ERROR << "Vulkan: image with resource id '" << resource_id.label << "-" << resource_id.index << "' does not exist";
            exit(EXIT_FAILURE);
        }

        VkImageView* Storage_Manager::Get_Image_View(Resource_ID resource_id)
        {
            for (unsigned int i = 0; i < m_images.size(); i++)
            {
                if (m_images[i].resource_id == resource_id)
                {
                    if (m_images[i].resource_id.type == Resource_Type::IMAGE)
                    {
                        return &m_images[i].image_info->image_view;
                    }
                }
            }

            LOG_ERROR << "Vulkan: image with resource id '" << resource_id.label << "-" << resource_id.index << "' does not exist";
            exit(EXIT_FAILURE);
        }

        VkBuffer* Storage_Manager::Get_Buffer(Resource_ID resource_id)
        {
            for (unsigned int i = 0; i < m_buffers.size(); i++)
            {
                if (m_buffers[i].resource_id == resource_id)
                {
                    if (m_buffers[i].resource_id.type == Resource_Type::BUFFER)
                    {
                        return &m_buffers[i].buffer;
                    }
                }
            }

            LOG_ERROR << "Vulkan: buffer with resource id '" << resource_id.label << "-" << resource_id.index << "' does not exist";
            exit(EXIT_FAILURE);
        }

        Storage_Manager::Resource_Data Storage_Manager::Get_Resource_Data(Resource_ID resource_id)
        {
            if (resource_id.type == BUFFER)
            {
                for (unsigned int i = 0; i < m_buffers.size(); i++)
                {
                    if (m_buffers[i].resource_id == resource_id)
                    {
                        return {resource_id, m_buffers[i].descriptor_type};
                    }
                }
            }
            else
            {
                for (unsigned int i = 0; i < m_images.size(); i++)
                {
                    if (m_images[i].resource_id == resource_id)
                    {
                        if (m_images[i].resource_id.type == Resource_Type::IMAGE)
                        {
                            return {resource_id, m_images[i].image_info->descriptor_type};
                        }
                        else
                        {
                            LOG_ERROR << "Vulkan: cannot get descriptor type of swapchain image";
                            exit(EXIT_FAILURE);
                        }
                    }
                }
            }

            LOG_ERROR << "Vulkan: resource " << resource_id.label << "-" << resource_id.index << " does not exist";
            exit(EXIT_FAILURE);
        }

        bool Storage_Manager::Does_Resource_Exist(Resource_ID resource_id)
        {
            if (resource_id.type == BUFFER)
            {
                for (unsigned int i = 0; i < m_buffers.size(); i++)
                {
                    if (m_buffers[i].resource_id == resource_id)
                    {
                        return true;
                    }
                }
            }
            else if (resource_id.type == IMAGE || resource_id.type == SWAPCHAIN_IMAGE)
            {
                for (unsigned int i = 0; i < m_images.size(); i++)
                {
                    if (m_images[i].resource_id == resource_id)
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        void Storage_Manager::Upload_To_Buffer(Resource_ID resource_id, void* data, size_t data_size)
        {
            if (resource_id.type == BUFFER)
            {
                for (unsigned int i = 0; i < m_buffers.size(); i++)
                {
                    if (m_buffers[i].resource_id == resource_id)
                    {
                        void* mapped_memory;
                        VALIDATE_VKRESULT(vkMapMemory(*(m_logical_device_ptr->Get_Device()), m_buffers[i].buffer_memory, 0, data_size, 0, &mapped_memory), "Vulkan: failed to map memory for buffer upload");
                        memcpy(mapped_memory, data, data_size);
                        vkUnmapMemory(*(m_logical_device_ptr->Get_Device()), m_buffers[i].buffer_memory);
                        return;
                    }
                }
            }

            LOG_ERROR << "Vulkan: buffer " << resource_id.label << "-" << resource_id.index << " does not exist";
            exit(EXIT_FAILURE);
        }
    } // namespace Vulkan
} // namespace Cascade_Graphics