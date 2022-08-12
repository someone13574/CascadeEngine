#pragma once

#include "descriptor_set_manager.hpp"
#include "logical_device_wrapper.hpp"
#include "shader_manager.hpp"
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
        class Descriptor_Set_Manager;

        class Pipeline_Manager
        {
        public:
            enum Pipeline_Type
            {
                COMPUTE
            };

            struct Pipeline_Data
            {
                std::string label;
                Pipeline_Type type;

                VkPipeline pipeline;
                VkPipelineLayout pipeline_layout;
            };

        private:
            std::vector<Pipeline_Data> m_pipelines;

            std::shared_ptr<Descriptor_Set_Manager> m_descriptor_set_manager_ptr;
            std::shared_ptr<Logical_Device_Wrapper> m_logical_device_wrapper_ptr;
            std::shared_ptr<Shader_Manager> m_shader_manager_ptr;
            std::shared_ptr<Storage_Manager> m_storage_manager_ptr;

        public:
            Pipeline_Manager(std::shared_ptr<Descriptor_Set_Manager> descriptor_set_manager_ptr,
                             std::shared_ptr<Logical_Device_Wrapper> logical_device_wrapper_ptr,
                             std::shared_ptr<Shader_Manager> shader_manager_ptr,
                             std::shared_ptr<Storage_Manager> storage_manager_ptr);
            ~Pipeline_Manager();

        public:
            std::string Add_Compute_Pipeline(std::string label, std::string resource_grouping_label, std::string shader_label);
            void Delete_Pipeline(std::string label);

            Pipeline_Data* Get_Pipeline_Data(std::string label);
        };
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics