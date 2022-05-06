#include "pipeline_manager.hpp"

#include "../debug_tools.hpp"

namespace CascadeGraphics
{
    namespace Vulkan
    {
        Pipeline_Manager::Pipeline_Manager(std::shared_ptr<Descriptor_Set_Manager> descriptor_set_manager_ptr,
                                           std::shared_ptr<Device> logical_device_ptr,
                                           std::shared_ptr<Storage_Manager> storage_manager_ptr,
                                           std::shared_ptr<Shader_Manager> shader_manager_ptr)
            : m_descriptor_set_manager_ptr(descriptor_set_manager_ptr), m_logical_Device_ptr(logical_device_ptr), m_storage_manager_ptr(storage_manager_ptr), m_shader_manager_ptr(shader_manager_ptr)
        {
        }

        Pipeline_Manager::~Pipeline_Manager()
        {
            // LOG_INFO << "Vulkan: destroying pipelines";

            // for (unsigned int i = 0; i < m_pipelines.size(); i++)
            // {
            //     vkDestroyPipeline(*(m_logical_Device_ptr->Get_Device()), m_pipelines[i], nullptr);
            // }

            // vkDestroyPipelineLayout(*(m_logical_Device_ptr->Get_Device()), m_pipeline_layout, nullptr);

            // LOG_TRACE << "Vulkan: finished destroying pipelines";
        }

        void Pipeline_Manager::Add_Compute_Pipeline(std::string label, std::string resource_grouping_label, std::string shader_label)
        {
            LOG_INFO << "Vulkan: creating compute pipeline with label " << label;

            for (unsigned int i = 0; i < m_pipelines.size(); i++)
            {
                if (m_pipelines[i].label == label)
                {
                    LOG_ERROR << "Vulkan: cannot use label '" << label << "' because it is already in use";
                    exit(EXIT_FAILURE);
                }
            }

            m_pipelines.resize(m_pipelines.size() + 1);

            m_pipelines.back() = {};
            m_pipelines.back().label = label;

            VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
            pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipeline_layout_create_info.pNext = NULL;
            pipeline_layout_create_info.flags = 0;
            pipeline_layout_create_info.setLayoutCount = 1;
            pipeline_layout_create_info.pSetLayouts = m_descriptor_set_manager_ptr->Get_Descriptor_Set_Layout(resource_grouping_label);
            pipeline_layout_create_info.pushConstantRangeCount = 0;
            pipeline_layout_create_info.pPushConstantRanges = NULL;

            VALIDATE_VKRESULT(vkCreatePipelineLayout(*(m_logical_Device_ptr->Get_Device()), &pipeline_layout_create_info, nullptr, &m_pipelines.back().pipeline_layout),
                              "Vulkan: failed to create pipeline layout");

            VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info = {};
            pipeline_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            pipeline_shader_stage_create_info.pNext = NULL;
            pipeline_shader_stage_create_info.flags = 0;
            pipeline_shader_stage_create_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
            pipeline_shader_stage_create_info.module = *m_shader_manager_ptr->Get_Shader_Module(shader_label);
            pipeline_shader_stage_create_info.pName = "main";
            pipeline_shader_stage_create_info.pSpecializationInfo = NULL;

            VkComputePipelineCreateInfo compute_pipeline_create_info = {};
            compute_pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
            compute_pipeline_create_info.pNext = NULL;
            compute_pipeline_create_info.flags = 0;
            compute_pipeline_create_info.stage = pipeline_shader_stage_create_info;
            compute_pipeline_create_info.layout = m_pipelines.back().pipeline_layout;
            compute_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
            compute_pipeline_create_info.basePipelineIndex = 0;

            VALIDATE_VKRESULT(vkCreateComputePipelines(*(m_logical_Device_ptr->Get_Device()), VK_NULL_HANDLE, 1, &compute_pipeline_create_info, NULL, &m_pipelines.back().pipeline),
                              "Vulkan: failed to create compute pipeline");

            LOG_TRACE << "Vulkan: finished creating compute pipeline";
        }
    } // namespace Vulkan
} // namespace CascadeGraphics