#pragma once

#include "../vulkan_header.hpp"

#include "device_wrapper.hpp"
#include "shader_manager.hpp"
#include "storage_manager.hpp"

#include <memory>

namespace CascadeGraphics
{
    namespace Vulkan
    {
        class Pipeline
        {
        private:
            std::shared_ptr<Device> m_logical_Device_ptr;
            std::shared_ptr<Storage_Manager> m_storage_manager_ptr;
            std::shared_ptr<Shader_Manager> m_shader_manager_ptr;

            VkPipeline m_pipeline;
            VkPipelineLayout m_pipeline_layout;

        private:
            void Create_Pipeline_Layout();
            void Create_Pipeline();

        public:
            Pipeline(std::shared_ptr<Device> logical_device_ptr, std::shared_ptr<Storage_Manager> storage_manager_ptr, std::shared_ptr<Shader_Manager> shader_manager_ptr);
            ~Pipeline();
        };
    } // namespace Vulkan
} // namespace CascadeGraphics