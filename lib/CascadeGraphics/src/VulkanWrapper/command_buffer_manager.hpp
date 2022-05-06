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
        struct Image_Resource_State
        {
            std::string label;
            unsigned int id;

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
            VkCommandBuffer command_buffer;
            std::vector<Image_Resource_State> image_resource_states;
        };

        class Command_Buffer_Manager
        {
        private:
            std::shared_ptr<Device> m_logical_device_ptr;
            std::shared_ptr<Storage_Manager> m_storage_manager_ptr;

            std::vector<Command_Pool> m_command_pools;
            std::vector<Command_Buffer> m_command_buffers;

        private:
            void Create_Command_Pool(unsigned int queue_family);
            void Allocate_Command_Buffer(unsigned int command_buffer_index, unsigned int command_pool_index);

        public:
            Command_Buffer_Manager();

        public:
            void Add_Command_Buffer(unsigned int queue_family);
        };
    } // namespace Vulkan
} // namespace CascadeGraphics