#pragma once

#include "vulkan_header.hpp"

#include "descriptor_set_manager.hpp"
#include "device_wrapper.hpp"
#include "shader_manager.hpp"
#include "storage_manager.hpp"

#include <memory>
#include <string>
#include <vector>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Storage_Manager;
        class Descriptor_Set_Manager;

        class Pipeline_Manager
        {
        public:
            enum Pipeline_Type
            {
                COMPUTE
            };

            struct Pipeline
            {
                std::string label;
                Pipeline_Type type;

                VkPipeline pipeline;
                VkPipelineLayout pipeline_layout;
            };

        private:
            std::shared_ptr<Descriptor_Set_Manager> m_descriptor_set_manager_ptr;
            std::shared_ptr<Device> m_logical_device_ptr;
            std::shared_ptr<Storage_Manager> m_storage_manager_ptr;
            std::shared_ptr<Shader_Manager> m_shader_manager_ptr;

            std::vector<Pipeline> m_pipelines;

        private:
            unsigned int Get_Pipeline_Index(std::string label);

        public:
            Pipeline_Manager(std::shared_ptr<Descriptor_Set_Manager> descriptor_set_manager_ptr, std::shared_ptr<Device> logical_device_ptr, std::shared_ptr<Storage_Manager> storage_manager_ptr, std::shared_ptr<Shader_Manager> shader_manager_ptr);
            ~Pipeline_Manager();

        public:
            void Add_Compute_Pipeline(std::string label, std::string resource_grouping_label, std::string shader_label);

            Pipeline_Type Get_Pipeline_Type(std::string label);
            VkPipeline* Get_Pipeline(std::string label);
            VkPipelineLayout* Get_Pipeline_Layout(std::string label);
        };
    } // namespace Vulkan
} // namespace Cascade_Graphics