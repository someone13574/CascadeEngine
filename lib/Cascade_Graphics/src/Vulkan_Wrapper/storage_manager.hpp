#pragma once


#include "vulkan_header.hpp"

#include "device_wrapper.hpp"
#include "physical_device_wrapper.hpp"
#include "queue_manager.hpp"

#include <string>
#include <vector>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Storage_Manager
        {
        public:
            struct Resource_ID
            {
                std::string label;
                unsigned int index;

                enum Resource_Type
                {
                    BUFFER_RESOURCE,
                    IMAGE_RESOURCE
                } resource_type;

                bool operator==(Resource_ID other_id)
                {
                    return index == other_id.index && label == other_id.label && resource_type == other_id.resource_type;
                }
            };

            struct Buffer_Resource
            {
                Resource_ID resource_id;

                VkDeviceSize buffer_size;
                VkDescriptorType descriptor_type;
                VkBufferUsageFlagBits buffer_usage;
                unsigned int resource_queue_mask;

                VkBuffer buffer;
                VkDeviceMemory device_memory;
                VkMemoryRequirements memory_requirements;
                unsigned int memory_type_index;
            };

            struct Image_Resource
            {
                Resource_ID resource_id;

                VkFormat image_format;
                VkImageUsageFlags image_usage;
                VkDescriptorType descriptor_type;
                VkExtent2D image_size;
                unsigned int resource_queue_mask;

                VkImage image;
                VkImageView image_view;
                VkDeviceMemory device_memory;
                VkMemoryRequirements memory_requirements;
                unsigned int memory_type_index;
            };

            struct Resource_Grouping
            {
                std::string label;

                bool has_descriptor_set;
                unsigned int buffer_resource_count;
                unsigned int image_resource_count;
                std::vector<Resource_ID> resource_ids;
            };

        private:
            std::vector<Buffer_Resource> m_buffer_resources;
            std::vector<Image_Resource> m_image_resources;
            std::vector<Resource_Grouping> m_resource_groupings;

            std::shared_ptr<Device> m_logical_device_ptr;
            std::shared_ptr<Physical_Device> m_physical_device_ptr;
            std::shared_ptr<Queue_Manager> m_queue_manager_ptr;

        private:
            std::string Get_Resource_String(Resource_ID resource_id);
            unsigned int Get_Buffer_Index(Resource_ID resource_id);
            unsigned int Get_Image_Index(Resource_ID resource_id);

            void Create_VkBuffer(Resource_ID resource_id);
            void Get_Buffer_Memory_Info(Resource_ID resource_id);
            void Allocate_Buffer_Memory(Resource_ID resource_id);

            void Create_VkImage(Resource_ID resource_id);
            void Get_Image_Memory_Info(Resource_ID resource_id);
            void Allocate_Image_Memory(Resource_ID resource_id);
            void Create_Image_View(Resource_ID resource_id);

        public:
            Storage_Manager(std::shared_ptr<Device> logical_device_ptr, std::shared_ptr<Physical_Device> physical_device_ptr, std::shared_ptr<Queue_Manager> queue_manager_ptr);

        public:
            void Create_Buffer(std::string label, VkDeviceSize buffer_size, VkBufferUsageFlagBits buffer_usage, VkDescriptorType descriptor_type, unsigned int resource_queue_mask);
            void Create_Image(std::string label, VkFormat image_format, VkImageUsageFlags image_usage, VkDescriptorType descriptor_type, VkExtent2D image_size, unsigned int resource_queue_mask);
            void Create_Resource_Grouping(std::string label, std::vector<Resource_ID> resource_ids);

            void Add_Image(std::string label, Image_Resource image_resource);

            void Upload_To_Buffer(Resource_ID resource_id, void* data, size_t data_size);

            Buffer_Resource* Get_Buffer_Resource(Resource_ID resource_id);
            Image_Resource* Get_Image_Resource(Resource_ID resource_id);
            Resource_Grouping* Get_Resource_Grouping(std::string label);
        };
    } // namespace Vulkan
} // namespace Cascade_Graphics