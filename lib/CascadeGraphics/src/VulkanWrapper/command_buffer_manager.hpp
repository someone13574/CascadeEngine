#pragma once

#include "../vulkan_header.hpp"
#include "device_wrapper.hpp"
#include "pipeline_manager.hpp"
#include "resource_grouping_manager.hpp"
#include "storage_manager.hpp"

#include <memory>
#include <string>
#include <vector>

namespace CascadeGraphics
{
    namespace Vulkan
    {
        class Command_Buffer_Manager
        {
        private:
            struct Image_Resource_State
            {
                Storage_Manager::Resource_ID resource_id;

                VkAccessFlags previous_access_flags;
                VkAccessFlags current_access_flags;
                VkImageLayout previous_image_layout;
                VkImageLayout current_image_layout;
                VkPipelineStageFlags previous_pipeline_stage_flags;
                VkPipelineStageFlags current_pipeline_stage_flags;
            };

            struct Command_Pool
            {
                unsigned int queue_family;
                VkCommandPool command_pool;
            };

            struct Command_Buffer
            {
                std::string label;
                std::vector<std::string> resource_group_labels;
                std::string pipeline_label;

                VkCommandBuffer command_buffer;
                std::vector<Image_Resource_State> image_resource_states;
            };

        private:
            std::shared_ptr<Resource_Grouping_Manager> m_resource_grouping_manager_ptr;
            std::shared_ptr<Device> m_logical_device_ptr;
            std::shared_ptr<Pipeline_Manager> m_pipeline_manager_ptr;
            std::shared_ptr<Storage_Manager> m_storage_manager_ptr;

            std::vector<Command_Pool> m_command_pools;
            std::vector<Command_Buffer> m_command_buffers;

        private:
            void Create_Command_Pool(unsigned int queue_family);
            void Allocate_Command_Buffer(unsigned int command_buffer_index, unsigned int command_pool_index);
            unsigned int Get_Command_Buffer_Index(std::string label);

        public:
            Command_Buffer_Manager(std::shared_ptr<Resource_Grouping_Manager> resource_grouping_manager_ptr,
                                   std::shared_ptr<Device> logical_device_ptr,
                                   std::shared_ptr<Pipeline_Manager> pipeline_manager_ptr,
                                   std::shared_ptr<Storage_Manager> storage_manager_ptr);
            ~Command_Buffer_Manager();

        public:
            void Add_Command_Buffer(std::string label, unsigned int queue_family, std::vector<std::string> resource_group_labels, std::string pipeline_label);

            void Begin_Recording(std::string label, VkCommandBufferUsageFlagBits usage_flags);
            void End_Recording(std::string label);
            void Image_Memory_Barrier(std::string command_buffer_label, Storage_Manager::Resource_ID resource_id, VkAccessFlags access_flags, VkImageLayout image_layout, VkPipelineStageFlags pipeline_stage_flags);
            void Dispatch_Compute_Shader(std::string command_buffer_label, unsigned int group_count_x, unsigned int group_count_y, unsigned int group_count_z);
        };
    } // namespace Vulkan
} // namespace CascadeGraphics