#include "descriptor.hpp"

#include <acorn_logging.hpp>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Descriptor::Descriptor(Buffer* buffer_ptr, VkDescriptorType descriptor_type) :
            m_descriptor_type(descriptor_type)
        {
            LOG_TRACE << "Graphics (Vulkan): Creating buffer descriptor";

            VkDescriptorBufferInfo descriptor_buffer_info = {};
            descriptor_buffer_info.buffer = buffer_ptr->Get_Buffer();
            descriptor_buffer_info.offset = 0;
            descriptor_buffer_info.range = VK_WHOLE_SIZE;

            m_descriptor_buffer_info = std::move(descriptor_buffer_info);
        }

        Descriptor::Descriptor(Image* image_ptr, VkDescriptorType descriptor_type) :
            m_descriptor_type(descriptor_type)
        {
            LOG_TRACE << "Graphics (Vulkan): Creating image descriptor";

            VkDescriptorImageInfo descriptor_image_info = {};
            descriptor_image_info.sampler = image_ptr->Get_Sampler();
            descriptor_image_info.imageView = image_ptr->Get_Image_View();
            descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

            m_descriptor_image_info = std::move(descriptor_image_info);
        }

        VkDescriptorType Descriptor::Get_Descriptor_Type()
        {
            return m_descriptor_type;
        }

        VkDescriptorImageInfo* Descriptor::Get_Image_Descriptor_Info()
        {
            if (m_descriptor_image_info.has_value())
                return m_descriptor_image_info.operator->();
            else
                return nullptr;
        }

        VkDescriptorBufferInfo* Descriptor::Get_Buffer_Descriptor_Info()
        {
            if (m_descriptor_buffer_info.has_value())
                return m_descriptor_buffer_info.operator->();
            else
                return nullptr;
        }
    }    // namespace Vulkan
}    // namespace Cascade_Graphics