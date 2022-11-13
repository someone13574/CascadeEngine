#include "storage_manager.hpp"

#include "debug_tools.hpp"
#include <cstring>


namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        Storage_Manager::Storage_Manager(std::shared_ptr<Logical_Device_Wrapper> logical_device_wrapper_ptr, std::shared_ptr<Physical_Device_Wrapper> physical_device_wrapper_ptr, std::shared_ptr<Queue_Manager> queue_manager_ptr)
            : m_logical_device_wrapper_ptr(logical_device_wrapper_ptr), m_physical_device_wrapper_ptr(physical_device_wrapper_ptr), m_queue_manager_ptr(queue_manager_ptr)
        {
        }

        Storage_Manager::~Storage_Manager()
        {
            LOG_INFO << "Vulkan Backend: Cleaning up storage";

            for (uint32_t i = 0; i < m_buffer_resources.size(); i++)
            {
                Buffer_Resource* buffer_resource_ptr = &m_buffer_resources[i];

                LOG_TRACE << "Vulkan Backend: Destroying buffer " << buffer_resource_ptr->identifier.Get_Identifier_String();

                vkDestroyBuffer(*m_logical_device_wrapper_ptr->Get_Device(), buffer_resource_ptr->buffer, nullptr);
                vkFreeMemory(*m_logical_device_wrapper_ptr->Get_Device(), buffer_resource_ptr->device_memory, nullptr);
            }
            m_buffer_resources.clear();

            for (uint32_t i = 0; i < m_image_resources.size(); i++)
            {
                Image_Resource* image_resource_ptr = &m_image_resources[i];

                if (!image_resource_ptr->is_swapchain_image)
                {
                    LOG_TRACE << "Vulkan Backend: Destorying image " << image_resource_ptr->identifier.Get_Identifier_String();

                    vkDestroyImage(*m_logical_device_wrapper_ptr->Get_Device(), image_resource_ptr->image, nullptr);
                    vkDestroyImageView(*m_logical_device_wrapper_ptr->Get_Device(), image_resource_ptr->image_view, nullptr);
                    vkFreeMemory(*m_logical_device_wrapper_ptr->Get_Device(), image_resource_ptr->device_memory, nullptr);
                }
            }
            m_image_resources.clear();
            m_resource_groupings.clear();

            LOG_TRACE << "Vulkan Backend: Finished cleaning up storage";
        }

        uint32_t Storage_Manager::Get_Buffer_Index(Identifier identifier)
        {
            for (uint32_t i = 0; i < m_buffer_resources.size(); i++)
            {
                if (identifier == m_buffer_resources[i].identifier)
                {
                    return i;
                }
            }

            LOG_ERROR << "Vulkan Backend: No buffers named " << identifier.Get_Identifier_String() << " exist";
            exit(EXIT_FAILURE);
        }

        uint32_t Storage_Manager::Get_Image_Index(Identifier identifier)
        {
            for (uint32_t i = 0; i < m_image_resources.size(); i++)
            {
                if (identifier == m_image_resources[i].identifier)
                {
                    return i;
                }
            }

            LOG_ERROR << "Vulkan Backend: No images named " << identifier.Get_Identifier_String() << " exist";
            exit(EXIT_FAILURE);
        }

        uint32_t Storage_Manager::Get_Resource_Grouping_Index(Identifier identifier)
        {
            for (uint32_t i = 0; i < m_resource_groupings.size(); i++)
            {
                if (identifier == m_resource_groupings[i].identifier)
                {
                    return i;
                }
            }

            LOG_ERROR << "Vulkan Backend: No resource groupings named " << identifier.Get_Identifier_String() << " exist";
            exit(EXIT_FAILURE);
        }

        void Storage_Manager::Create_VkBuffer(Identifier identifier)
        {
            LOG_TRACE << "Vulkan Backend: Creating VkBuffer for " << identifier.Get_Identifier_String();

            Buffer_Resource* buffer_resource_ptr = Get_Buffer_Resource(identifier);

            std::vector<uint32_t> unique_queues = m_queue_manager_ptr->Get_Unique_Queue_Families(buffer_resource_ptr->resource_queue_mask);

            VkBufferCreateInfo buffer_create_info = {};
            buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_create_info.pNext = nullptr;
            buffer_create_info.flags = 0;
            buffer_create_info.size = buffer_resource_ptr->buffer_size;
            buffer_create_info.usage = buffer_resource_ptr->buffer_usage;
            buffer_create_info.sharingMode = (unique_queues.size() == 1) ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
            buffer_create_info.queueFamilyIndexCount = static_cast<uint32_t>(unique_queues.size());
            buffer_create_info.pQueueFamilyIndices = unique_queues.data();

            VALIDATE_VKRESULT(vkCreateBuffer(*m_logical_device_wrapper_ptr->Get_Device(), &buffer_create_info, nullptr, &buffer_resource_ptr->buffer), "Vulkan Backend: Failed to create VkBuffer");
        }

        void Storage_Manager::Get_Buffer_Memory_Info(Identifier identifier)
        {
            LOG_TRACE << "Vulkan Backend: Getting memory infomation for " << identifier.Get_Identifier_String();

            Buffer_Resource* buffer_resource_ptr = Get_Buffer_Resource(identifier);

            vkGetBufferMemoryRequirements(*m_logical_device_wrapper_ptr->Get_Device(), buffer_resource_ptr->buffer, &buffer_resource_ptr->memory_requirements);

            int32_t memory_type_index = Physical_Device_Wrapper::Find_Memory(m_physical_device_wrapper_ptr->Get_Physical_Device(), &buffer_resource_ptr->memory_requirements, buffer_resource_ptr->memory_property_flags, true);
            if (memory_type_index == -1)
            {
                LOG_ERROR << "Vulkan Backend: No memory heaps meet memory requirements for buffer " << identifier.Get_Identifier_String();
                exit(EXIT_FAILURE);
            }
            else if (memory_type_index == -2)
            {
                LOG_ERROR << "Vulkan Backend: Required memory for buffer " << identifier.Get_Identifier_String() << "(" << buffer_resource_ptr->memory_requirements.size << " bytes) exeeds the memory budget for all valid heaps";
                exit(EXIT_FAILURE);
            }
            else
            {
                buffer_resource_ptr->memory_type_index = memory_type_index;
            }
        }

        void Storage_Manager::Allocate_Buffer_Memory(Identifier identifier)
        {
            LOG_TRACE << "Vulkan Backend: Allocating memory for buffer " << identifier.Get_Identifier_String();

            Buffer_Resource* buffer_resource_ptr = Get_Buffer_Resource(identifier);

            VkMemoryAllocateInfo memory_allocate_info = {};
            memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            memory_allocate_info.pNext = nullptr;
            memory_allocate_info.allocationSize = buffer_resource_ptr->memory_requirements.size;
            memory_allocate_info.memoryTypeIndex = buffer_resource_ptr->memory_type_index;

            VALIDATE_VKRESULT(vkAllocateMemory(*m_logical_device_wrapper_ptr->Get_Device(), &memory_allocate_info, nullptr, &buffer_resource_ptr->device_memory), "Vulkan Backend: Failed to allocate buffer memory");
            VALIDATE_VKRESULT(vkBindBufferMemory(*m_logical_device_wrapper_ptr->Get_Device(), buffer_resource_ptr->buffer, buffer_resource_ptr->device_memory, 0), "Vulkan Backend: Failed to bind buffer memory");
        }

        void Storage_Manager::Create_VkImage(Identifier identifier)
        {
            LOG_TRACE << "Vulkan Backend: Creating VkImage for image " << identifier.Get_Identifier_String();

            Image_Resource* image_resource_ptr = Get_Image_Resource(identifier);

            std::vector<uint32_t> unique_queues = m_queue_manager_ptr->Get_Unique_Queue_Families(image_resource_ptr->resource_queue_mask);

            VkImageCreateInfo image_create_info = {};
            image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_create_info.pNext = nullptr;
            image_create_info.flags = 0;
            image_create_info.imageType = VK_IMAGE_TYPE_2D;
            image_create_info.format = image_resource_ptr->image_format;
            image_create_info.extent = {image_resource_ptr->image_size.width, image_resource_ptr->image_size.height, 1};
            image_create_info.mipLevels = 1;
            image_create_info.arrayLayers = 1;
            image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
            image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
            image_create_info.usage = image_resource_ptr->image_usage;
            image_create_info.sharingMode = (unique_queues.size() == 1) ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
            image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            VALIDATE_VKRESULT(vkCreateImage(*m_logical_device_wrapper_ptr->Get_Device(), &image_create_info, nullptr, &image_resource_ptr->image), "Vulkan Backend: Failed to create VkImage");
        }

        void Storage_Manager::Get_Image_Memory_Info(Identifier identifier)
        {
            LOG_TRACE << "Vulkan Backend: Getting memory infomation for " << identifier.Get_Identifier_String();

            Image_Resource* image_resource_ptr = Get_Image_Resource(identifier);

            vkGetImageMemoryRequirements(*m_logical_device_wrapper_ptr->Get_Device(), image_resource_ptr->image, &image_resource_ptr->memory_requirements);

            int32_t memory_type_index = Physical_Device_Wrapper::Find_Memory(m_physical_device_wrapper_ptr->Get_Physical_Device(), &image_resource_ptr->memory_requirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true);
            if (memory_type_index == -1)
            {
                LOG_ERROR << "Vulkan Backend: No memory heaps meet memory requirements for image " << identifier.Get_Identifier_String();
                exit(EXIT_FAILURE);
            }
            else if (memory_type_index == -2)
            {
                LOG_ERROR << "Vulkan Backend: Required memory for image " << identifier.Get_Identifier_String() << "(" << image_resource_ptr->memory_requirements.size << " bytes) exeeds the memory budget for all valid heaps";
                exit(EXIT_FAILURE);
            }
            else
            {
                image_resource_ptr->memory_type_index = memory_type_index;
            }
        }

        void Storage_Manager::Allocate_Image_Memory(Identifier identifier)
        {
            LOG_TRACE << "Vulkan Backend: Allocating memory for image " << identifier.Get_Identifier_String();

            Image_Resource* image_resource_ptr = Get_Image_Resource(identifier);

            VkMemoryAllocateInfo memory_allocate_info = {};
            memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            memory_allocate_info.pNext = nullptr;
            memory_allocate_info.allocationSize = image_resource_ptr->memory_requirements.size;
            memory_allocate_info.memoryTypeIndex = image_resource_ptr->memory_type_index;

            VALIDATE_VKRESULT(vkAllocateMemory(*m_logical_device_wrapper_ptr->Get_Device(), &memory_allocate_info, nullptr, &image_resource_ptr->device_memory), "Vulkan Backend: Failed to allocate image memory");
            VALIDATE_VKRESULT(vkBindImageMemory(*m_logical_device_wrapper_ptr->Get_Device(), image_resource_ptr->image, image_resource_ptr->device_memory, 0), "Vulkan Backend: Failed to bind image memory");
        }

        void Storage_Manager::Create_Image_View(Identifier identifier)
        {
            LOG_TRACE << "Vulkan Backend: Creating image view for image " << identifier.Get_Identifier_String();

            Image_Resource* image_resource_ptr = Get_Image_Resource(identifier);

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

            VALIDATE_VKRESULT(vkCreateImageView(*m_logical_device_wrapper_ptr->Get_Device(), &image_view_create_info, nullptr, &image_resource_ptr->image_view), "Vulkan Backend: Failed to create image view");
        }

        Identifier Storage_Manager::Create_Buffer(std::string label, VkDeviceSize buffer_size, VkBufferUsageFlags buffer_usage, VkDescriptorType descriptor_type, VkMemoryPropertyFlags memory_property_flags, uint32_t resource_queue_mask)
        {
            if (buffer_size == 0)
            {
                LOG_DEBUG << "Vulkan Backend: Buffer size is 0. Creating temporary buffer to find maximum buffer size";

                Identifier temp_buffer_id = Create_Buffer(label + "-temp", 1, buffer_usage, descriptor_type, memory_property_flags, resource_queue_mask);
                Buffer_Resource* buffer_resource_ptr = Get_Buffer_Resource(temp_buffer_id);

                VkPhysicalDeviceMemoryBudgetPropertiesEXT device_memory_budget_properties = {};
                device_memory_budget_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT;
                device_memory_budget_properties.pNext = nullptr;

                VkPhysicalDeviceMemoryProperties2 device_memory_properties = {};
                device_memory_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
                device_memory_properties.pNext = &device_memory_budget_properties;

                vkGetPhysicalDeviceMemoryProperties2(*m_physical_device_wrapper_ptr->Get_Physical_Device(), &device_memory_properties);

                uint32_t memory_type_index = buffer_resource_ptr->memory_type_index;
                uint32_t heap_index = device_memory_properties.memoryProperties.memoryTypes[memory_type_index].heapIndex;

                LOG_INFO << "Vulkan Backend: Destroying temporary buffer " << temp_buffer_id.Get_Identifier_String();
                Destroy_Buffer(temp_buffer_id);

                buffer_size = device_memory_budget_properties.heapBudget[heap_index];
            }

            Identifier identifier = {};
            identifier.label = label;
            identifier.index = 0;

            while (true)
            {
                bool index_in_use = false;

                for (uint32_t i = 0; i < m_buffer_resources.size(); i++)
                {
                    index_in_use |= m_buffer_resources[i].identifier == identifier;
                }

                if (!index_in_use)
                {
                    break;
                }
                else
                {
                    identifier.index++;
                }
            }

            LOG_INFO << "Vulkan Backend: Creating buffer " << identifier.Get_Identifier_String();

            m_buffer_resources.resize(m_buffer_resources.size() + 1);
            m_buffer_resources.back() = {};
            m_buffer_resources.back().identifier = identifier;
            m_buffer_resources.back().buffer_size = buffer_size;
            m_buffer_resources.back().descriptor_type = descriptor_type;
            m_buffer_resources.back().buffer_usage = buffer_usage;
            m_buffer_resources.back().memory_property_flags = memory_property_flags;
            m_buffer_resources.back().resource_queue_mask = resource_queue_mask;
            m_buffer_resources.back().buffer = VK_NULL_HANDLE;
            m_buffer_resources.back().device_memory = VK_NULL_HANDLE;
            m_buffer_resources.back().memory_type_index = 0;

            Create_VkBuffer(identifier);
            Get_Buffer_Memory_Info(identifier);
            Allocate_Buffer_Memory(identifier);

            return identifier;
        }

        Identifier Storage_Manager::Create_Image(std::string label, VkFormat image_format, VkImageUsageFlags image_usage, VkDescriptorType descriptor_type, VkExtent2D image_size, uint32_t resource_queue_mask)
        {
            Identifier identifier = {};
            identifier.label = label;
            identifier.index = 0;

            while (true)
            {
                bool index_in_use = false;

                for (uint32_t i = 0; i < m_image_resources.size(); i++)
                {
                    index_in_use |= m_image_resources[i].identifier == identifier;
                }

                if (!index_in_use)
                {
                    break;
                }
                else
                {
                    identifier.index++;
                }
            }

            LOG_INFO << "Vulkan Backend: Creating image " << identifier.Get_Identifier_String();

            m_image_resources.resize(m_image_resources.size() + 1);
            m_image_resources.back() = {};
            m_image_resources.back().identifier = identifier;
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

            Create_VkImage(identifier);
            Get_Image_Memory_Info(identifier);
            Allocate_Image_Memory(identifier);
            Create_Image_View(identifier);

            return identifier;
        }

        Identifier Storage_Manager::Create_Resource_Grouping(std::string label, std::vector<Identifier> resource_identifiers)
        {
            Identifier identifier = {};
            identifier.label = label;
            identifier.index = 0;

            while (true)
            {
                bool index_in_use = false;

                for (uint32_t i = 0; i < m_resource_groupings.size(); i++)
                {
                    index_in_use |= m_resource_groupings[i].identifier == identifier;
                }

                if (!index_in_use)
                {
                    break;
                }
                else
                {
                    identifier.index++;
                }
            }

            LOG_INFO << "Vulkan Backend: Creating resource grouping " << identifier.Get_Identifier_String();

            m_resource_groupings.resize(m_resource_groupings.size() + 1);
            m_resource_groupings.back() = {};
            m_resource_groupings.back().identifier = identifier;
            m_resource_groupings.back().has_descriptor_set = false;

            for (uint32_t i = 0; i < resource_identifiers.size(); i++)
            {
                bool resource_exists = false;
                for (uint32_t j = 0; j < m_buffer_resources.size(); j++)
                {
                    if (resource_identifiers[i] == m_buffer_resources[j].identifier)
                    {
                        resource_exists = true;
                        m_resource_groupings.back().buffer_identifiers.push_back(resource_identifiers[i]);
                        break;
                    }
                }
                for (uint32_t j = 0; j < m_image_resources.size(); j++)
                {
                    if (resource_identifiers[i] == m_image_resources[j].identifier)
                    {
                        resource_exists = true;
                        m_resource_groupings.back().image_identifiers.push_back(resource_identifiers[i]);
                        break;
                    }
                }
                if (!resource_exists)
                {
                    LOG_ERROR << "Vulkan Backend: The resource " << resource_identifiers[i].Get_Identifier_String() << " does not exist";
                    exit(EXIT_FAILURE);
                }
            }

            return identifier;
        }

        Identifier Storage_Manager::Add_Image(std::string label, Image_Resource image_resource)
        {
            Identifier identifier = {};
            identifier.label = label;
            identifier.index = 0;

            while (true)
            {
                bool index_in_use = false;

                for (uint32_t i = 0; i < m_image_resources.size(); i++)
                {
                    index_in_use |= m_image_resources[i].identifier == identifier;
                }

                if (!index_in_use)
                {
                    break;
                }
                else
                {
                    identifier.index++;
                }
            }

            LOG_INFO << "Vulkan Backend: Adding image resource to storage manager (provided identifier was overwritten) with identifier " << identifier.Get_Identifier_String();

            image_resource.identifier = identifier;
            m_image_resources.push_back(image_resource);

            return identifier;
        }

        void Storage_Manager::Destroy_Buffer(Identifier identifier)
        {
            LOG_TRACE << "Vulkan Backend: Destroying buffer " << identifier.Get_Identifier_String();

            uint32_t buffer_index = Get_Buffer_Index(identifier);
            Buffer_Resource* buffer_resource_ptr = &m_buffer_resources[buffer_index];

            vkDestroyBuffer(*m_logical_device_wrapper_ptr->Get_Device(), buffer_resource_ptr->buffer, nullptr);
            vkFreeMemory(*m_logical_device_wrapper_ptr->Get_Device(), buffer_resource_ptr->device_memory, nullptr);
            m_buffer_resources.erase(m_buffer_resources.begin() + buffer_index);
        }

        void Storage_Manager::Destroy_Image(Identifier identifier)
        {
            LOG_TRACE << "Vulkan Backend: Destroying image " << identifier.Get_Identifier_String();

            uint32_t image_index = Get_Image_Index(identifier);
            Image_Resource* image_resource_ptr = &m_image_resources[image_index];

            if (!image_resource_ptr->is_swapchain_image)
            {
                vkDestroyImage(*m_logical_device_wrapper_ptr->Get_Device(), image_resource_ptr->image, nullptr);
                vkDestroyImageView(*m_logical_device_wrapper_ptr->Get_Device(), image_resource_ptr->image_view, nullptr);
                vkFreeMemory(*m_logical_device_wrapper_ptr->Get_Device(), image_resource_ptr->device_memory, nullptr);
            }

            m_image_resources.erase(m_image_resources.begin() + image_index);
        }

        void Storage_Manager::Remove_Resource_Grouping(Identifier identifier)
        {
            LOG_TRACE << "Vulkan Backend: Removing resource grouping " << identifier.Get_Identifier_String();

            uint32_t resource_grouping_index = Get_Resource_Grouping_Index(identifier);
            m_resource_groupings.erase(m_resource_groupings.begin() + resource_grouping_index);
        }

        void Storage_Manager::Resize_Buffer(Identifier identifier, VkDeviceSize buffer_size)
        {
            LOG_TRACE << "Vulkan Backend: Resizing buffer " << identifier.Get_Identifier_String();

            Buffer_Resource* buffer_resource_ptr = Get_Buffer_Resource(identifier);

            vkDestroyBuffer(*m_logical_device_wrapper_ptr->Get_Device(), buffer_resource_ptr->buffer, nullptr);
            vkFreeMemory(*m_logical_device_wrapper_ptr->Get_Device(), buffer_resource_ptr->device_memory, nullptr);

            buffer_resource_ptr->buffer_size = buffer_size;
            buffer_resource_ptr->buffer = VK_NULL_HANDLE;
            buffer_resource_ptr->device_memory = VK_NULL_HANDLE;
            buffer_resource_ptr->memory_type_index = 0;

            Create_VkBuffer(identifier);
            Get_Buffer_Memory_Info(identifier);
            Allocate_Buffer_Memory(identifier);
        }

        void Storage_Manager::Upload_To_Buffer_Direct(Identifier identifier, void* data, size_t data_size)
        {
            Buffer_Resource* buffer_resource_ptr = Get_Buffer_Resource(identifier);

            if (buffer_resource_ptr->memory_property_flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
            {
                void* mapped_memory;
                VALIDATE_VKRESULT(vkMapMemory(*m_logical_device_wrapper_ptr->Get_Device(), buffer_resource_ptr->device_memory, 0, data_size, 0, &mapped_memory), "Vulkan Backend: Failed to map memory");

                memcpy(mapped_memory, data, data_size);

                vkUnmapMemory(*m_logical_device_wrapper_ptr->Get_Device(), buffer_resource_ptr->device_memory);
            }
            else
            {
                LOG_ERROR << "Vulkan Backend: Cannot upload to non host-visible buffers";
                exit(EXIT_FAILURE);
            }
        }

        void Storage_Manager::Upload_To_Buffer_Staging(Identifier identifier, Identifier staging_buffer_identifier, void* data, size_t data_size, std::shared_ptr<Vulkan_Graphics> vulkan_graphics)
        {
            VALIDATE_VKRESULT(vkDeviceWaitIdle(*m_logical_device_wrapper_ptr->Get_Device()), "Vulkan Backend: Failed to wait for idle device");

            std::chrono::time_point<std::chrono::high_resolution_clock> start_time = std::chrono::high_resolution_clock::now(); //

            Buffer_Resource* staging_buffer = Get_Buffer_Resource(staging_buffer_identifier);

            if (!(staging_buffer->memory_property_flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
            {
                LOG_ERROR << "Vulkan Backend: Staging buffer must have memory property VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT";
                exit(EXIT_FAILURE);
            }

            Identifier resource_grouping_identifier = Create_Resource_Grouping("staging-buffer-upload", {identifier, staging_buffer_identifier});
            Identifier descriptor_set_identifier = vulkan_graphics->m_descriptor_set_manager_ptr->Create_Descriptor_Set(resource_grouping_identifier);
            Identifier uploading_fence_identifier = vulkan_graphics->m_synchronization_manager_ptr->Create_Fence("currently_uploading_fence");

            Identifier empty_pipeline_identifier = {"", 0};

            size_t uploaded = 0;
            size_t max_upload_size = staging_buffer->buffer_size;

            Identifier command_buffer_identifier = vulkan_graphics->m_command_buffer_manager_ptr->Add_Command_Buffer("staging-buffer-upload", m_queue_manager_ptr->Get_Queue_Family_Index(Queue_Manager::Queue_Types::TRANSFER_QUEUE),
                                                                                                                     {resource_grouping_identifier}, empty_pipeline_identifier);

            while (uploaded < data_size)
            {
                size_t upload_size = std::min<size_t>(data_size - uploaded, max_upload_size);

                VALIDATE_VKRESULT(vkWaitForFences(*vulkan_graphics->m_logical_device_wrapper_ptr->Get_Device(), 1, vulkan_graphics->m_synchronization_manager_ptr->Get_Fence(uploading_fence_identifier), VK_TRUE, UINT64_MAX),
                                  "Vulkan Backend: Failed to wait for fence");
                VALIDATE_VKRESULT(vkResetFences(*vulkan_graphics->m_logical_device_wrapper_ptr->Get_Device(), 1, vulkan_graphics->m_synchronization_manager_ptr->Get_Fence(uploading_fence_identifier)), "Vulkan Backend: Failed to reset fence");

                void* mapped_memory;
                VALIDATE_VKRESULT(vkMapMemory(*m_logical_device_wrapper_ptr->Get_Device(), staging_buffer->device_memory, 0, upload_size, 0, &mapped_memory), "Vulkan Backend: Failed to map memory");
                memcpy(((uint8_t*)mapped_memory), ((uint8_t*)data) + uploaded, upload_size);
                vkUnmapMemory(*m_logical_device_wrapper_ptr->Get_Device(), staging_buffer->device_memory);

                vulkan_graphics->m_command_buffer_manager_ptr->Reset_Command_Buffer(command_buffer_identifier);

                vulkan_graphics->m_command_buffer_manager_ptr->Begin_Recording(command_buffer_identifier, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
                vulkan_graphics->m_command_buffer_manager_ptr->Copy_Buffer(command_buffer_identifier, staging_buffer_identifier, identifier, 0, uploaded, upload_size);
                vulkan_graphics->m_command_buffer_manager_ptr->End_Recording(command_buffer_identifier);

                VkSubmitInfo submit_info = {};
                submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submit_info.pNext = nullptr;
                submit_info.waitSemaphoreCount = 0;
                submit_info.pWaitSemaphores = nullptr;
                submit_info.pWaitDstStageMask = nullptr;
                submit_info.commandBufferCount = 1;
                submit_info.pCommandBuffers = vulkan_graphics->m_command_buffer_manager_ptr->Get_Command_Buffer(command_buffer_identifier);
                submit_info.signalSemaphoreCount = 0;
                submit_info.pSignalSemaphores = nullptr;

                VALIDATE_VKRESULT(vkQueueSubmit(*m_queue_manager_ptr->Get_Queue(Queue_Manager::Queue_Types::TRANSFER_QUEUE), 1, &submit_info, *vulkan_graphics->m_synchronization_manager_ptr->Get_Fence(uploading_fence_identifier)),
                                  "Vulkan Backend: Failed to submit staging buffer upload command buffer");

                uploaded += upload_size;
            }

            vulkan_graphics->m_descriptor_set_manager_ptr->Remove_Descriptor_Set(descriptor_set_identifier);
            Remove_Resource_Grouping(resource_grouping_identifier);
            vulkan_graphics->m_command_buffer_manager_ptr->Remove_Command_Buffer(command_buffer_identifier);

            VALIDATE_VKRESULT(vkDeviceWaitIdle(*m_logical_device_wrapper_ptr->Get_Device()), "Failed to wait for idle device");

            LOG_DEBUG << "Time to upload: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time).count() / 1000.0 << " seconds";

            LOG_INFO << "Vulkan Backend: Finished staging buffer upload";
        }

        Storage_Manager::Buffer_Resource* Storage_Manager::Get_Buffer_Resource(Identifier identifier)
        {
            return &m_buffer_resources[Get_Buffer_Index(identifier)];
        }

        Storage_Manager::Image_Resource* Storage_Manager::Get_Image_Resource(Identifier identifier)
        {
            return &m_image_resources[Get_Image_Index(identifier)];
        }

        Storage_Manager::Resource_Grouping* Storage_Manager::Get_Resource_Grouping(Identifier identifier)
        {
            return &m_resource_groupings[Get_Resource_Grouping_Index(identifier)];
        }
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics