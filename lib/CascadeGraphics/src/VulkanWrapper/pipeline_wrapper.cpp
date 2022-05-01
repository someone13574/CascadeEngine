#include "pipeline_wrapper.hpp"

#include "../debug_tools.hpp"

namespace CascadeGraphics
{
    namespace Vulkan
    {
        Pipeline::Pipeline(std::shared_ptr<Device> logical_device_ptr, std::shared_ptr<Storage_Manager> storage_manager_ptr, std::shared_ptr<Shader_Manager> shader_manager_ptr)
            : m_logical_Device_ptr(logical_device_ptr), m_storage_manager_ptr(storage_manager_ptr), m_shader_manager_ptr(shader_manager_ptr)
        {
            Create_Pipeline_Layout();
            Create_Pipeline();
        }

        Pipeline::~Pipeline()
        {
            LOG_INFO << "Vulkan: destroying pipelines";

            for (unsigned int i = 0; i < m_pipelines.size(); i++)
            {
                vkDestroyPipeline(*(m_logical_Device_ptr->Get_Device()), m_pipelines[i], nullptr);
            }

            vkDestroyPipelineLayout(*(m_logical_Device_ptr->Get_Device()), m_pipeline_layout, nullptr);

            LOG_TRACE << "Vulkan: finished destroying pipelines";
        }

        void Pipeline::Create_Pipeline_Layout()
        {
            LOG_INFO << "Vulkan: creating pipeline layout";

            VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
            pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipeline_layout_create_info.pNext = NULL;
            pipeline_layout_create_info.flags = 0;
            pipeline_layout_create_info.setLayoutCount = 1;
            pipeline_layout_create_info.pSetLayouts = m_storage_manager_ptr->Get_Descriptor_Set_Layout();
            pipeline_layout_create_info.pushConstantRangeCount = 0;
            pipeline_layout_create_info.pPushConstantRanges = nullptr;

            VALIDATE_VKRESULT(vkCreatePipelineLayout(*(m_logical_Device_ptr->Get_Device()), &pipeline_layout_create_info, nullptr, &m_pipeline_layout), "Vulkan: failed to create pipeline layout");

            LOG_TRACE << "Vulkan: finished creating pipeline layout";
        }

        void Pipeline::Create_Pipeline()
        {
            LOG_INFO << "Vulkan: creating pipelines";

            std::vector<Shader_Info> shaders = m_shader_manager_ptr->Get_Shaders_Details();

            for (unsigned int i = 0; i < shaders.size(); i++)
            {
                if (shaders[i].shader_type == Shader_Type::COMPUTE)
                {
                    LOG_TRACE << "Vulkan: create compute pipeline for shader shader " << shaders[i].label;

                    VkPipelineShaderStageCreateInfo compute_pipeline_shader_stage = {};
                    compute_pipeline_shader_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                    compute_pipeline_shader_stage.pNext = NULL;
                    compute_pipeline_shader_stage.flags = 0;
                    compute_pipeline_shader_stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
                    compute_pipeline_shader_stage.module = *m_shader_manager_ptr->Get_Shader_Module(shaders[i].label);
                    compute_pipeline_shader_stage.pName = "main";
                    compute_pipeline_shader_stage.pSpecializationInfo = nullptr;

                    VkComputePipelineCreateInfo compute_pipeline_create_info = {};
                    compute_pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
                    compute_pipeline_create_info.pNext = NULL;
                    compute_pipeline_create_info.flags = 0;
                    compute_pipeline_create_info.stage = compute_pipeline_shader_stage;
                    compute_pipeline_create_info.layout = m_pipeline_layout;
                    compute_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
                    compute_pipeline_create_info.basePipelineIndex = 0;

                    m_pipelines.resize(m_pipelines.size() + 1);

                    // This is inefficient, better to call once
                    VALIDATE_VKRESULT(vkCreateComputePipelines(*(m_logical_Device_ptr->Get_Device()), VK_NULL_HANDLE, 1, &compute_pipeline_create_info, nullptr, &m_pipelines.back()),
                                      "Vulkan: failed to create compute pipeline");
                }
            }

            LOG_TRACE << "Vulkan: finished creating pipelines";
        }
    } // namespace Vulkan
} // namespace CascadeGraphics