#pragma once

#include "identifier.hpp"
#include "logical_device_wrapper.hpp"
#include "storage_manager.hpp"
#include "vulkan_header.hpp"
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
                Identifier identifier;
                Identifier resource_grouping_identifier;

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

            std::shared_ptr<Logical_Device_Wrapper> m_logical_device_wrapper_ptr;
            std::shared_ptr<Storage_Manager> m_storage_manager_ptr;

        private:
            void Create_Descriptor_Set_Layout(Identifier identifier);
            void Create_Descriptor_Pool(Identifier identifier);
            void Allocate_Descriptor_Set(Identifier identifier);
            void Create_Write_Descriptor_Set(Identifier identifier);

        public:
            Descriptor_Set_Manager(std::shared_ptr<Logical_Device_Wrapper> logical_device_wrapper_ptr, std::shared_ptr<Storage_Manager> storage_manager_ptr);
            ~Descriptor_Set_Manager();

        public:
            Identifier Create_Descriptor_Set(Identifier resource_grouping_identifier);
            void Remove_Descriptor_Set(Identifier identifier);

            Descriptor_Set_Data* Get_Descriptor_Set_Data(Identifier identifier);
        };
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics