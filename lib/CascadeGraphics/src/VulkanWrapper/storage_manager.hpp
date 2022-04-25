#pragma once

#include "../vulkan_header.hpp"

#include "device_wrapper.hpp"
#include "physical_device_wrapper.hpp"
#include "queue_wrapper.hpp"

#include <memory>
#include <vector>

namespace CascadeGraphics
{
    namespace Vulkan
    {
        struct Buffer
        {
            const char* label;
            unsigned int id;

            VkBuffer buffer;
            VkDeviceMemory buffer_memory;
        };

        struct Image
        {
            const char* label;
            unsigned int id;

            VkImage image;
            VkImageView image_view;
            VkDeviceMemory image_memory;
        };

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
        private:
            std::shared_ptr<Device> m_logical_device_ptr;
            std::shared_ptr<Physical_Device> m_physical_device_ptr;
            std::shared_ptr<Queue_Manager> m_queue_manager_ptr;

            std::vector<Buffer> m_buffers;
            std::vector<Image> m_images;

        private:
            unsigned int Get_Next_Buffer_Id(const char* label);
            unsigned int Get_Next_Image_Id(const char* label);
            std::vector<unsigned int> Get_Queue_Families(Resouce_Queue_Families resouce_queue_families);

        public:
            Storage_Manager(std::shared_ptr<Device> logical_device_ptr, std::shared_ptr<Physical_Device> physical_device_ptr, std::shared_ptr<Queue_Manager> queue_manager_ptr);
            ~Storage_Manager();

            void Create_Buffer(const char* label, VkDeviceSize buffer_size, VkBufferUsageFlagBits buffer_usage, Resouce_Queue_Families resouce_queue_families);
            void Create_Image(const char* label, VkFormat image_format, VkImageUsageFlags image_usage, VkExtent2D image_size, Resouce_Queue_Families resouce_queue_families);
        };
    } // namespace Vulkan
} // namespace CascadeGraphics