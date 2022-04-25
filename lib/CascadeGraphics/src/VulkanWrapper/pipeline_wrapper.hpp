#pragma once

#include "../vulkan_header.hpp"

#include "device_wrapper.hpp"

#include <memory>

namespace CascadeGraphics
{
    namespace Vulkan
    {
        class Pipeline
        {
        private:
            std::shared_ptr<Device> m_logical_Device_ptr;

            VkPipeline m_pipeline;
            VkPipelineLayout m_pipeline_layout;

        private:
            void Create_Pipeline_Layout();

        public:
            Pipeline(std::shared_ptr<Device> logical_device_ptr);
        };
    } // namespace Vulkan
} // namespace CascadeGraphics