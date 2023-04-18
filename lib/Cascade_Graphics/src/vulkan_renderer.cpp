#include "vulkan_renderer.hpp"

#include "Data_Types/vector_3.hpp"
#include "Vulkan_Backend/buffer.hpp"
#include "Vulkan_Backend/descriptor.hpp"
#include "Vulkan_Backend/queue_data.hpp"
#include "Vulkan_Backend/swapchain_builder.hpp"
#include "Vulkan_Backend/win32_surface.hpp"
#include "Vulkan_Backend/xcb_surface.hpp"
#include <acorn_logging.hpp>
#include <cmath>
#include <vulkan/vk_enum_string_helper.h>

namespace Cascade_Graphics
{
    Vulkan_Renderer::Vulkan_Renderer(Graphics* graphics_ptr, Platform platform, Window_Info* window_info_ptr) :
        m_vulkan_graphics_ptr(static_cast<Vulkan_Graphics*>(graphics_ptr)), m_window_info_ptr(window_info_ptr), m_camera(Vector_3<double>(0.0, 0.0, 0.0), Vector_3<double>(1.0, 0.0, 0.0))
    {
        LOG_INFO << "Graphics: Initializing renderer with Vulkan backend";

        m_camera.Set_Position(Vector_3<double>(-5.0, 0.0, 0.0));
        m_camera.Look_At(Vector_3<double>(0.0, 0.0, 0.0));

        // Create a surface
        if (platform == Platform::LINUX_XCB)
        {
            m_surface_ptr = new Vulkan::XCB_Surface(window_info_ptr, m_vulkan_graphics_ptr->m_instance_ptr);
        }
        else if (platform == Platform::WINDOWS_WIN32)
        {
            m_surface_ptr = new Vulkan::WIN32_Surface(window_info_ptr, m_vulkan_graphics_ptr->m_instance_ptr);
        }
        else
        {
            LOG_FATAL << "Graphics (Vulkan): Unknown platform";
            exit(EXIT_FAILURE);
        }

        // Create the swapchain
        m_swapchain_ptr = Vulkan::Swapchain_Builder(m_vulkan_graphics_ptr->m_physical_device_ptr, m_surface_ptr)
                              .Select_Image_Format(std::vector<VkSurfaceFormatKHR> {{VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}})
                              .Select_Image_Extent(window_info_ptr)
                              .Set_Swapchain_Image_Usage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT)
                              .Select_Present_Mode(std::vector<VkPresentModeKHR> {VK_PRESENT_MODE_IMMEDIATE_KHR})
                              .Set_Allowed_Queue_Requirements(std::vector<Vulkan::Device_Queue_Requirement*> {&m_vulkan_graphics_ptr->m_physical_device_ptr->Get_Device_Queues().device_queue_requirements[0]})
                              .Build(m_vulkan_graphics_ptr->m_device_ptr);

        std::vector<Vulkan::Image*>
            swapchain_images;
        for (uint32_t swapchain_image_index = 0; swapchain_image_index < m_swapchain_ptr->Get_Image_Count(); swapchain_image_index++)
        {
            swapchain_images.push_back(m_swapchain_ptr->Get_Image_Object(swapchain_image_index));
        }

        // Create render targets
        for (uint32_t render_target_index = 0; render_target_index < m_swapchain_ptr->Get_Image_Count(); render_target_index++)
        {
            m_render_target_ptrs.push_back(new Vulkan::Image(m_vulkan_graphics_ptr->m_device_ptr, m_swapchain_ptr->Get_Surface_Format().format, m_swapchain_ptr->Get_Image_Extent(), VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, std::vector<Vulkan::Device_Queue_Requirement*> {&m_vulkan_graphics_ptr->m_physical_device_ptr->Get_Device_Queues().device_queue_requirements[0]}, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0));
        }

        // Create camera buffer
        for (uint32_t camera_buffer_index = 0; camera_buffer_index < m_swapchain_ptr->Get_Image_Count(); camera_buffer_index++)
        {
            m_camera_buffer_ptrs.push_back(new Vulkan::Buffer(m_vulkan_graphics_ptr->m_device_ptr, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(Camera::GPU_Camera_Data), std::vector<Vulkan::Device_Queue_Requirement*> {&m_vulkan_graphics_ptr->m_physical_device_ptr->Get_Device_Queues().device_queue_requirements[0]}, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT));
        }

        // Create descriptor sets
        for (uint32_t descriptor_set_index = 0; descriptor_set_index < m_swapchain_ptr->Get_Image_Count(); descriptor_set_index++)
        {
            std::vector<Vulkan::Descriptor> rendering_descriptors = {Vulkan::Descriptor(m_render_target_ptrs[descriptor_set_index], VK_DESCRIPTOR_TYPE_STORAGE_IMAGE), Vulkan::Descriptor(m_camera_buffer_ptrs[descriptor_set_index], VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)};
            m_rendering_descriptor_set_ptrs.push_back(new Vulkan::Descriptor_Set(m_vulkan_graphics_ptr->m_device_ptr, rendering_descriptors));
        }

        // Create compute pipeline
        m_rendering_pipeline_ptr = new Vulkan::Compute_Pipeline(m_vulkan_graphics_ptr->m_device_ptr, "../lib/Cascade_Graphics/src/Vulkan_Backend/Shaders/render.comp", m_rendering_descriptor_set_ptrs);

        // Create and record command buffers
        m_rendering_command_buffers_ptr = new Vulkan::Command_Buffer(m_vulkan_graphics_ptr->m_device_ptr, m_rendering_pipeline_ptr, m_swapchain_ptr->Get_Image_Count(), m_vulkan_graphics_ptr->m_physical_device_ptr->Get_Device_Queues().device_queue_requirements[0].device_queues[0].queue_family_index);
        for (uint32_t i = 0; i < m_swapchain_ptr->Get_Image_Count(); i++)
        {
            m_rendering_command_buffers_ptr->Bind_Descriptor_Set(m_rendering_descriptor_set_ptrs[i], i);

            m_rendering_command_buffers_ptr->Add_Image(m_render_target_ptrs[i]);
            m_rendering_command_buffers_ptr->Add_Image(swapchain_images[i]);

            m_rendering_command_buffers_ptr->Image_Memory_Barrier(swapchain_images[i], VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT).Image_Memory_Barrier(m_render_target_ptrs[i], VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        }
        m_rendering_command_buffers_ptr->Dispatch_Compute_Shader(std::ceil(m_swapchain_ptr->Get_Image_Extent().width / 32.0), std::ceil(m_swapchain_ptr->Get_Image_Extent().height / 32.0), 1);
        for (uint32_t i = 0; i < m_swapchain_ptr->Get_Image_Count(); i++)
        {
            m_rendering_command_buffers_ptr->Image_Memory_Barrier(m_render_target_ptrs[i], VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT)
                .Copy_Image(m_render_target_ptrs[i], swapchain_images[i], m_swapchain_ptr->Get_Image_Extent().width, m_swapchain_ptr->Get_Image_Extent().height)
                .Image_Memory_Barrier(m_render_target_ptrs[i], VK_ACCESS_NONE, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_TRANSFER_BIT)
                .Image_Memory_Barrier(swapchain_images[i], VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_TRANSFER_BIT);
        }
        m_rendering_command_buffers_ptr->Finish_Recording();

        // Create synchronization objects
        m_rendering_complete_semaphores_ptr = new Vulkan::Semaphore(m_vulkan_graphics_ptr->m_device_ptr, m_swapchain_ptr->Get_Image_Count());
        m_image_available_semaphores_ptr = new Vulkan::Semaphore(m_vulkan_graphics_ptr->m_device_ptr, m_swapchain_ptr->Get_Image_Count());
        m_command_buffer_complete_fences_ptr = new Vulkan::Fence(m_vulkan_graphics_ptr->m_device_ptr, m_swapchain_ptr->Get_Image_Count(), true);

        LOG_INFO
            << "Graphics: Finished initializing renderer with Vulkan backend";
    }

    Vulkan_Renderer::~Vulkan_Renderer()
    {
        LOG_INFO << "Graphics: Destroying Vulkan renderer objects";

        VkResult device_wait_idle_result = vkDeviceWaitIdle(m_vulkan_graphics_ptr->m_device_ptr->Get());
        if (device_wait_idle_result != VK_SUCCESS)
        {
            LOG_FATAL << "Graphics (Vulkan): Failed to wait for device idle with code " << device_wait_idle_result << " (" << string_VkResult(device_wait_idle_result) << ")";
            exit(EXIT_FAILURE);
        }

        delete m_rendering_complete_semaphores_ptr;

        delete m_image_available_semaphores_ptr;
        delete m_command_buffer_complete_fences_ptr;
        delete m_rendering_command_buffers_ptr;
        delete m_rendering_pipeline_ptr;

        for (uint32_t i = 0; i < m_swapchain_ptr->Get_Image_Count(); i++)
        {
            delete m_rendering_descriptor_set_ptrs[i];
            delete m_render_target_ptrs[i];
            delete m_camera_buffer_ptrs[i];
        }
        m_rendering_descriptor_set_ptrs.clear();
        m_render_target_ptrs.clear();
        m_camera_buffer_ptrs.clear();

        delete m_swapchain_ptr;
        delete m_surface_ptr;

        LOG_INFO << "Graphics: Finished destroying Vulkan renderer objects";
    }

    void Vulkan_Renderer::Recreate_Swapchain()
    {
        LOG_INFO << "Graphics (Vulkan): Recreating swapchain";

        // Wait for idle
        VkResult device_wait_idle_result = vkDeviceWaitIdle(m_vulkan_graphics_ptr->m_device_ptr->Get());
        if (device_wait_idle_result != VK_SUCCESS)
        {
            LOG_FATAL << "Graphics (Vulkan): Failed to wait for device idle with code " << device_wait_idle_result << " (" << string_VkResult(device_wait_idle_result) << ")";
            exit(EXIT_FAILURE);
        }

        // Create new swapchain
        uint32_t old_swapchain_image_count = m_swapchain_ptr->Get_Image_Count();
        m_window_info_ptr->Update_Window_Info();
        Vulkan::Swapchain* new_swapchain = Vulkan::Swapchain_Builder(m_vulkan_graphics_ptr->m_physical_device_ptr, m_surface_ptr)
                                               .Set_Old_Swapchain(m_swapchain_ptr)
                                               .Select_Image_Format(std::vector<VkSurfaceFormatKHR> {{VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}})
                                               .Select_Image_Extent(m_window_info_ptr)
                                               .Set_Swapchain_Image_Usage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT)
                                               .Select_Present_Mode(std::vector<VkPresentModeKHR> {VK_PRESENT_MODE_IMMEDIATE_KHR})
                                               .Set_Allowed_Queue_Requirements(std::vector<Vulkan::Device_Queue_Requirement*> {&m_vulkan_graphics_ptr->m_physical_device_ptr->Get_Device_Queues().device_queue_requirements[0]})
                                               .Build(m_vulkan_graphics_ptr->m_device_ptr);
        delete m_swapchain_ptr;
        m_swapchain_ptr = new_swapchain;

        std::vector<Vulkan::Image*>
            swapchain_images;
        for (uint32_t swapchain_image_index = 0; swapchain_image_index < m_swapchain_ptr->Get_Image_Count(); swapchain_image_index++)
        {
            swapchain_images.push_back(m_swapchain_ptr->Get_Image_Object(swapchain_image_index));
        }

        if (old_swapchain_image_count != m_swapchain_ptr->Get_Image_Count())
        {
            LOG_ERROR << "Graphics (Vulkan): FIXME: New swapchain has differing image count";
            exit(EXIT_FAILURE);
        }

        // Recreate images
        for (uint32_t image_index = 0; image_index < m_swapchain_ptr->Get_Image_Count(); image_index++)
        {
            delete m_render_target_ptrs[image_index];
            m_render_target_ptrs[image_index] = new Vulkan::Image(m_vulkan_graphics_ptr->m_device_ptr, m_swapchain_ptr->Get_Surface_Format().format, m_swapchain_ptr->Get_Image_Extent(), VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, std::vector<Vulkan::Device_Queue_Requirement*> {&m_vulkan_graphics_ptr->m_physical_device_ptr->Get_Device_Queues().device_queue_requirements[0]}, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0);
        }

        // Update descriptor sets
        for (uint32_t image_index = 0; image_index < m_swapchain_ptr->Get_Image_Count(); image_index++)
        {
            m_rendering_descriptor_set_ptrs[image_index]->Update_Image_Descriptor(0, m_render_target_ptrs[image_index]);
            m_rendering_descriptor_set_ptrs[image_index]->Update();
        }

        // Recreate and rerecord command buffers
        delete m_rendering_command_buffers_ptr;
        m_rendering_command_buffers_ptr = new Vulkan::Command_Buffer(m_vulkan_graphics_ptr->m_device_ptr, m_rendering_pipeline_ptr, m_swapchain_ptr->Get_Image_Count(), m_vulkan_graphics_ptr->m_physical_device_ptr->Get_Device_Queues().device_queue_requirements[0].device_queues[0].queue_family_index);
        for (uint32_t i = 0; i < m_swapchain_ptr->Get_Image_Count(); i++)
        {
            m_rendering_command_buffers_ptr->Bind_Descriptor_Set(m_rendering_descriptor_set_ptrs[i], i);

            m_rendering_command_buffers_ptr->Add_Image(m_render_target_ptrs[i]);
            m_rendering_command_buffers_ptr->Add_Image(swapchain_images[i]);

            m_rendering_command_buffers_ptr->Image_Memory_Barrier(swapchain_images[i], VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT).Image_Memory_Barrier(m_render_target_ptrs[i], VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        }
        m_rendering_command_buffers_ptr->Dispatch_Compute_Shader(std::ceil(m_swapchain_ptr->Get_Image_Extent().width / 32.0), std::ceil(m_swapchain_ptr->Get_Image_Extent().height / 32.0), 1);
        for (uint32_t i = 0; i < m_swapchain_ptr->Get_Image_Count(); i++)
        {
            m_rendering_command_buffers_ptr->Image_Memory_Barrier(m_render_target_ptrs[i], VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT)
                .Copy_Image(m_render_target_ptrs[i], swapchain_images[i], m_swapchain_ptr->Get_Image_Extent().width, m_swapchain_ptr->Get_Image_Extent().height)
                .Image_Memory_Barrier(m_render_target_ptrs[i], VK_ACCESS_NONE, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_TRANSFER_BIT)
                .Image_Memory_Barrier(swapchain_images[i], VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_TRANSFER_BIT);
        }
        m_rendering_command_buffers_ptr->Finish_Recording();
    }

    void Vulkan_Renderer::Render_Frame()
    {
        // Block cpu until this command buffer is idle again (blocks cpu, can be moved after acquire image?)
        vkWaitForFences(m_vulkan_graphics_ptr->m_device_ptr->Get(), 1, m_command_buffer_complete_fences_ptr->Get(m_active_command_buffer_index), VK_TRUE, UINT32_MAX);
        vkResetFences(m_vulkan_graphics_ptr->m_device_ptr->Get(), 1, m_command_buffer_complete_fences_ptr->Get(m_active_command_buffer_index));

        // Update camera buffer
        Camera::GPU_Camera_Data camera_data = m_camera.Get_GPU_Camera_Data();
        m_camera_buffer_ptrs[m_active_command_buffer_index]->Direct_Upload_To_Buffer(&camera_data, sizeof(Camera::GPU_Camera_Data));

        // Get the next swapchain image
        uint32_t image_index;
        VkResult acquire_next_image_result = vkAcquireNextImageKHR(m_vulkan_graphics_ptr->m_device_ptr->Get(), *m_swapchain_ptr->Get(), UINT32_MAX, *m_image_available_semaphores_ptr->Get(m_active_command_buffer_index), VK_NULL_HANDLE, &image_index);
        if (acquire_next_image_result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            Recreate_Swapchain();
            m_active_command_buffer_index = (m_active_command_buffer_index + 1) % m_swapchain_ptr->Get_Image_Count();
            return;
        }
        else if (acquire_next_image_result != VK_SUCCESS)
        {
            LOG_FATAL << "Graphics (Vulkan): Failed to acquire next swapchain image with code " << acquire_next_image_result << " (" << string_VkResult(acquire_next_image_result) << ")";
            exit(EXIT_FAILURE);
        }

        // Submit command buffer (waits until image is available on gpu, cpu continues)
        VkPipelineStageFlags wait_on_stage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pNext = NULL;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = m_image_available_semaphores_ptr->Get(m_active_command_buffer_index);
        submit_info.pWaitDstStageMask = &wait_on_stage;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = m_rendering_command_buffers_ptr->Get(m_active_command_buffer_index);
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = m_rendering_complete_semaphores_ptr->Get(m_active_command_buffer_index);

        VkResult queue_submit_result = vkQueueSubmit(m_vulkan_graphics_ptr->m_device_ptr->Get_Device_Queues()->device_queue_requirements[0].device_queues[0].queue, 1, &submit_info, *m_command_buffer_complete_fences_ptr->Get(m_active_command_buffer_index));
        if (queue_submit_result != VK_SUCCESS)
        {
            LOG_FATAL << "Graphics (Vulkan): Failed to submit queue";
            exit(EXIT_FAILURE);
        }

        // Present swapchain image (waits until rendering is complete, doesn't block cpu)
        VkPresentInfoKHR present_info = {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.pNext = NULL;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = m_rendering_complete_semaphores_ptr->Get(m_active_command_buffer_index);
        present_info.swapchainCount = 1;
        present_info.pSwapchains = m_swapchain_ptr->Get();
        present_info.pImageIndices = &image_index;
        present_info.pResults = nullptr;

        VkResult queue_present_result = vkQueuePresentKHR(m_vulkan_graphics_ptr->m_device_ptr->Get_Device_Queues()->device_queue_requirements[0].device_queues[0].queue, &present_info);
        if (queue_present_result == VK_ERROR_OUT_OF_DATE_KHR || queue_present_result == VK_SUBOPTIMAL_KHR)
        {
            Recreate_Swapchain();
            m_active_command_buffer_index = (m_active_command_buffer_index + 1) % m_swapchain_ptr->Get_Image_Count();
            return;
        }
        else if (queue_present_result != VK_SUCCESS)
        {
            LOG_FATAL << "Graphics (Vulkan): Failed to present swapchain code " << queue_present_result << " (" << string_VkResult(queue_present_result) << ")";
            exit(EXIT_FAILURE);
        }

        // Update active command buffer
        m_active_command_buffer_index = (m_active_command_buffer_index + 1) % m_swapchain_ptr->Get_Image_Count();
    }
}    // namespace Cascade_Graphics