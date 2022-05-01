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
            LOG_INFO << "Vulkan: destroying pipeline";

            vkDestroyPipelineLayout(*(m_logical_Device_ptr->Get_Device()), m_pipeline_layout, nullptr);

            LOG_TRACE << "Vulkan: finished destroying pipeline";
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
            LOG_INFO << "Vulkan: creating pipeline";

            LOG_TRACE << "Vulkan: finished creating pipeline";
        }
    } // namespace Vulkan
} // namespace CascadeGraphics