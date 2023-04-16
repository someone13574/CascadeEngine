#pragma once

#ifdef __linux__
    #define VK_USE_PLATFORM_XCB_KHR
#elif defined _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "descriptor_set.hpp"
#include "device.hpp"
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Compute_Pipeline
        {
        private:
            Device* m_device_ptr;

            VkPipeline m_pipeline;
            VkPipelineLayout m_pipeline_layout;

        private:
            VkShaderModule Compile_Shader(std::string shader_path);

        public:
            Compute_Pipeline(Device* device_ptr, std::string shader_path, std::vector<Descriptor_Set*> descriptor_sets);
            ~Compute_Pipeline();

            VkPipeline Get();
            VkPipelineLayout Get_Layout();
        };
    }    // namespace Vulkan
}    // namespace Cascade_Graphics