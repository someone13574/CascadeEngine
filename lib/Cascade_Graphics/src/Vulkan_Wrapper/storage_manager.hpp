#pragma once

#include "vulkan_header.hpp"

#include "identifier.hpp"
#include "logical_device_wrapper.hpp"
#include "physical_device_wrapper.hpp"
#include "queue_manager.hpp"
#include "vulkan_graphics.hpp"
#include <string>
#include <vector>


namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        class Vulkan_Graphics;

        class Storage_Manager
        {
        public:
            struct Buffer_Resource
            {
                Identifier identifier;

                VkDeviceSize buffer_size;
                VkDescriptorType descriptor_type;
                VkBufferUsageFlags buffer_usage;
                VkMemoryPropertyFlags memory_property_flags;
                uint32_t resource_queue_mask;

                VkBuffer buffer;
                VkDeviceMemory device_memory;
                VkMemoryRequirements memory_requirements;
                uint32_t memory_type_index;
            };

            struct Image_Resource
            {
                Identifier identifier;

                bool is_swapchain_image;
                VkFormat image_format;
                VkImageUsageFlags image_usage;
                VkDescriptorType descriptor_type;
                VkExtent2D image_size;
                uint32_t resource_queue_mask;

                VkImage image;
                VkImageView image_view;
                VkDeviceMemory device_memory;
                VkMemoryRequirements memory_requirements;
                uint32_t memory_type_index;
            };

            struct Resource_Grouping
            {
                Identifier identifier;

                bool has_descriptor_set;
                std::vector<Identifier> buffer_identifiers;
                std::vector<Identifier> image_identifiers;
            };

        private:
            std::vector<Buffer_Resource> m_buffer_resources;
            std::vector<Image_Resource> m_image_resources;
            std::vector<Resource_Grouping> m_resource_groupings;

            std::shared_ptr<Logical_Device_Wrapper> m_logical_device_wrapper_ptr;
            std::shared_ptr<Physical_Device_Wrapper> m_physical_device_wrapper_ptr;
            std::shared_ptr<Queue_Manager> m_queue_manager_ptr;

        private:
            uint32_t Get_Buffer_Index(Identifier identifier);
            uint32_t Get_Image_Index(Identifier identifier);
            uint32_t Get_Resource_Grouping_Index(Identifier identifier);

            void Create_VkBuffer(Identifier identifier);
            void Get_Buffer_Memory_Info(Identifier identifier);
            void Allocate_Buffer_Memory(Identifier identifier);

            void Create_VkImage(Identifier identifier);
            void Get_Image_Memory_Info(Identifier identifier);
            void Allocate_Image_Memory(Identifier identifier);
            void Create_Image_View(Identifier identifier);

        public:
            Storage_Manager(std::shared_ptr<Logical_Device_Wrapper> logical_device_wrapper_ptr, std::shared_ptr<Physical_Device_Wrapper> physical_device_wrapper_ptr, std::shared_ptr<Queue_Manager> queue_manager_ptr);
            ~Storage_Manager();

        public:
            Identifier Create_Buffer(std::string label, VkDeviceSize buffer_size, bool strict_buffer_size, VkBufferUsageFlags buffer_usage, VkDescriptorType descriptor_type, VkMemoryPropertyFlags memory_property_flags, uint32_t resource_queue_mask);
            Identifier Create_Image(std::string label, VkFormat image_format, VkImageUsageFlags image_usage, VkDescriptorType descriptor_type, VkExtent2D image_size, uint32_t resource_queue_mask);
            Identifier Create_Resource_Grouping(std::string label, std::vector<Identifier> resource_identifiers);
            Identifier Add_Image(std::string label, Image_Resource image_resource);

            void Destroy_Buffer(Identifier identifier);
            void Destroy_Image(Identifier identifier);
            void Remove_Resource_Grouping(Identifier identifier);

            void Resize_Buffer(Identifier identifier, VkDeviceSize buffer_size);
            void Upload_To_Buffer_Direct(Identifier identifier, void* data, size_t data_size);
            void Upload_To_Buffer_Staging(Identifier identifier, Identifier staging_buffer_identifier, void* data, size_t data_size, std::shared_ptr<Vulkan_Graphics> vulkan_graphics);
            void Download_From_Buffer_Staging(Identifier identifier, Identifier staging_buffer_identifier, void* data, size_t data_size, std::shared_ptr<Vulkan_Graphics> vulkan_graphics);

            Buffer_Resource* Get_Buffer_Resource(Identifier identifier);
            Image_Resource* Get_Image_Resource(Identifier identifier);
            Resource_Grouping* Get_Resource_Grouping(Identifier identifier);
        };
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics