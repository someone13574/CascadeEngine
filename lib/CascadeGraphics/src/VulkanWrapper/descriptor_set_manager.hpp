#pragma once

#include "../vulkan_header.hpp"
#include "device_wrapper.hpp"
#include "storage_manager.hpp"

#include <memory>
#include <string>
#include <vector>

namespace CascadeGraphics
{
    namespace Vulkan
    {
        class Descriptor_Set_Manager
        {
        private:
            struct Resource_Grouping
            {
                std::string label;
                VkDescriptorSet* descriptor_set_ptr;
                VkDescriptorSetLayout* descriptor_set_layout_ptr;
                std::vector<Storage_Manager::Resource_ID> resources;
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

        public:
            Descriptor_Set_Manager(std::shared_ptr<Device> logical_device_ptr, std::shared_ptr<Storage_Manager> storage_manager_ptr);
            ~Descriptor_Set_Manager();

        public:
            void Add_Descriptor_Set(std::string label, std::vector<Storage_Manager::Resource_ID> resources);
            void Allocate_Descriptor_Sets();

            VkDescriptorSet* Get_Descriptor_Set(std::string label);
            VkDescriptorSetLayout* Get_Descriptor_Set_Layout(std::string label);
        };
    } // namespace Vulkan
} // namespace CascadeGraphics