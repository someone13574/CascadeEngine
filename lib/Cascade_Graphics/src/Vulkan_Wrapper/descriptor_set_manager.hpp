#pragma once

#include "vulkan_header.hpp"

#include "device_wrapper.hpp"
#include "storage_manager.hpp"
#include <memory>
#include <string>
#include <vector>


namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        class Storage_Manager;

        class Descriptor_Set_Manager
        {
        public:
            struct Descriptor_Set_Data
            {
                std::string label;

                VkDescriptorSet descriptor_set;
                VkDescriptorPool descriptor_pool;
                VkDescriptorSetLayout descriptor_set_layout;

                std::vector<VkWriteDescriptorSet> write_descriptor_sets;
                std::vector<VkDescriptorBufferInfo> buffer_descriptor_infos;
                std::vector<VkDescriptorImageInfo> image_descriptor_infos;
            };

        private:
            std::vector<Descriptor_Set_Data> m_descriptor_sets;
            std::vector<VkSampler> m_samplers;

            std::shared_ptr<Device> m_logical_device_ptr;
            std::shared_ptr<Storage_Manager> m_storage_manager_ptr;

        private:
            void Create_Descriptor_Set_Layout(std::string label);
            void Create_Descriptor_Pool(std::string label);
            void Allocate_Descriptor_Set(std::string label);
            void Create_Write_Descriptor_Set(std::string label);

        public:
            Descriptor_Set_Manager(std::shared_ptr<Device> logical_device_ptr, std::shared_ptr<Storage_Manager> storage_manager_ptr);
            ~Descriptor_Set_Manager();

        public:
            void Create_Descriptor_Set(std::string resource_grouping_label);
            void Remove_Descriptor_Set(std::string resource_grouping_label);

            Descriptor_Set_Data* Get_Descriptor_Set_Data(std::string label);
        };
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics