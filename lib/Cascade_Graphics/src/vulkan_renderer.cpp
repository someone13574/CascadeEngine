#include "vulkan_renderer.hpp"

#include "Vulkan_Backend/buffer.hpp"
#include "Vulkan_Backend/descriptor.hpp"
#include "Vulkan_Backend/queue_data.hpp"
#include "Vulkan_Backend/swapchain_builder.hpp"
#include "Vulkan_Backend/win32_surface.hpp"
#include "Vulkan_Backend/xcb_surface.hpp"
#include "vulkan_graphics.hpp"
#include <acorn_logging.hpp>
#include <cmath>

namespace Cascade_Graphics
{
    Vulkan_Renderer::Vulkan_Renderer(Graphics* graphics_ptr, Platform platform, Window_Info* window_info_ptr)
    {
        LOG_INFO << "Graphics: Initializing renderer with Vulkan backend";

        Vulkan_Graphics* vulkan_graphics_ptr = static_cast<Vulkan_Graphics*>(graphics_ptr);

        if (platform == Platform::LINUX_XCB)
        {
            m_surface_ptr = new Vulkan::XCB_Surface(window_info_ptr, vulkan_graphics_ptr->m_instance_ptr);
        }
        else if (platform == Platform::WINDOWS_WIN32)
        {
            m_surface_ptr = new Vulkan::WIN32_Surface(window_info_ptr, vulkan_graphics_ptr->m_instance_ptr);
        }
        else
        {
            LOG_FATAL << "Graphics (Vulkan): Unknown platform";
            exit(EXIT_FAILURE);
        }

        m_swapchain_ptr = Vulkan::Swapchain_Builder(vulkan_graphics_ptr->m_physical_device_ptr, m_surface_ptr)
                              .Select_Image_Format(std::vector<VkSurfaceFormatKHR> {{VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}})
                              .Select_Image_Extent(window_info_ptr)
                              .Set_Swapchain_Image_Usage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT)
                              .Select_Present_Mode(std::vector<VkPresentModeKHR> {VK_PRESENT_MODE_FIFO_RELAXED_KHR, VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR})
                              .Set_Allowed_Queue_Requirements(std::vector<Vulkan::Device_Queue_Requirement*> {&vulkan_graphics_ptr->m_physical_device_ptr->Get_Device_Queues().device_queue_requirements[0]})
                              .Build(vulkan_graphics_ptr->m_device_ptr);

        std::vector<Vulkan::Image*>
            swapchain_images;
        for (uint32_t i = 0; i < m_swapchain_ptr->Get_Image_Count(); i++)
        {
            m_render_target_ptrs.push_back(new Vulkan::Image(vulkan_graphics_ptr->m_device_ptr, m_swapchain_ptr->Get_Surface_Format().format, m_swapchain_ptr->Get_Image_Extent(), VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, std::vector<Vulkan::Device_Queue_Requirement*> {&vulkan_graphics_ptr->m_physical_device_ptr->Get_Device_Queues().device_queue_requirements[0]}, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0));
            swapchain_images.push_back(m_swapchain_ptr->Get_Image_Object(i));

            std::vector<Vulkan::Descriptor> rendering_descriptors = {Vulkan::Descriptor(m_render_target_ptrs[i], VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)};
            m_rendering_descriptor_set_ptrs.push_back(new Vulkan::Descriptor_Set(vulkan_graphics_ptr->m_device_ptr, rendering_descriptors));
        }

        m_rendering_pipeline_ptr = new Vulkan::Compute_Pipeline(vulkan_graphics_ptr->m_device_ptr, "../lib/Cascade_Graphics/src/Vulkan_Backend/Shaders/render.comp", m_rendering_descriptor_set_ptrs);

        m_rendering_command_buffers_ptr = new Vulkan::Command_Buffer(vulkan_graphics_ptr->m_device_ptr, m_rendering_pipeline_ptr, m_swapchain_ptr->Get_Image_Count(), vulkan_graphics_ptr->m_physical_device_ptr->Get_Device_Queues().device_queue_requirements[0].device_queues[0].queue_family_index);
        for (uint32_t i = 0; i < m_swapchain_ptr->Get_Image_Count(); i++)
        {
            m_rendering_command_buffers_ptr->Bind_Descriptor_Set(m_rendering_descriptor_set_ptrs[i], i);

            m_rendering_command_buffers_ptr->Add_Image(m_render_target_ptrs[i]);
            m_rendering_command_buffers_ptr->Add_Image(swapchain_images[i]);

            m_rendering_command_buffers_ptr->Image_Memory_Barrier(swapchain_images[i], VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT)->Image_Memory_Barrier(m_render_target_ptrs[i], VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        }
        m_rendering_command_buffers_ptr->Dispatch_Compute_Shader(std::ceil(m_swapchain_ptr->Get_Image_Extent().width / 32.0), std::ceil(m_swapchain_ptr->Get_Image_Extent().height / 32.0), 1);
        for (uint32_t i = 0; i < m_swapchain_ptr->Get_Image_Count(); i++)
        {
            m_rendering_command_buffers_ptr->Image_Memory_Barrier(m_render_target_ptrs[i], VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT)
                ->Copy_Image(m_render_target_ptrs[i], swapchain_images[i], m_swapchain_ptr->Get_Image_Extent().width, m_swapchain_ptr->Get_Image_Extent().height)
                ->Image_Memory_Barrier(m_render_target_ptrs[i], VK_ACCESS_NONE, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_TRANSFER_BIT)
                ->Image_Memory_Barrier(swapchain_images[i], VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_TRANSFER_BIT);
        }
        m_rendering_command_buffers_ptr->Finish_Recording();

        LOG_INFO
            << "Graphics: Finished initializing renderer with Vulkan backend";
    }

    Vulkan_Renderer::~Vulkan_Renderer()
    {
        LOG_INFO << "Graphics: Destroying Vulkan renderer objects";

        delete m_rendering_command_buffers_ptr;
        delete m_rendering_pipeline_ptr;

        for (uint32_t i = 0; i < m_render_target_ptrs.size(); i++)
        {
            delete m_rendering_descriptor_set_ptrs[i];
            delete m_render_target_ptrs[i];
        }
        m_rendering_descriptor_set_ptrs.clear();
        m_render_target_ptrs.clear();

        delete m_swapchain_ptr;
        delete m_surface_ptr;

        LOG_INFO << "Graphics: Finished destroying Vulkan renderer objects";
    }
}    // namespace Cascade_Graphics