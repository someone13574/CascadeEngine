#include "renderer.hpp"

#include "cascade_logging.hpp"

#include <cmath>
#include <memory>

namespace CGV = Cascade_Graphics::Vulkan;

namespace Cascade_Core
{
    Renderer::Renderer(Cascade_Graphics::Vulkan::Surface::Window_Data window_data, unsigned int width, unsigned int height) : m_width(width), m_height(height)
    {
        m_initialized = false;

        LOG_INFO << "Initializing renderer";

        instance_ptr = std::make_shared<CGV::Instance>("Application name", 0);

        surface_ptr = std::make_shared<CGV::Surface>(window_data, instance_ptr);

        validation_layer_ptr = std::make_shared<Cascade_Graphics_Debugging::Vulkan::Validation_Layer>(instance_ptr);

        queue_manager_ptr = std::make_shared<CGV::Queue_Manager>(false, true, true, false, false, true, surface_ptr);

        physical_device_ptr = std::make_shared<CGV::Physical_Device>(instance_ptr, queue_manager_ptr, surface_ptr);

        queue_manager_ptr->Set_Queue_Family_Indices(physical_device_ptr);

        logical_device_ptr = std::make_shared<CGV::Device>(queue_manager_ptr, validation_layer_ptr, physical_device_ptr);

        swapchain_ptr = std::make_shared<CGV::Swapchain>(logical_device_ptr, physical_device_ptr, surface_ptr, queue_manager_ptr, m_width, m_height);

        storage_manager_ptr = std::make_shared<CGV::Storage_Manager>(logical_device_ptr, physical_device_ptr, queue_manager_ptr, swapchain_ptr);
        storage_manager_ptr->Create_Image("render_target", VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, {width, height}, {false, true, true, false, false, false});
        storage_manager_ptr->Create_Buffer("camera_data", sizeof(Cascade_Graphics::Camera::GPU_Camera_Data), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, {false, true, true, false, false, false});
        storage_manager_ptr->Add_Swapchain("swapchain");

        shader_manager_ptr = std::make_shared<CGV::Shader_Manager>(logical_device_ptr);

#if defined __linux__
        shader_manager_ptr->Add_Shader("render_shader", "/home/owen/Documents/Code/C++/CascadeEngine/build/build/build/Cascade_Graphics/src/Shaders/render.comp.spv");
#elif defined _WIN32 || defined WIN32
        shader_manager_ptr->Add_Shader("render_shader", "C:/Users/Owen Law/Documents/Code/C++/CascadeEngine/build/build/build/Cascade_Graphics/src/Shaders/render.comp.spv");
#endif

        resource_grouping_manager = std::make_shared<CGV::Resource_Grouping_Manager>(logical_device_ptr, storage_manager_ptr);
        resource_grouping_manager->Add_Resource_Grouping("per_frame_descriptor_set", {{0, "render_target", CGV::Storage_Manager::IMAGE}, {0, "camera_data", CGV::Storage_Manager::BUFFER}}, true);
        resource_grouping_manager->Add_Resource_Grouping("swapchain", {{0, "swapchain", CGV::Storage_Manager::SWAPCHAIN_IMAGE}, {1, "swapchain", CGV::Storage_Manager::SWAPCHAIN_IMAGE}, {2, "swapchain", CGV::Storage_Manager::SWAPCHAIN_IMAGE}}, false);
        resource_grouping_manager->Create_Descriptor_Sets();

        pipeline_manager_ptr = std::make_shared<CGV::Pipeline_Manager>(resource_grouping_manager, logical_device_ptr, storage_manager_ptr, shader_manager_ptr);
        pipeline_manager_ptr->Add_Compute_Pipeline("rendering_pipeline", "per_frame_descriptor_set", "render_shader");

        command_buffer_manager_ptr = std::make_shared<CGV::Command_Buffer_Manager>(resource_grouping_manager, logical_device_ptr, pipeline_manager_ptr, storage_manager_ptr);

        command_buffer_manager_ptr->Add_Command_Buffer("render_frame", queue_manager_ptr->Get_Queue_Family_Indices().m_compute_index.value(), {"per_frame_descriptor_set", "swapchain"}, "rendering_pipeline");
        command_buffer_manager_ptr->Begin_Recording({"render_frame", 0}, (VkCommandBufferUsageFlagBits)0);
        command_buffer_manager_ptr->Image_Memory_Barrier({"render_frame", 0}, {0, "swapchain", CGV::Storage_Manager::SWAPCHAIN_IMAGE}, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT);
        command_buffer_manager_ptr->Image_Memory_Barrier({"render_frame", 0}, {0, "render_target", CGV::Storage_Manager::IMAGE}, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        command_buffer_manager_ptr->Dispatch_Compute_Shader({"render_frame", 0}, std::ceil(m_width / 32.0), std::ceil(m_width / 32.0), 1);
        command_buffer_manager_ptr->Image_Memory_Barrier({"render_frame", 0}, {0, "render_target", CGV::Storage_Manager::IMAGE}, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT);
        command_buffer_manager_ptr->Copy_Image({"render_frame", 0}, {0, "render_target", CGV::Storage_Manager::IMAGE}, {0, "swapchain", CGV::Storage_Manager::SWAPCHAIN_IMAGE});
        command_buffer_manager_ptr->Image_Memory_Barrier({"render_frame", 0}, {0, "render_target", CGV::Storage_Manager::IMAGE}, VK_ACCESS_NONE, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_TRANSFER_BIT);
        command_buffer_manager_ptr->Image_Memory_Barrier({"render_frame", 0}, {0, "swapchain", CGV::Storage_Manager::SWAPCHAIN_IMAGE}, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_TRANSFER_BIT);
        command_buffer_manager_ptr->End_Recording({"render_frame", 0});

        command_buffer_manager_ptr->Add_Command_Buffer("render_frame", queue_manager_ptr->Get_Queue_Family_Indices().m_compute_index.value(), {"per_frame_descriptor_set", "swapchain"}, "rendering_pipeline");
        command_buffer_manager_ptr->Begin_Recording({"render_frame", 1}, (VkCommandBufferUsageFlagBits)0);
        command_buffer_manager_ptr->Image_Memory_Barrier({"render_frame", 1}, {1, "swapchain", CGV::Storage_Manager::SWAPCHAIN_IMAGE}, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT);
        command_buffer_manager_ptr->Image_Memory_Barrier({"render_frame", 1}, {0, "render_target", CGV::Storage_Manager::IMAGE}, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        command_buffer_manager_ptr->Dispatch_Compute_Shader({"render_frame", 1}, std::ceil(m_width / 32.0), std::ceil(m_width / 32.0), 1);
        command_buffer_manager_ptr->Image_Memory_Barrier({"render_frame", 1}, {0, "render_target", CGV::Storage_Manager::IMAGE}, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT);
        command_buffer_manager_ptr->Copy_Image({"render_frame", 1}, {0, "render_target", CGV::Storage_Manager::IMAGE}, {1, "swapchain", CGV::Storage_Manager::SWAPCHAIN_IMAGE});
        command_buffer_manager_ptr->Image_Memory_Barrier({"render_frame", 1}, {0, "render_target", CGV::Storage_Manager::IMAGE}, VK_ACCESS_NONE, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_TRANSFER_BIT);
        command_buffer_manager_ptr->Image_Memory_Barrier({"render_frame", 1}, {1, "swapchain", CGV::Storage_Manager::SWAPCHAIN_IMAGE}, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_TRANSFER_BIT);
        command_buffer_manager_ptr->End_Recording({"render_frame", 1});

        command_buffer_manager_ptr->Add_Command_Buffer("render_frame", queue_manager_ptr->Get_Queue_Family_Indices().m_compute_index.value(), {"per_frame_descriptor_set", "swapchain"}, "rendering_pipeline");
        command_buffer_manager_ptr->Begin_Recording({"render_frame", 2}, (VkCommandBufferUsageFlagBits)0);
        command_buffer_manager_ptr->Image_Memory_Barrier({"render_frame", 2}, {2, "swapchain", CGV::Storage_Manager::SWAPCHAIN_IMAGE}, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT);
        command_buffer_manager_ptr->Image_Memory_Barrier({"render_frame", 2}, {0, "render_target", CGV::Storage_Manager::IMAGE}, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        command_buffer_manager_ptr->Dispatch_Compute_Shader({"render_frame", 2}, std::ceil(m_width / 32.0), std::ceil(m_width / 32.0), 1);
        command_buffer_manager_ptr->Image_Memory_Barrier({"render_frame", 2}, {0, "render_target", CGV::Storage_Manager::IMAGE}, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT);
        command_buffer_manager_ptr->Copy_Image({"render_frame", 2}, {0, "render_target", CGV::Storage_Manager::IMAGE}, {2, "swapchain", CGV::Storage_Manager::SWAPCHAIN_IMAGE});
        command_buffer_manager_ptr->Image_Memory_Barrier({"render_frame", 2}, {0, "render_target", CGV::Storage_Manager::IMAGE}, VK_ACCESS_NONE, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_TRANSFER_BIT);
        command_buffer_manager_ptr->Image_Memory_Barrier({"render_frame", 2}, {2, "swapchain", CGV::Storage_Manager::SWAPCHAIN_IMAGE}, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_TRANSFER_BIT);
        command_buffer_manager_ptr->End_Recording({"render_frame", 2});

        synchronization_manager_ptr = std::make_shared<CGV::Synchronization_Manager>(logical_device_ptr);
        synchronization_manager_ptr->Create_Semaphore("swapchain_image_available");
        synchronization_manager_ptr->Create_Semaphore("swapchain_image_available");
        synchronization_manager_ptr->Create_Semaphore("swapchain_image_available");
        synchronization_manager_ptr->Create_Semaphore("render_finished");
        synchronization_manager_ptr->Create_Semaphore("render_finished");
        synchronization_manager_ptr->Create_Semaphore("render_finished");
        synchronization_manager_ptr->Create_Fence("in_flight");
        synchronization_manager_ptr->Create_Fence("in_flight");
        synchronization_manager_ptr->Create_Fence("in_flight");
        synchronization_manager_ptr->Add_Fence("image_in_flight", VK_NULL_HANDLE);
        synchronization_manager_ptr->Add_Fence("image_in_flight", VK_NULL_HANDLE);
        synchronization_manager_ptr->Add_Fence("image_in_flight", VK_NULL_HANDLE);

        m_camera_ptr = std::make_shared<Cascade_Graphics::Camera>(Cascade_Graphics::Vector_3(-3.0, 0.0, 0.0), Cascade_Graphics::Vector_3(1.0, 0.0, 0.0));

        LOG_INFO << "Renderer initialized";

        m_initialized = true;
    }

    Renderer::~Renderer()
    {
        LOG_TRACE << "Renderer cleanup started";

        LOG_TRACE << "Renderer cleanup finished";
    }

    void Renderer::Render_Frame()
    {
        if (m_initialized)
        {
            vkWaitForFences(*(logical_device_ptr->Get_Device()), 1, synchronization_manager_ptr->Get_Fence({"in_flight", m_current_frame}), VK_TRUE, UINT64_MAX);

            unsigned int image_index;
            VALIDATE_VKRESULT(
                vkAcquireNextImageKHR(*(logical_device_ptr->Get_Device()), *(swapchain_ptr->Get_Swapchain()), UINT64_MAX, *synchronization_manager_ptr->Get_Semaphore({"swapchain_image_available", m_current_frame}), VK_NULL_HANDLE, &image_index),
                "Vulkan: failed to acquire next swapchain image");

            if (*synchronization_manager_ptr->Get_Fence({"image_in_flight", m_current_frame}) != VK_NULL_HANDLE)
            {
                vkWaitForFences(*(logical_device_ptr->Get_Device()), 1, synchronization_manager_ptr->Get_Fence({"image_in_flight", m_current_frame}), VK_TRUE, UINT64_MAX);
            }
            *(synchronization_manager_ptr->Get_Fence({"image_in_flight", m_current_frame})) = *(synchronization_manager_ptr->Get_Fence({"in_flight", m_current_frame}));

            VkPipelineStageFlags pipeline_wait_stage_mask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

            //

            Cascade_Graphics::Camera::GPU_Camera_Data camera_data = {};
            camera_data.matrix_x0 = m_camera_ptr->Get_Camera_To_World_Matrix().m_x0;
            camera_data.matrix_x1 = m_camera_ptr->Get_Camera_To_World_Matrix().m_x1;
            camera_data.matrix_x2 = m_camera_ptr->Get_Camera_To_World_Matrix().m_x2;
            camera_data.matrix_y0 = m_camera_ptr->Get_Camera_To_World_Matrix().m_y0;
            camera_data.matrix_y1 = m_camera_ptr->Get_Camera_To_World_Matrix().m_y1;
            camera_data.matrix_y2 = m_camera_ptr->Get_Camera_To_World_Matrix().m_y2;
            camera_data.matrix_z0 = m_camera_ptr->Get_Camera_To_World_Matrix().m_z0;
            camera_data.matrix_z1 = m_camera_ptr->Get_Camera_To_World_Matrix().m_z1;
            camera_data.matrix_z2 = m_camera_ptr->Get_Camera_To_World_Matrix().m_z2;
            camera_data.origin_x = m_camera_ptr->Get_Camera_Position().m_x;
            camera_data.origin_y = m_camera_ptr->Get_Camera_Position().m_y;
            camera_data.origin_z = m_camera_ptr->Get_Camera_Position().m_z;

            storage_manager_ptr->Upload_To_Buffer({0, "camera_data", CGV::Storage_Manager::Resource_Type::BUFFER}, &camera_data, sizeof(Cascade_Graphics::Camera::GPU_Camera_Data));

            //

            VkSubmitInfo submit_info = {};
            submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submit_info.pNext = nullptr;
            submit_info.waitSemaphoreCount = 1;
            submit_info.pWaitSemaphores = synchronization_manager_ptr->Get_Semaphore({"swapchain_image_available", m_current_frame});
            submit_info.pWaitDstStageMask = &pipeline_wait_stage_mask;
            submit_info.commandBufferCount = 1;
            submit_info.pCommandBuffers = command_buffer_manager_ptr->Get_Command_Buffer({"render_frame", image_index});
            submit_info.signalSemaphoreCount = 1;
            submit_info.pSignalSemaphores = synchronization_manager_ptr->Get_Semaphore({"render_finished", m_current_frame});

            vkResetFences(*(logical_device_ptr->Get_Device()), 1, synchronization_manager_ptr->Get_Fence({"in_flight", m_current_frame}));

            VALIDATE_VKRESULT(vkQueueSubmit(*(queue_manager_ptr->Get_Queue(1)), 1, &submit_info, *synchronization_manager_ptr->Get_Fence({"in_flight", m_current_frame})), "Vulkan: failed to submit queue");

            VkPresentInfoKHR present_info = {};
            present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            present_info.pNext = nullptr;
            present_info.waitSemaphoreCount = 1;
            present_info.pWaitSemaphores = synchronization_manager_ptr->Get_Semaphore({"render_finished", m_current_frame});
            present_info.swapchainCount = 1;
            present_info.pSwapchains = swapchain_ptr->Get_Swapchain();
            present_info.pImageIndices = &image_index;
            present_info.pResults = nullptr;

            VALIDATE_VKRESULT(vkQueuePresentKHR(*(queue_manager_ptr->Get_Queue(5)), &present_info), "Vulkan: failed to present");

            m_current_frame = (m_current_frame + 1) % 3;
        }
    }

    std::shared_ptr<Cascade_Graphics::Camera> Renderer::Get_Camera()
    {
        return m_camera_ptr;
    }
} // namespace Cascade_Core