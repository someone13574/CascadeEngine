#pragma once

#include "descriptor_set_manager.hpp"
#include "logical_device_wrapper.hpp"
#include "pipeline_manager.hpp"
#include "storage_manager.hpp"
#include "storage_manager_resource_id.hpp"
#include "vulkan_header.hpp"
#include <memory>
#include <string>
#include <vector>


namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        class Storage_Manager;
        class Descriptor_Set_Manager;
        class Pipeline_Manager;

        class Command_Buffer_Manager
        {
        public:
            struct Identifier
            {
                std::string label;
                uint32_t index;

                bool operator==(Identifier other_identifier)
                {
                    return label == other_identifier.label && index == other_identifier.index;
                }
            };

        private:
            struct Image_Resource_State
            {
                Resource_ID resource_id;

                VkAccessFlags previous_access_flags;
                VkAccessFlags current_access_flags;
                VkImageLayout previous_image_layout;
                VkImageLayout current_image_layout;
                VkPipelineStageFlags previous_pipeline_stage_flags;
                VkPipelineStageFlags current_pipeline_stage_flags;
            };

            struct Command_Pool_Data
            {
                uint32_t queue_family;
                VkCommandPool command_pool;
            };

            struct Command_Buffer_Data
            {
                Identifier identifier;

                std::string pipeline_label;
                std::vector<std::string> resource_group_labels;

                VkCommandBuffer command_buffer;
                std::vector<Image_Resource_State> image_resource_states;
            };

        private:
            std::vector<Command_Pool_Data> m_command_pools;
            std::vector<Command_Buffer_Data> m_command_buffers;

            std::shared_ptr<Descriptor_Set_Manager> m_descriptor_set_manager;
            std::shared_ptr<Logical_Device_Wrapper> m_logical_device_wrapper_ptr;
            std::shared_ptr<Pipeline_Manager> m_pipeline_manager_ptr;
            std::shared_ptr<Storage_Manager> m_storage_manager_ptr;

        private:
            void Create_Command_Pool(uint32_t queue_family);
            void Allocate_Command_Buffer(uint32_t command_buffer_index, uint32_t command_pool_index);

            uint32_t Get_Command_Buffer_Index(Identifier identifier);
            uint32_t Get_Next_Index(std::string label);

        public:
            Command_Buffer_Manager(std::shared_ptr<Descriptor_Set_Manager> descriptor_set_manager,
                                   std::shared_ptr<Logical_Device_Wrapper> logical_device_wrapper_ptr,
                                   std::shared_ptr<Pipeline_Manager> pipeline_manager_ptr,
                                   std::shared_ptr<Storage_Manager> storage_manager_ptr);
            ~Command_Buffer_Manager();

        public:
            void Add_Command_Buffer(std::string label, uint32_t queue_family, std::vector<std::string> resource_group_labels, std::string pipeline_label);
            void Remove_Command_Buffer(Identifier identifier);
            void Reset_Command_Buffer(Identifier identifier);

            void Begin_Recording(Identifier identifier, VkCommandBufferUsageFlagBits usage_flags);
            void End_Recording(Identifier identifier);
            void Image_Memory_Barrier(Identifier identifier, Resource_ID resource_id, VkAccessFlags access_flags, VkImageLayout image_layout, VkPipelineStageFlags pipeline_stage_flags);
            void Dispatch_Compute_Shader(Identifier identifier, uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z);
            void Copy_Image(Identifier identifier, Resource_ID source_resource_id, Resource_ID destination_resource_id, uint32_t width, uint32_t height);
            void Copy_Buffer(Identifier identifier, Resource_ID source, Resource_ID destination, VkDeviceSize src_offset, VkDeviceSize dst_offset, VkDeviceSize copy_size);

            VkCommandBuffer* Get_Command_Buffer(Identifier identifier);
        };
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics