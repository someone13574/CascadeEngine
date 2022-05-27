#include "pipeline_manager.hpp"

#include "debug_tools.hpp"

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Pipeline_Manager::Pipeline_Manager(std::shared_ptr<Resource_Grouping_Manager> resource_grouping_manager_ptr,
                                           std::shared_ptr<Device> logical_device_ptr,
                                           std::shared_ptr<Storage_Manager> storage_manager_ptr,
                                           std::shared_ptr<Shader_Manager> shader_manager_ptr)
            : m_resource_grouping_manager_ptr(resource_grouping_manager_ptr), m_logical_device_ptr(logical_device_ptr), m_storage_manager_ptr(storage_manager_ptr), m_shader_manager_ptr(shader_manager_ptr)
        {
        }

        Pipeline_Manager::~Pipeline_Manager()
        {
            LOG_INFO << "Vulkan: Destroying pipelines";

            for (unsigned int i = 0; i < m_pipelines.size(); i++)
            {
                vkDestroyPipeline(*(m_logical_device_ptr->Get_Device()), m_pipelines[i].pipeline, nullptr);
                vkDestroyPipelineLayout(*(m_logical_device_ptr->Get_Device()), m_pipelines[i].pipeline_layout, nullptr);
            }

            LOG_TRACE << "Vulkan: Finished destroying pipelines";
        }

        unsigned int Pipeline_Manager::Get_Pipeline_Index(std::string label)
        {
            for (unsigned int i = 0; i < m_pipelines.size(); i++)
            {
                if (m_pipelines[i].label == label)
                {
                    return i;
                }
            }

            LOG_ERROR << "Vulkan: Pipeline with label '" << label << "' does not exist";
            exit(EXIT_FAILURE);
        }

        void Pipeline_Manager::Add_Compute_Pipeline(std::string label, std::string resource_grouping_label, std::string shader_label)
        {
            LOG_INFO << "Vulkan: Creating compute pipeline with label " << label;

            for (unsigned int i = 0; i < m_pipelines.size(); i++)
            {
                if (m_pipelines[i].label == label)
                {
                    LOG_ERROR << "Vulkan: Cannot use label '" << label << "' because it is already in use";
                    exit(EXIT_FAILURE);
                }
            }

            m_pipelines.resize(m_pipelines.size() + 1);

            m_pipelines.back() = {};
            m_pipelines.back().label = label;
            m_pipelines.back().type = Pipeline_Type::COMPUTE;

            VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
            pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipeline_layout_create_info.pNext = nullptr;
            pipeline_layout_create_info.flags = 0;
            pipeline_layout_create_info.setLayoutCount = 1;
            pipeline_layout_create_info.pSetLayouts = m_resource_grouping_manager_ptr->Get_Descriptor_Set_Layout(resource_grouping_label);
            pipeline_layout_create_info.pushConstantRangeCount = 0;
            pipeline_layout_create_info.pPushConstantRanges = nullptr;

            VALIDATE_VKRESULT(vkCreatePipelineLayout(*(m_logical_device_ptr->Get_Device()), &pipeline_layout_create_info, nullptr, &m_pipelines.back().pipeline_layout), "Vulkan: Failed to create pipeline layout");

            VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info = {};
            pipeline_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            pipeline_shader_stage_create_info.pNext = nullptr;
            pipeline_shader_stage_create_info.flags = 0;
            pipeline_shader_stage_create_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
            pipeline_shader_stage_create_info.module = *m_shader_manager_ptr->Get_Shader_Module(shader_label);
            pipeline_shader_stage_create_info.pName = "main";
            pipeline_shader_stage_create_info.pSpecializationInfo = nullptr;

            VkComputePipelineCreateInfo compute_pipeline_create_info = {};
            compute_pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
            compute_pipeline_create_info.pNext = nullptr;
            compute_pipeline_create_info.flags = 0;
            compute_pipeline_create_info.stage = pipeline_shader_stage_create_info;
            compute_pipeline_create_info.layout = m_pipelines.back().pipeline_layout;
            compute_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
            compute_pipeline_create_info.basePipelineIndex = 0;

            VALIDATE_VKRESULT(vkCreateComputePipelines(*(m_logical_device_ptr->Get_Device()), VK_NULL_HANDLE, 1, &compute_pipeline_create_info, nullptr, &m_pipelines.back().pipeline), "Vulkan: Failed to create compute pipeline");

            LOG_TRACE << "Vulkan: Finished creating compute pipeline";
        }

        Pipeline_Manager::Pipeline_Type Pipeline_Manager::Get_Pipeline_Type(std::string label)
        {
            return m_pipelines[Get_Pipeline_Index(label)].type;
        }

        VkPipeline* Pipeline_Manager::Get_Pipeline(std::string label)
        {
            return &m_pipelines[Get_Pipeline_Index(label)].pipeline;
        }

        VkPipelineLayout* Pipeline_Manager::Get_Pipeline_Layout(std::string label)
        {
            return &m_pipelines[Get_Pipeline_Index(label)].pipeline_layout;
        }
    } // namespace Vulkan
} // namespace Cascade_Graphics