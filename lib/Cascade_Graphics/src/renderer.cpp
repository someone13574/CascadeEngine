#include "renderer.hpp"

#include "cascade_logging.hpp"

namespace Cascade_Graphics
{
    Renderer::Renderer(Window_Information window_information) : m_window_information(window_information)
    {
#ifdef CSD_USE_VULKAN
        LOG_DEBUG << "Graphics: Creating renderer with Vulkan backend";

        Initialize_Vulkan();
#else
        LOG_ERROR << "Graphics: Currently only the Vulkan backend is supported. Recompile with CSD_USE_VULKAN";
        exit(EXIT_FAILURE);
#endif

        m_camera_ptr = std::make_shared<Camera>(Vector_3<double>(-3.0, 0.0, 0.0), Vector_3<double>(1.0, 0.0, 0.0));

        m_renderer_initialized = true;
        LOG_DEBUG << "Graphics: Renderer initialized";
    }

#ifdef CSD_USE_VULKAN

    void Renderer::Initialize_Vulkan()
    {
        unsigned int width = *m_window_information.width_ptr;
        unsigned int height = *m_window_information.height_ptr;

        m_instance_ptr = std::make_shared<Vulkan::Instance>("Application name", 0);
        m_surface_ptr = std::make_shared<Vulkan::Surface>(m_window_information, m_instance_ptr);
        m_validation_layer_ptr = std::make_shared<Cascade_Graphics_Debugging::Vulkan::Validation_Layer>(m_instance_ptr);
        m_queue_manager_ptr = std::make_shared<Vulkan::Queue_Manager>(false, true, true, false, false, true, m_surface_ptr);

        m_physical_device_ptr = std::make_shared<Vulkan::Physical_Device>(m_instance_ptr, m_queue_manager_ptr, m_surface_ptr);
        m_queue_manager_ptr->Set_Queue_Family_Indices(m_physical_device_ptr);

        m_logical_device_ptr = std::make_shared<Vulkan::Device>(m_queue_manager_ptr, m_validation_layer_ptr, m_physical_device_ptr);
        m_swapchain_ptr = std::make_shared<Vulkan::Swapchain>(m_logical_device_ptr, m_physical_device_ptr, m_surface_ptr, m_queue_manager_ptr, width, height);

        m_storage_manager_ptr = std::make_shared<Vulkan::Storage_Manager>(m_logical_device_ptr, m_physical_device_ptr, m_queue_manager_ptr, m_swapchain_ptr);
        m_storage_manager_ptr->Create_Image("render_target", VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, {width, height}, {false, true, true, false, false, false});
        m_storage_manager_ptr->Create_Buffer("camera_data", sizeof(Cascade_Graphics::Camera::GPU_Camera_Data), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, {false, true, true, false, false, false});
        m_storage_manager_ptr->Create_Buffer("voxel_buffer", sizeof(Cascade_Graphics::Object::GPU_Voxel) * 2, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, {false, true, true, false, false, false});
        m_storage_manager_ptr->Add_Swapchain("swapchain");

        m_shader_manager_ptr = std::make_shared<Vulkan::Shader_Manager>(m_logical_device_ptr);
#if defined __linux__
        m_shader_manager_ptr->Add_Shader("render_shader", "/home/owen/Documents/Code/C++/CascadeEngine/build/build/build/Cascade_Graphics/src/Shaders/render.comp.spv");
#elif defined _WIN32 || defined WIN32
        m_shader_manager_ptr->Add_Shader("render_shader", "C:/Users/Owen Law/Documents/Code/C++/CascadeEngine/build/build/build/Cascade_Graphics/src/Shaders/render.comp.spv");
#endif

        m_resource_grouping_manager_ptr = std::make_shared<Vulkan::Resource_Grouping_Manager>(m_logical_device_ptr, m_storage_manager_ptr);
        m_resource_grouping_manager_ptr->Add_Resource_Grouping("per_frame_descriptor_set",
                                                               {{0, "render_target", Vulkan::Storage_Manager::IMAGE}, {0, "camera_data", Vulkan::Storage_Manager::BUFFER}, {0, "voxel_buffer", Vulkan::Storage_Manager::BUFFER}}, true);
        m_resource_grouping_manager_ptr->Add_Resource_Grouping(
            "swapchain", {{0, "swapchain", Vulkan::Storage_Manager::SWAPCHAIN_IMAGE}, {1, "swapchain", Vulkan::Storage_Manager::SWAPCHAIN_IMAGE}, {2, "swapchain", Vulkan::Storage_Manager::SWAPCHAIN_IMAGE}}, false);
        m_resource_grouping_manager_ptr->Create_Descriptor_Sets();

        m_pipeline_manager_ptr = std::make_shared<Vulkan::Pipeline_Manager>(m_resource_grouping_manager_ptr, m_logical_device_ptr, m_storage_manager_ptr, m_shader_manager_ptr);
        m_pipeline_manager_ptr->Add_Compute_Pipeline("rendering_pipeline", "per_frame_descriptor_set", "render_shader");

        m_command_buffer_manager_ptr = std::make_shared<Vulkan::Command_Buffer_Manager>(m_resource_grouping_manager_ptr, m_logical_device_ptr, m_pipeline_manager_ptr, m_storage_manager_ptr);
        Record_Vulkan_Command_Buffers(width, height);

        m_synchronization_manager_ptr = std::make_shared<Vulkan::Synchronization_Manager>(m_logical_device_ptr);
        m_synchronization_manager_ptr->Create_Semaphore("swapchain_image_available");
        m_synchronization_manager_ptr->Create_Semaphore("swapchain_image_available");
        m_synchronization_manager_ptr->Create_Semaphore("swapchain_image_available");
        m_synchronization_manager_ptr->Create_Semaphore("render_finished");
        m_synchronization_manager_ptr->Create_Semaphore("render_finished");
        m_synchronization_manager_ptr->Create_Semaphore("render_finished");
        m_synchronization_manager_ptr->Create_Fence("in_flight");
        m_synchronization_manager_ptr->Create_Fence("in_flight");
        m_synchronization_manager_ptr->Create_Fence("in_flight");
        m_synchronization_manager_ptr->Add_Fence("image_in_flight", VK_NULL_HANDLE);
        m_synchronization_manager_ptr->Add_Fence("image_in_flight", VK_NULL_HANDLE);
        m_synchronization_manager_ptr->Add_Fence("image_in_flight", VK_NULL_HANDLE);


        Cascade_Graphics::Object::GPU_Voxel gpu_voxel_a = {};
        gpu_voxel_a.x_position = -0.5;
        gpu_voxel_a.y_position = 0.0;
        gpu_voxel_a.z_position = 0.0;
        gpu_voxel_a.size = 0.25;

        Cascade_Graphics::Object::GPU_Voxel gpu_voxel_b = {};
        gpu_voxel_b.x_position = 0.5;
        gpu_voxel_b.y_position = 0.0;
        gpu_voxel_b.z_position = 0.0;
        gpu_voxel_b.size = 0.25;

        Cascade_Graphics::Object::GPU_Voxel gpu_voxels[2] = {gpu_voxel_a, gpu_voxel_b};
        m_storage_manager_ptr->Upload_To_Buffer({0, "voxel_buffer", Vulkan::Storage_Manager::Resource_Type::BUFFER}, &gpu_voxels, sizeof(Object::GPU_Voxel) * 2);
    }

    void Renderer::Record_Vulkan_Command_Buffers(unsigned int width, unsigned int height)
    {
        if (m_command_buffer_manager_ptr != nullptr)
        {
            for (unsigned int i = 0; i < 3; i++)
            {
                m_command_buffer_manager_ptr->Add_Command_Buffer("render_frame", m_queue_manager_ptr->Get_Queue_Family_Indices().m_compute_index.value(), {"per_frame_descriptor_set", "swapchain"}, "rendering_pipeline");
                m_command_buffer_manager_ptr->Begin_Recording({"render_frame", i}, (VkCommandBufferUsageFlagBits)0);
                m_command_buffer_manager_ptr->Image_Memory_Barrier({"render_frame", i}, {i, "swapchain", Vulkan::Storage_Manager::SWAPCHAIN_IMAGE}, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT);
                m_command_buffer_manager_ptr->Image_Memory_Barrier({"render_frame", i}, {0, "render_target", Vulkan::Storage_Manager::IMAGE}, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
                m_command_buffer_manager_ptr->Dispatch_Compute_Shader({"render_frame", i}, std::ceil(width / 32.0), std::ceil(height / 32.0), 1);
                m_command_buffer_manager_ptr->Image_Memory_Barrier({"render_frame", i}, {0, "render_target", Vulkan::Storage_Manager::IMAGE}, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT);
                m_command_buffer_manager_ptr->Copy_Image({"render_frame", i}, {0, "render_target", Vulkan::Storage_Manager::IMAGE}, {i, "swapchain", Vulkan::Storage_Manager::SWAPCHAIN_IMAGE}, width, height);
                m_command_buffer_manager_ptr->Image_Memory_Barrier({"render_frame", i}, {0, "render_target", Vulkan::Storage_Manager::IMAGE}, VK_ACCESS_NONE, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_TRANSFER_BIT);
                m_command_buffer_manager_ptr->Image_Memory_Barrier({"render_frame", i}, {i, "swapchain", Vulkan::Storage_Manager::SWAPCHAIN_IMAGE}, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_TRANSFER_BIT);
                m_command_buffer_manager_ptr->End_Recording({"render_frame", i});
            }
        }
        else
        {
            LOG_ERROR << "Vulkan: Command buffer manager does not have value";
            exit(EXIT_FAILURE);
        }
    }

    void Renderer::Recreate_Swapchain()
    {
        vkDeviceWaitIdle(*m_logical_device_ptr->Get_Device());

        m_command_buffer_manager_ptr.reset();
        m_pipeline_manager_ptr.reset();
        m_resource_grouping_manager_ptr.reset();
        m_storage_manager_ptr.reset();
        m_swapchain_ptr.reset();

        unsigned int width = *m_window_information.width_ptr;
        unsigned int height = *m_window_information.height_ptr;

        LOG_DEBUG << "Vulkan: New window size " << width << "x" << height;

        m_swapchain_ptr = std::make_shared<Vulkan::Swapchain>(m_logical_device_ptr, m_physical_device_ptr, m_surface_ptr, m_queue_manager_ptr, width, height);

        m_storage_manager_ptr = std::make_shared<Vulkan::Storage_Manager>(m_logical_device_ptr, m_physical_device_ptr, m_queue_manager_ptr, m_swapchain_ptr);
        m_storage_manager_ptr->Create_Image("render_target", VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, {width, height}, {false, true, true, false, false, false});
        m_storage_manager_ptr->Create_Buffer("camera_data", sizeof(Cascade_Graphics::Camera::GPU_Camera_Data), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, {false, true, true, false, false, false});
        m_storage_manager_ptr->Create_Buffer("voxel_buffer", sizeof(Cascade_Graphics::Object::GPU_Voxel) * 2, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, {false, true, true, false, false, false});
        m_storage_manager_ptr->Add_Swapchain("swapchain");

        m_resource_grouping_manager_ptr = std::make_shared<Vulkan::Resource_Grouping_Manager>(m_logical_device_ptr, m_storage_manager_ptr);
        m_resource_grouping_manager_ptr->Add_Resource_Grouping("per_frame_descriptor_set",
                                                               {{0, "render_target", Vulkan::Storage_Manager::IMAGE}, {0, "camera_data", Vulkan::Storage_Manager::BUFFER}, {0, "voxel_buffer", Vulkan::Storage_Manager::BUFFER}}, true);
        m_resource_grouping_manager_ptr->Add_Resource_Grouping(
            "swapchain", {{0, "swapchain", Vulkan::Storage_Manager::SWAPCHAIN_IMAGE}, {1, "swapchain", Vulkan::Storage_Manager::SWAPCHAIN_IMAGE}, {2, "swapchain", Vulkan::Storage_Manager::SWAPCHAIN_IMAGE}}, false);
        m_resource_grouping_manager_ptr->Create_Descriptor_Sets();

        m_pipeline_manager_ptr = std::make_shared<Vulkan::Pipeline_Manager>(m_resource_grouping_manager_ptr, m_logical_device_ptr, m_storage_manager_ptr, m_shader_manager_ptr);
        m_pipeline_manager_ptr->Add_Compute_Pipeline("rendering_pipeline", "per_frame_descriptor_set", "render_shader");

        m_command_buffer_manager_ptr = std::make_shared<Vulkan::Command_Buffer_Manager>(m_resource_grouping_manager_ptr, m_logical_device_ptr, m_pipeline_manager_ptr, m_storage_manager_ptr);
        Record_Vulkan_Command_Buffers(width, height);

        Object::GPU_Voxel gpu_voxel_a = {};
        gpu_voxel_a.x_position = -0.5;
        gpu_voxel_a.y_position = 0.0;
        gpu_voxel_a.z_position = 0.0;
        gpu_voxel_a.size = 0.25;

        Object::GPU_Voxel gpu_voxel_b = {};
        gpu_voxel_b.x_position = 0.5;
        gpu_voxel_b.y_position = 0.0;
        gpu_voxel_b.z_position = 0.0;
        gpu_voxel_b.size = 0.25;

        Object::GPU_Voxel gpu_voxels[2] = {gpu_voxel_a, gpu_voxel_b};
        m_storage_manager_ptr->Upload_To_Buffer({0, "voxel_buffer", Vulkan::Storage_Manager::Resource_Type::BUFFER}, &gpu_voxels, sizeof(Object::GPU_Voxel) * 2);
    }


#endif

    void Renderer::Render_Frame()
    {
#ifdef CSD_USE_VULKAN
        if (m_renderer_initialized)
        {
            vkWaitForFences(*m_logical_device_ptr->Get_Device(), 1, m_synchronization_manager_ptr->Get_Fence({"in_flight", m_current_frame}), VK_TRUE, UINT64_MAX);

            unsigned int image_index;
            VkResult acquire_next_image_result
                = vkAcquireNextImageKHR(*m_logical_device_ptr->Get_Device(), *m_swapchain_ptr->Get_Swapchain(), UINT64_MAX, *m_synchronization_manager_ptr->Get_Semaphore({"swapchain_image_available", m_current_frame}), VK_NULL_HANDLE, &image_index);

            if (acquire_next_image_result == VK_ERROR_OUT_OF_DATE_KHR)
            {
                LOG_DEBUG << "Vulkan: Swapchain is out of date";

                return;
            }
            else
            {
                VALIDATE_VKRESULT(acquire_next_image_result, "Vulkan: Failed to acquire next swapchain image");
            }

            if (*m_synchronization_manager_ptr->Get_Fence({"image_in_flight", m_current_frame}) != VK_NULL_HANDLE)
            {
                vkWaitForFences(*m_logical_device_ptr->Get_Device(), 1, m_synchronization_manager_ptr->Get_Fence({"image_in_flight", m_current_frame}), VK_TRUE, UINT64_MAX);
            }
            *m_synchronization_manager_ptr->Get_Fence({"image_in_flight", m_current_frame}) = *m_synchronization_manager_ptr->Get_Fence({"in_flight", m_current_frame});

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

            m_storage_manager_ptr->Upload_To_Buffer({0, "camera_data", Vulkan::Storage_Manager::Resource_Type::BUFFER}, &camera_data, sizeof(Cascade_Graphics::Camera::GPU_Camera_Data));

            //

            VkSubmitInfo submit_info = {};
            submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submit_info.pNext = nullptr;
            submit_info.waitSemaphoreCount = 1;
            submit_info.pWaitSemaphores = m_synchronization_manager_ptr->Get_Semaphore({"swapchain_image_available", m_current_frame});
            submit_info.pWaitDstStageMask = &pipeline_wait_stage_mask;
            submit_info.commandBufferCount = 1;
            submit_info.pCommandBuffers = m_command_buffer_manager_ptr->Get_Command_Buffer({"render_frame", image_index});
            submit_info.signalSemaphoreCount = 1;
            submit_info.pSignalSemaphores = m_synchronization_manager_ptr->Get_Semaphore({"render_finished", m_current_frame});

            vkResetFences(*m_logical_device_ptr->Get_Device(), 1, m_synchronization_manager_ptr->Get_Fence({"in_flight", m_current_frame}));

            VALIDATE_VKRESULT(vkQueueSubmit(*m_queue_manager_ptr->Get_Queue(1), 1, &submit_info, *m_synchronization_manager_ptr->Get_Fence({"in_flight", m_current_frame})), "Vulkan: Failed to submit queue");

            VkPresentInfoKHR present_info = {};
            present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            present_info.pNext = nullptr;
            present_info.waitSemaphoreCount = 1;
            present_info.pWaitSemaphores = m_synchronization_manager_ptr->Get_Semaphore({"render_finished", m_current_frame});
            present_info.swapchainCount = 1;
            present_info.pSwapchains = m_swapchain_ptr->Get_Swapchain();
            present_info.pImageIndices = &image_index;
            present_info.pResults = nullptr;

            VkResult present_result = vkQueuePresentKHR(*m_queue_manager_ptr->Get_Queue(5), &present_info);
            if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR)
            {
                LOG_DEBUG << "Vulkan: Swapchain is out of date or suboptimal";
                // exit(EXIT_FAILURE);
                Recreate_Swapchain();

                return;
            }
            else
            {
                VALIDATE_VKRESULT(present_result, "Vulkan: Failed to present");
            }

            m_current_frame = (m_current_frame + 1) % 3;
        }
#endif
    }

    std::shared_ptr<Camera> Renderer::Get_Camera()
    {
        return m_camera_ptr;
    }
} // namespace Cascade_Graphics