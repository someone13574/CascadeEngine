#include "pipeline_manager.hpp"

#include "debug_tools.hpp"


namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        Pipeline_Manager::Pipeline_Manager(std::shared_ptr<Descriptor_Set_Manager> descriptor_set_manager_ptr,
                                           std::shared_ptr<Logical_Device_Wrapper> logical_device_wrapper_ptr,
                                           std::shared_ptr<Shader_Manager> shader_manager_ptr,
                                           std::shared_ptr<Storage_Manager> storage_manager_ptr)
            : m_descriptor_set_manager_ptr(descriptor_set_manager_ptr), m_logical_device_wrapper_ptr(logical_device_wrapper_ptr), m_shader_manager_ptr(shader_manager_ptr), m_storage_manager_ptr(storage_manager_ptr)
        {
        }

        Pipeline_Manager::~Pipeline_Manager()
        {
            LOG_INFO << "Vulkan Backend: Destroying pipelines";

            for (uint32_t i = 0; i < m_pipelines.size(); i++)
            {
                vkDestroyPipeline(*m_logical_device_wrapper_ptr->Get_Device(), m_pipelines[i].pipeline, nullptr);
                vkDestroyPipelineLayout(*m_logical_device_wrapper_ptr->Get_Device(), m_pipelines[i].pipeline_layout, nullptr);
            }

            LOG_TRACE << "Vulkan Backend: Finished destroying pipelines";
        }

        Identifier Pipeline_Manager::Add_Compute_Pipeline(std::string label, Identifier descriptor_set_identifier, Identifier shader_identifier)
        {
            Identifier identifier = {};
            identifier.label = label;
            identifier.index = 0;

            while (true)
            {
                bool index_in_use = false;

                for (uint32_t i = 0; i < m_pipelines.size(); i++)
                {
                    index_in_use |= m_pipelines[i].identifier == identifier;
                }

                if (!index_in_use)
                {
                    break;
                }
                else
                {
                    identifier.index++;
                }
            }

            LOG_INFO << "Vulkan Backend: Creating compute pipeline with identifier " << identifier.Get_Identifier_String();

            m_pipelines.resize(m_pipelines.size() + 1);

            m_pipelines.back() = {};
            m_pipelines.back().identifier = identifier;
            m_pipelines.back().type = Pipeline_Type::COMPUTE;

            VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
            pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipeline_layout_create_info.pNext = nullptr;
            pipeline_layout_create_info.flags = 0;
            pipeline_layout_create_info.setLayoutCount = 1;
            pipeline_layout_create_info.pSetLayouts = &m_descriptor_set_manager_ptr->Get_Descriptor_Set_Data(descriptor_set_identifier)->descriptor_set_layout;
            pipeline_layout_create_info.pushConstantRangeCount = 0;
            pipeline_layout_create_info.pPushConstantRanges = nullptr;

            VALIDATE_VKRESULT(vkCreatePipelineLayout(*m_logical_device_wrapper_ptr->Get_Device(), &pipeline_layout_create_info, nullptr, &m_pipelines.back().pipeline_layout), "Vulkan Backend: Failed to create pipeline layout");

            VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info = {};
            pipeline_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            pipeline_shader_stage_create_info.pNext = nullptr;
            pipeline_shader_stage_create_info.flags = 0;
            pipeline_shader_stage_create_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
            pipeline_shader_stage_create_info.module = m_shader_manager_ptr->Get_Shader_Data(shader_identifier)->shader_module;
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

            VALIDATE_VKRESULT(vkCreateComputePipelines(*m_logical_device_wrapper_ptr->Get_Device(), VK_NULL_HANDLE, 1, &compute_pipeline_create_info, nullptr, &m_pipelines.back().pipeline), "Vulkan Backend: Failed to create compute pipeline");

            LOG_TRACE << "Vulkan Backend: Finished creating compute pipeline";
            return identifier;
        }

        void Pipeline_Manager::Delete_Pipeline(Identifier identifier)
        {
            LOG_INFO << "Vulkan Backend: Destroying pipeline " << identifier.Get_Identifier_String();

            for (uint32_t i = 0; i < m_pipelines.size(); i++)
            {
                if (m_pipelines[i].identifier == identifier)
                {
                    vkDestroyPipeline(*m_logical_device_wrapper_ptr->Get_Device(), m_pipelines[i].pipeline, nullptr);
                    vkDestroyPipelineLayout(*m_logical_device_wrapper_ptr->Get_Device(), m_pipelines[i].pipeline_layout, nullptr);

                    m_pipelines.erase(m_pipelines.begin() + i);
                    return;
                }
            }

            LOG_ERROR << "Vulkan Backend: Pipeline with identifier " << identifier.Get_Identifier_String() << " does not exist";
            exit(EXIT_FAILURE);
        }

        Pipeline_Manager::Pipeline_Data* Pipeline_Manager::Get_Pipeline_Data(Identifier identifier)
        {
            for (uint32_t i = 0; i < m_pipelines.size(); i++)
            {
                if (m_pipelines[i].identifier == identifier)
                {
                    return &m_pipelines[i];
                }
            }

            LOG_ERROR << "Vulkan Backend: Pipeline with identifier " << identifier.Get_Identifier_String() << " does not exist";
            exit(EXIT_FAILURE);
        }
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics