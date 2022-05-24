#pragma once

#include "../vulkan_header.hpp"

#include "device_wrapper.hpp"
#include "physical_device_wrapper.hpp"
#include "queue_wrapper.hpp"
#include "swapchain_wrapper.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        struct Resouce_Queue_Families
        {
            bool use_graphics;
            bool use_compute;
            bool use_transfer;
            bool use_sparse_binding;
            bool use_protected;
            bool use_present;
        };

        class Storage_Manager
        {
        public:
            enum Resource_Type
            {
                BUFFER,
                IMAGE,
                SWAPCHAIN_IMAGE
            };

            struct Resource_ID
            {
                unsigned int index;
                std::string label;
                Resource_Type type;

                bool operator==(Resource_ID other_id)
                {
                    return index == other_id.index && label == other_id.label && type == other_id.type;
                }
            };

            struct Resource_Data
            {
                Resource_ID resource_id;

                VkDescriptorType descriptor_type;
            };

            struct Buffer_Resource
            {
                Resource_ID resource_id;

                VkBuffer buffer;
                VkDeviceMemory buffer_memory;
                VkDescriptorType descriptor_type;
            };

            struct Image_Info
            {
                VkImage image;
                VkImageView image_view;
                VkDeviceMemory image_memory;
                VkDescriptorType descriptor_type;
            };

            struct Image_Resource
            {
                Resource_ID resource_id;

                std::optional<Image_Info> image_info;
            };

        private:
            std::shared_ptr<Device> m_logical_device_ptr;
            std::shared_ptr<Physical_Device> m_physical_device_ptr;
            std::shared_ptr<Queue_Manager> m_queue_manager_ptr;
            std::shared_ptr<Swapchain> m_swapchain_ptr;

            std::vector<Buffer_Resource> m_buffers;
            std::vector<Image_Resource> m_images;

        private:
            unsigned int Get_Next_Buffer_Id(std::string label);
            unsigned int Get_Next_Image_Id(std::string label);
            std::vector<unsigned int> Get_Queue_Families(Resouce_Queue_Families resouce_queue_families);

        public:
            Storage_Manager(std::shared_ptr<Device> logical_device_ptr, std::shared_ptr<Physical_Device> physical_device_ptr, std::shared_ptr<Queue_Manager> queue_manager_ptr, std::shared_ptr<Swapchain> swapchain_ptr);
            ~Storage_Manager();

        public:
            void Create_Buffer(std::string label, VkDeviceSize buffer_size, VkBufferUsageFlagBits buffer_usage, VkDescriptorType buffer_type, Resouce_Queue_Families resouce_queue_families);
            void Create_Image(std::string label, VkFormat image_format, VkImageUsageFlags image_usage, VkDescriptorType image_type, VkExtent2D image_size, Resouce_Queue_Families resouce_queue_families);

            void Add_Swapchain(std::string label);

            VkImage* Get_Image(Resource_ID resource_id);
            VkImageView* Get_Image_View(Resource_ID resource_id);
            VkBuffer* Get_Buffer(Resource_ID resource_id);

            Resource_Data Get_Resource_Data(Resource_ID resource_id);
            bool Does_Resource_Exist(Resource_ID resource_id);

            void Upload_To_Buffer(Resource_ID resource_id, void* data, size_t data_size);
        };
    } // namespace Vulkan
} // namespace Cascade_Graphics