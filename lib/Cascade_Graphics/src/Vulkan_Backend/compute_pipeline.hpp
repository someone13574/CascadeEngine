#pragma once

#ifdef __linux__
    #define VK_USE_PLATFORM_XCB_KHR
#elif defined _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "device.hpp"
#include <string>
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

        private:
            VkShaderModule Compile_Shader(std::string shader_path);

        public:
            Compute_Pipeline(Device* device_ptr, std::string shader_path);
            ~Compute_Pipeline();
        };
    }    // namespace Vulkan
}    // namespace Cascade_Graphics