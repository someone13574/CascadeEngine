#pragma once

#include "../vulkan_header.hpp"

#include "descriptor_set_manager.hpp"
#include "device_wrapper.hpp"
#include "shader_manager.hpp"
#include "storage_manager.hpp"

#include <memory>
#include <string>
#include <vector>

namespace CascadeGraphics
{
    namespace Vulkan
    {
        class Pipeline_Manager
        {
        public:
            struct Pipeline
            {
                std::string label;
                VkPipeline pipeline;
                VkPipelineLayout pipeline_layout;
            };

        private:
            std::shared_ptr<Descriptor_Set_Manager> m_descriptor_set_manager_ptr;
            std::shared_ptr<Device> m_logical_Device_ptr;
            std::shared_ptr<Storage_Manager> m_storage_manager_ptr;
            std::shared_ptr<Shader_Manager> m_shader_manager_ptr;

            std::vector<Pipeline> m_pipelines;

        public:
            Pipeline_Manager(std::shared_ptr<Descriptor_Set_Manager> descriptor_set_manager_ptr,
                             std::shared_ptr<Device> logical_device_ptr,
                             std::shared_ptr<Storage_Manager> storage_manager_ptr,
                             std::shared_ptr<Shader_Manager> shader_manager_ptr);
            ~Pipeline_Manager();

        public:
            void Add_Compute_Pipeline(std::string label, std::string resource_grouping_label, std::string shader_label);
        };
    } // namespace Vulkan
} // namespace CascadeGraphics