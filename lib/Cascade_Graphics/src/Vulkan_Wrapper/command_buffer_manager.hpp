#pragma once

#include "descriptor_set_manager.hpp"
#include "device_wrapper.hpp"
#include "pipeline_manager.hpp"
#include "storage_manager.hpp"
#include "vulkan_header.hpp"

#include <memory>
#include <string>
#include <vector>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Command_Buffer_Manager
        {
        public:
            struct Identifier
            {
                std::string label;
                unsigned int index;

                bool operator==(Identifier other_identifier)
                {
                    return label == other_identifier.label && index == other_identifier.index;
                }
            };

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
                Identifier identifier;

                std::vector<std::string> resource_group_labels;
                std::string pipeline_label;

                VkCommandBuffer command_buffer;
                std::vector<Image_Resource_State> image_resource_states;
            };

        private:
            std::shared_ptr<Descriptor_Set_Manager> m_descriptor_set_manager;
            std::shared_ptr<Device> m_logical_device_ptr;
            std::shared_ptr<Pipeline_Manager> m_pipeline_manager_ptr;
            std::shared_ptr<Storage_Manager> m_storage_manager_ptr;

            std::vector<Command_Pool> m_command_pools;
            std::vector<Command_Buffer> m_command_buffers;

        private:
            void Create_Command_Pool(unsigned int queue_family);
            void Allocate_Command_Buffer(unsigned int command_buffer_index, unsigned int command_pool_index);
            unsigned int Get_Command_Buffer_Index(Identifier identifier);
            unsigned int Get_Next_Index(std::string label);

        public:
            Command_Buffer_Manager(std::shared_ptr<Descriptor_Set_Manager> descriptor_set_manager,
                                   std::shared_ptr<Device> logical_device_ptr,
                                   std::shared_ptr<Pipeline_Manager> pipeline_manager_ptr,
                                   std::shared_ptr<Storage_Manager> storage_manager_ptr);
            ~Command_Buffer_Manager();

        public:
            void Add_Command_Buffer(std::string label, unsigned int queue_family, std::vector<std::string> resource_group_labels, std::string pipeline_label);

            void Begin_Recording(Identifier identifier, VkCommandBufferUsageFlagBits usage_flags);
            void End_Recording(Identifier identifier);
            void Image_Memory_Barrier(Identifier identifier, Storage_Manager::Resource_ID resource_id, VkAccessFlags access_flags, VkImageLayout image_layout, VkPipelineStageFlags pipeline_stage_flags);
            void Dispatch_Compute_Shader(Identifier identifier, unsigned int group_count_x, unsigned int group_count_y, unsigned int group_count_z);
            void Copy_Image(Identifier identifier, Storage_Manager::Resource_ID source_resource_id, Storage_Manager::Resource_ID destination_resource_id, unsigned int width, unsigned int height);

            VkCommandBuffer* Get_Command_Buffer(Identifier identifier);
        };
    } // namespace Vulkan
} // namespace Cascade_Graphics