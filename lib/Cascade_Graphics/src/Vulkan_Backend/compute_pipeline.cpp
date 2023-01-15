#include "compute_pipeline.hpp"

#include <acorn_logging.hpp>
#include <fstream>
#include <shaderc/shaderc.hpp>
#include <vector>
#include <vulkan/vk_enum_string_helper.h>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Compute_Pipeline::Compute_Pipeline(Device* device_ptr, std::string shader_path) :
            m_device_ptr(device_ptr)
        {
            LOG_DEBUG << "Graphics (Vulkan): Creating compute pipeline";
            LOG_TRACE << "Graphics (Vulkan): Creating pipeline layout";

            VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
            pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipeline_layout_create_info.pNext = NULL;
            pipeline_layout_create_info.flags = 0;
            pipeline_layout_create_info.setLayoutCount = 0;
            pipeline_layout_create_info.pSetLayouts = NULL;
            pipeline_layout_create_info.pushConstantRangeCount = 0;
            pipeline_layout_create_info.pPushConstantRanges = NULL;

            VkPipelineLayout pipeline_layout;

            VkResult create_pipeline_layout_result = vkCreatePipelineLayout(m_device_ptr->Get(), &pipeline_layout_create_info, NULL, &pipeline_layout);
            if (create_pipeline_layout_result != VK_SUCCESS)
            {
                LOG_FATAL << "Graphics (Vulkan): Failed to create pipeline layout with VkResult " << string_VkResult(create_pipeline_layout_result);
                exit(EXIT_FAILURE);
            }

            LOG_TRACE << "Graphics (Vulkan): Creating shader stage";

            VkShaderModule shader_module = Compile_Shader(shader_path);

            VkPipelineShaderStageCreateInfo shader_stage_create_info = {};
            shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shader_stage_create_info.pNext = NULL;
            shader_stage_create_info.flags = 0;
            shader_stage_create_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
            shader_stage_create_info.module = shader_module;
            shader_stage_create_info.pName = "main";
            shader_stage_create_info.pSpecializationInfo = NULL;

            LOG_TRACE << "Graphics (Vulkan): Creating pipeline";

            VkComputePipelineCreateInfo compute_pipeline_create_info = {};
            compute_pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
            compute_pipeline_create_info.pNext = NULL;
            compute_pipeline_create_info.flags = 0;
            compute_pipeline_create_info.stage = shader_stage_create_info;
            compute_pipeline_create_info.layout = pipeline_layout;
            compute_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
            compute_pipeline_create_info.basePipelineIndex = 0;

            VkResult create_compute_pipeline_result = vkCreateComputePipelines(m_device_ptr->Get(), VK_NULL_HANDLE, 1, &compute_pipeline_create_info, NULL, &m_pipeline);
            if (create_compute_pipeline_result != VK_SUCCESS)
            {
                LOG_FATAL << "Graphics (Vulkan): Failed to create compute pipeline with VkResult " << string_VkResult(create_compute_pipeline_result);
                exit(EXIT_FAILURE);
            }

            LOG_TRACE << "Graphics (Vulkan): Cleaning up VkPipelineLayout and VkShaderModule";

            vkDestroyPipelineLayout(m_device_ptr->Get(), pipeline_layout, NULL);
            vkDestroyShaderModule(m_device_ptr->Get(), shader_module, NULL);
        }

        Compute_Pipeline::~Compute_Pipeline()
        {
            vkDestroyPipeline(m_device_ptr->Get(), m_pipeline, NULL);
        }

        VkShaderModule Compute_Pipeline::Compile_Shader(std::string shader_path)
        {
            // Load shader source from file
            LOG_TRACE << "Graphics (Vulkan): Loading source of shader at path " << shader_path;

            std::ifstream input_file_stream(shader_path, std::ios::in);

            if (!input_file_stream.is_open())
            {
                LOG_FATAL << "Graphics (Vulkan): Failed to open shader source file at path " << shader_path;
                exit(EXIT_FAILURE);
            }

            std::string current_line;
            std::string shader_source;

            while (!input_file_stream.eof())
            {
                getline(input_file_stream, current_line);
                shader_source += current_line + '\n';
            }

            // Compile shader
            shaderc::Compiler compiler;
            shaderc::CompileOptions compile_options;

#ifndef DEBUG
            compile_options.SetOptimizationLevel(shaderc_optimization_level_zero);
            compile_options.SetGenerateDebugInfo();
#else
            compile_options.SetOptimizationLevel(shaderc_optimization_level_performance);
#endif
            compile_options.SetWarningsAsErrors();
            compile_options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_1);
            compile_options.SetSourceLanguage(shaderc_source_language_glsl);

            shaderc::SpvCompilationResult compilation_result;
            compilation_result = compiler.CompileGlslToSpv(shader_source, shaderc_shader_kind::shaderc_compute_shader, shader_path.c_str(), compile_options);

            if (compilation_result.GetCompilationStatus() != shaderc_compilation_status_success)
            {
                LOG_FATAL << "Graphics (Vulkan): Failed to compile shader with error " << compilation_result.GetErrorMessage();
                exit(EXIT_FAILURE);
            }

            LOG_TRACE << "Graphics (Vulkan): Successfully compiled with " << compilation_result.GetNumWarnings() << " warnings";
            LOG_TRACE << "Graphics (Vulkan): Creating shader module";

            std::vector<uint32_t> compiled_shader = {compilation_result.cbegin(), compilation_result.cend()};

            VkShaderModuleCreateInfo shader_module_create_info = {};
            shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            shader_module_create_info.pNext = NULL;
            shader_module_create_info.flags = 0;
            shader_module_create_info.codeSize = compiled_shader.size() * sizeof(uint32_t);
            shader_module_create_info.pCode = compiled_shader.data();

            VkShaderModule shader_module;

            VkResult create_shader_module_result = vkCreateShaderModule(m_device_ptr->Get(), &shader_module_create_info, NULL, &shader_module);
            if (create_shader_module_result != VK_SUCCESS)
            {
                LOG_FATAL << "Graphics (Vulkan): Failed to create shader module with VkResult " << string_VkResult(create_shader_module_result);
                exit(EXIT_FAILURE);
            }

            return shader_module;
        }
    }    // namespace Vulkan
}    // namespace Cascade_Graphics