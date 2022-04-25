#include "pipeline_wrapper.hpp"

#include "cascade_logging.hpp"

namespace CascadeGraphics
{
    namespace Vulkan
    {
        Pipeline::Pipeline(std::shared_ptr<Device> logical_device_ptr) : m_logical_Device_ptr(logical_device_ptr)
        {
            LOG_INFO << "Vulkan: creating pipeline";
        }

        void Pipeline::Create_Pipeline_Layout()
        {
            LOG_TRACE << "Vulkan: creating pipeline layout";

            VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
            pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipeline_layout_create_info.pNext = NULL;
            pipeline_layout_create_info.flags = 0;
            pipeline_layout_create_info.setLayoutCount = 1;
        }
    } // namespace Vulkan
} // namespace CascadeGraphics