#pragma once

#include "../vulkan_header.hpp"
#include "device_wrapper.hpp"
#include "storage_manager.hpp"

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Resource_Grouping_Manager
        {
        private:
            struct Descriptor_Set_Info
            {
                VkDescriptorSet* descriptor_set_ptr;
                VkDescriptorSetLayout* descriptor_set_layout_ptr;

                std::vector<VkWriteDescriptorSet> write_descriptor_sets;
            };

            struct Resource_Grouping
            {
                std::string label;

                std::vector<Storage_Manager::Resource_ID> resources;
                std::optional<Descriptor_Set_Info> descriptor_set_info;
            };

        private:
            std::shared_ptr<Device> m_logical_device_ptr;
            std::shared_ptr<Storage_Manager> m_storage_manager_ptr;

            std::vector<Resource_Grouping> m_resource_groupings;

            bool m_descriptor_sets_allocated = false;
            VkDescriptorPool m_descriptor_pool;
            std::vector<VkDescriptorSet> m_descriptor_sets;
            std::vector<VkDescriptorSetLayout> m_descriptor_set_layouts;

        private:
            void Create_Descriptor_Pool();
            void Allocate_Descriptor_Sets();
            void Create_Write_Descriptor_Sets();

        public:
            Resource_Grouping_Manager(std::shared_ptr<Device> logical_device_ptr, std::shared_ptr<Storage_Manager> storage_manager_ptr);
            ~Resource_Grouping_Manager();

        public:
            void Add_Resource_Grouping(std::string label, std::vector<Storage_Manager::Resource_ID> resources, bool add_descriptor_set);
            void Create_Descriptor_Sets();

            bool Resource_Group_Has_Descriptor_Set(std::string resource_group_label);
            VkDescriptorSet* Get_Descriptor_Set(std::string resource_grouping_label);
            VkDescriptorSetLayout* Get_Descriptor_Set_Layout(std::string resource_grouping_label);
            std::vector<Storage_Manager::Resource_ID> Get_Resources(std::string resource_grouping_label);
        };
    } // namespace Vulkan
} // namespace Cascade_Graphics