#include "renderer.hpp"

#include "Vulkan_Wrapper/debug_tools.hpp"


namespace Cascade_Graphics
{
    Renderer::Renderer(std::shared_ptr<Vulkan_Backend::Vulkan_Graphics> vulkan_graphics_ptr, Window_Information window_information) : m_vulkan_graphics_ptr(vulkan_graphics_ptr), m_window_information(window_information)
    {
        std::unique_lock<std::mutex> vulkan_object_access_lock(m_vulkan_graphics_ptr->m_vulkan_objects_access_mutex);
        m_vulkan_graphics_ptr->m_vulkan_object_access_notify.wait(vulkan_object_access_lock, [&] { return m_vulkan_graphics_ptr->Is_Vulkan_Initialized(); });

        LOG_DEBUG << "Graphics: Creating renderer";

        m_camera_ptr = std::make_shared<Camera>(Vector_3<double>(-3.0, 0.0, 0.0), Vector_3<double>(1.0, 0.0, 0.0));
        m_object_manager_ptr = std::make_shared<Object_Manager>();

        m_surface_wrapper_ptr = std::make_shared<Vulkan_Backend::Surface_Wrapper>(m_vulkan_graphics_ptr->m_instance_wrapper_ptr, m_window_information);
        m_swapchain_wrapper_ptr = std::make_shared<Vulkan_Backend::Swapchain_Wrapper>(m_vulkan_graphics_ptr->m_logical_device_wrapper_ptr, m_vulkan_graphics_ptr->m_physical_device_wrapper_ptr, m_surface_wrapper_ptr,
                                                                                      m_vulkan_graphics_ptr->m_queue_manager_ptr, *m_window_information.width_ptr, *m_window_information.height_ptr);

        std::vector<Vulkan_Backend::Storage_Manager::Image_Resource> swapchain_image_resources = m_swapchain_wrapper_ptr->Get_Swapchain_Image_Resources();
        for (uint32_t i = 0; i < swapchain_image_resources.size(); i++)
        {
            m_swapchain_image_identifiers.push_back(m_vulkan_graphics_ptr->m_storage_manager_ptr->Add_Image("swapchain", swapchain_image_resources[i]));
        }

        m_render_target_image_identifier
            = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Image("render_target", swapchain_image_resources.front().image_format, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                                                         m_swapchain_wrapper_ptr->Get_Swapchain_Extent(), Vulkan_Backend::Queue_Manager::COMPUTE_QUEUE | Vulkan_Backend::Queue_Manager::TRANSFER_QUEUE);
        m_camera_data_identifier = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Buffer("camera_data", sizeof(Camera::GPU_Camera_Data), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                                                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                                                               Vulkan_Backend::Queue_Manager::COMPUTE_QUEUE | Vulkan_Backend::Queue_Manager::TRANSFER_QUEUE);
        m_object_buffer_identifier
            = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Buffer("object_buffer", sizeof(Object_Manager::GPU_Object), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, Vulkan_Backend::Queue_Manager::COMPUTE_QUEUE | Vulkan_Backend::Queue_Manager::TRANSFER_QUEUE);
        m_voxel_buffer_identifier
            = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Buffer("voxel_buffer", sizeof(Object_Manager::GPU_Voxel), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, Vulkan_Backend::Queue_Manager::COMPUTE_QUEUE | Vulkan_Backend::Queue_Manager::TRANSFER_QUEUE);
        m_hit_buffer_identifier = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Buffer("hit_buffer", sizeof(uint32_t) * 4, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                                                              Vulkan_Backend::Queue_Manager::COMPUTE_QUEUE);
        m_staging_buffer_identifier
            = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Buffer("staging_buffer", 0, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, Vulkan_Backend::Queue_Manager::TRANSFER_QUEUE);

        m_swapchain_resource_grouping_identifier = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Resource_Grouping("swapchain_resource_grouping", m_swapchain_image_identifiers);
        m_render_compute_resource_grouping_identifier = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Resource_Grouping(
            "render_compute_resource_grouping", {m_render_target_image_identifier, m_camera_data_identifier, m_object_buffer_identifier, m_voxel_buffer_identifier, m_hit_buffer_identifier});
        m_render_compute_descriptor_set_identifier = m_vulkan_graphics_ptr->m_descriptor_set_manager_ptr->Create_Descriptor_Set(m_render_compute_resource_grouping_identifier);

        m_render_shader_identifier = m_vulkan_graphics_ptr->m_shader_manager_ptr->Add_Shader("render_shader", "../lib/Cascade_Graphics/src/Shaders/render.comp");

        m_render_pipeline_identifier = m_vulkan_graphics_ptr->m_pipeline_manager_ptr->Add_Compute_Pipeline("render_pipeline", m_render_compute_descriptor_set_identifier, m_render_shader_identifier);
        Record_Command_Buffers();

        m_image_available_semaphore_identifier = m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Create_Semaphore("image_available_semaphore");
        m_render_finished_semaphore_identifier = m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Create_Semaphore("render_finished_semaphore");
        m_in_flight_fence_identifier = m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Create_Fence("in_flight_fence");

        LOG_DEBUG << "Graphics: Finished creating renderer";

        m_renderer_initialized = true;
        m_vulkan_graphics_ptr->m_vulkan_object_access_notify.notify_all();
    }

    Renderer::~Renderer()
    {
        LOG_DEBUG << "Graphics: Destroying renderer";

        m_swapchain_wrapper_ptr.reset();
        m_surface_wrapper_ptr.reset();

        LOG_DEBUG << "Graphics: Finished destroying renderer";
    }

    void Renderer::Record_Command_Buffers()
    {
        for (uint32_t i = 0; i < m_swapchain_wrapper_ptr->Get_Swapchain_Image_Count(); i++)
        {
            m_command_buffer_identifiers.push_back(m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Add_Command_Buffer("render_frame_command_buffer",
                                                                                                                           m_vulkan_graphics_ptr->m_queue_manager_ptr->Get_Queue_Family_Index(Vulkan_Backend::Queue_Manager::Queue_Types::COMPUTE_QUEUE),
                                                                                                                           {m_swapchain_resource_grouping_identifier, m_render_compute_resource_grouping_identifier}, m_render_pipeline_identifier));

            m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Begin_Recording(m_command_buffer_identifiers[i], (VkCommandBufferUsageFlagBits)0);
            m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Image_Memory_Barrier(m_command_buffer_identifiers[i], m_swapchain_image_identifiers[i], VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                                                      VK_PIPELINE_STAGE_TRANSFER_BIT);
            m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Image_Memory_Barrier(m_command_buffer_identifiers[i], m_render_target_image_identifier, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
            m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Dispatch_Compute_Shader(m_command_buffer_identifiers[i], static_cast<uint32_t>(std::ceil(m_swapchain_wrapper_ptr->Get_Swapchain_Extent().width / 32.0)),
                                                                                         static_cast<uint32_t>(std::ceil(m_swapchain_wrapper_ptr->Get_Swapchain_Extent().height / 32.0)), 1);
            m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Image_Memory_Barrier(m_command_buffer_identifiers[i], m_render_target_image_identifier, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                                                                      VK_PIPELINE_STAGE_TRANSFER_BIT);
            m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Copy_Image(m_command_buffer_identifiers[i], m_render_target_image_identifier, m_swapchain_image_identifiers[i], m_swapchain_wrapper_ptr->Get_Swapchain_Extent().width,
                                                                            m_swapchain_wrapper_ptr->Get_Swapchain_Extent().height);
            m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Image_Memory_Barrier(m_command_buffer_identifiers[i], m_render_target_image_identifier, VK_ACCESS_NONE, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_TRANSFER_BIT);
            m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Image_Memory_Barrier(m_command_buffer_identifiers[i], m_swapchain_image_identifiers[i], VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_TRANSFER_BIT);
            m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->End_Recording(m_command_buffer_identifiers[i]);
        }
    }

    void Renderer::Recreate_Swapchain()
    {
        LOG_DEBUG << "Vulkan: Recreating swapchain";

        VALIDATE_VKRESULT(vkDeviceWaitIdle(*m_vulkan_graphics_ptr->m_logical_device_wrapper_ptr->Get_Device()), "Graphics: Failed to wait for device idle");

        for (uint32_t i = 0; i < m_command_buffer_identifiers.size(); i++)
        {
            m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Remove_Command_Buffer(m_command_buffer_identifiers[i]);
        }
        for (uint32_t i = 0; i < m_swapchain_image_identifiers.size(); i++)
        {
            m_vulkan_graphics_ptr->m_storage_manager_ptr->Destroy_Image(m_swapchain_image_identifiers[i]);
        }
        m_command_buffer_identifiers.clear();
        m_swapchain_image_identifiers.clear();

        m_vulkan_graphics_ptr->m_pipeline_manager_ptr->Delete_Pipeline(m_render_pipeline_identifier);
        m_vulkan_graphics_ptr->m_descriptor_set_manager_ptr->Remove_Descriptor_Set(m_render_compute_descriptor_set_identifier);

        m_vulkan_graphics_ptr->m_storage_manager_ptr->Destroy_Image(m_render_target_image_identifier);
        m_vulkan_graphics_ptr->m_storage_manager_ptr->Destroy_Buffer(m_camera_data_identifier);
        m_vulkan_graphics_ptr->m_storage_manager_ptr->Destroy_Buffer(m_object_buffer_identifier);
        m_vulkan_graphics_ptr->m_storage_manager_ptr->Destroy_Buffer(m_voxel_buffer_identifier);
        m_vulkan_graphics_ptr->m_storage_manager_ptr->Destroy_Buffer(m_hit_buffer_identifier);
        m_vulkan_graphics_ptr->m_storage_manager_ptr->Destroy_Buffer(m_staging_buffer_identifier);
        m_vulkan_graphics_ptr->m_storage_manager_ptr->Remove_Resource_Grouping(m_swapchain_resource_grouping_identifier);
        m_vulkan_graphics_ptr->m_storage_manager_ptr->Remove_Resource_Grouping(m_render_compute_resource_grouping_identifier);

        m_swapchain_wrapper_ptr.reset();

        m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Destroy_Semaphore(m_image_available_semaphore_identifier);
        m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Destroy_Semaphore(m_render_finished_semaphore_identifier);
        m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Destroy_Fence(m_in_flight_fence_identifier);

        uint32_t width = *m_window_information.width_ptr;
        uint32_t height = *m_window_information.height_ptr;
        LOG_DEBUG << "Vulkan: New window size " << width << "x" << height;

        m_swapchain_wrapper_ptr = std::make_shared<Vulkan_Backend::Swapchain_Wrapper>(m_vulkan_graphics_ptr->m_logical_device_wrapper_ptr, m_vulkan_graphics_ptr->m_physical_device_wrapper_ptr, m_surface_wrapper_ptr,
                                                                                      m_vulkan_graphics_ptr->m_queue_manager_ptr, width, height);
        width = m_swapchain_wrapper_ptr->Get_Swapchain_Extent().width;
        height = m_swapchain_wrapper_ptr->Get_Swapchain_Extent().height;

        std::vector<Object_Manager::GPU_Object> gpu_objects = m_object_manager_ptr->Get_GPU_Objects();
        std::vector<Object_Manager::GPU_Voxel> gpu_voxels = m_object_manager_ptr->Get_GPU_Voxels();

        std::vector<Vulkan_Backend::Storage_Manager::Image_Resource> swapchain_image_resources = m_swapchain_wrapper_ptr->Get_Swapchain_Image_Resources();
        for (uint32_t i = 0; i < swapchain_image_resources.size(); i++)
        {
            m_swapchain_image_identifiers.push_back(m_vulkan_graphics_ptr->m_storage_manager_ptr->Add_Image("swapchain", swapchain_image_resources[i]));
        }

        m_render_target_image_identifier
            = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Image("render_target", swapchain_image_resources.front().image_format, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                                                         m_swapchain_wrapper_ptr->Get_Swapchain_Extent(), Vulkan_Backend::Queue_Manager::COMPUTE_QUEUE | Vulkan_Backend::Queue_Manager::TRANSFER_QUEUE);
        m_camera_data_identifier = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Buffer("camera_data", sizeof(Camera::GPU_Camera_Data), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                                                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                                                               Vulkan_Backend::Queue_Manager::COMPUTE_QUEUE | Vulkan_Backend::Queue_Manager::TRANSFER_QUEUE);
        m_object_buffer_identifier
            = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Buffer("object_buffer", sizeof(Object_Manager::GPU_Object) * gpu_objects.size(), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                                          VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, Vulkan_Backend::Queue_Manager::COMPUTE_QUEUE | Vulkan_Backend::Queue_Manager::TRANSFER_QUEUE);
        m_voxel_buffer_identifier
            = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Buffer("voxel_buffer", sizeof(Object_Manager::GPU_Voxel) * gpu_voxels.size(), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, Vulkan_Backend::Queue_Manager::COMPUTE_QUEUE | Vulkan_Backend::Queue_Manager::TRANSFER_QUEUE);
        m_hit_buffer_identifier = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Buffer("hit_buffer", sizeof(uint32_t) * 4 * gpu_voxels.size(), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                                                                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, Vulkan_Backend::Queue_Manager::COMPUTE_QUEUE);
        m_staging_buffer_identifier
            = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Buffer("staging_buffer", 0, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, Vulkan_Backend::Queue_Manager::TRANSFER_QUEUE);

        m_swapchain_resource_grouping_identifier = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Resource_Grouping("swapchain_resource_grouping", m_swapchain_image_identifiers);
        m_render_compute_resource_grouping_identifier = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Resource_Grouping(
            "render_compute_resource_grouping", {m_render_target_image_identifier, m_camera_data_identifier, m_object_buffer_identifier, m_voxel_buffer_identifier, m_hit_buffer_identifier});
        m_render_compute_descriptor_set_identifier = m_vulkan_graphics_ptr->m_descriptor_set_manager_ptr->Create_Descriptor_Set(m_render_compute_resource_grouping_identifier);

        m_render_pipeline_identifier = m_vulkan_graphics_ptr->m_pipeline_manager_ptr->Add_Compute_Pipeline("render_pipeline", m_render_compute_descriptor_set_identifier, m_render_shader_identifier);
        Record_Command_Buffers();

        m_vulkan_graphics_ptr->m_storage_manager_ptr->Upload_To_Buffer_Staging(m_object_buffer_identifier, m_staging_buffer_identifier, gpu_objects.data(), sizeof(Cascade_Graphics::Object_Manager::GPU_Object) * gpu_objects.size(),
                                                                               m_vulkan_graphics_ptr);
        m_vulkan_graphics_ptr->m_storage_manager_ptr->Upload_To_Buffer_Staging(m_voxel_buffer_identifier, m_staging_buffer_identifier, gpu_voxels.data(), sizeof(Cascade_Graphics::Object_Manager::GPU_Voxel) * gpu_voxels.size(), m_vulkan_graphics_ptr);

        m_image_available_semaphore_identifier = m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Create_Semaphore("image_available_semaphore");
        m_render_finished_semaphore_identifier = m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Create_Semaphore("render_finished_semaphore");
        m_in_flight_fence_identifier = m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Create_Fence("in_flight_fence");
    }

    void Renderer::Render_Frame()
    {
        std::unique_lock<std::mutex> vulkan_object_access_lock(m_vulkan_graphics_ptr->m_vulkan_objects_access_mutex);
        m_vulkan_graphics_ptr->m_vulkan_object_access_notify.wait(vulkan_object_access_lock, [&] { return m_rendering_active; });

        vkWaitForFences(*m_vulkan_graphics_ptr->m_logical_device_wrapper_ptr->Get_Device(), 1, m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Get_Fence(m_in_flight_fence_identifier), VK_TRUE, UINT64_MAX);
        vkResetFences(*m_vulkan_graphics_ptr->m_logical_device_wrapper_ptr->Get_Device(), 1, m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Get_Fence(m_in_flight_fence_identifier));

        uint32_t image_index;
        VkResult acquire_next_image_result = vkAcquireNextImageKHR(*m_vulkan_graphics_ptr->m_logical_device_wrapper_ptr->Get_Device(), *m_swapchain_wrapper_ptr->Get_Swapchain(), UINT64_MAX,
                                                                   *m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Get_Semaphore(m_image_available_semaphore_identifier), VK_NULL_HANDLE, &image_index);

        if (acquire_next_image_result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            LOG_DEBUG << "Vulkan: Swapchain out of date";
            Recreate_Swapchain();

            return;
        }
        else
        {
            VALIDATE_VKRESULT(acquire_next_image_result, "Vulkan: Failed to acquire next image");
        }

        Cascade_Graphics::Camera::GPU_Camera_Data camera_data = m_camera_ptr->Get_GPU_Camera_Data(which_hit_buffer);
        m_vulkan_graphics_ptr->m_storage_manager_ptr->Upload_To_Buffer_Direct(m_camera_data_identifier, &camera_data, sizeof(Cascade_Graphics::Camera::GPU_Camera_Data));
        which_hit_buffer = (which_hit_buffer + 1) % 4;

        VkPipelineStageFlags pipeline_wait_stage_mask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pNext = nullptr;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Get_Semaphore(m_image_available_semaphore_identifier);
        submit_info.pWaitDstStageMask = &pipeline_wait_stage_mask;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Get_Command_Buffer(m_command_buffer_identifiers[image_index]);
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Get_Semaphore(m_image_available_semaphore_identifier);

        VALIDATE_VKRESULT(vkQueueSubmit(*m_vulkan_graphics_ptr->m_queue_manager_ptr->Get_Queue(Vulkan_Backend::Queue_Manager::Queue_Types::COMPUTE_QUEUE), 1, &submit_info,
                                        *m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Get_Fence(m_in_flight_fence_identifier)),
                          "Vulkan: Failed to submit queue");

        VkPresentInfoKHR present_info = {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.pNext = nullptr;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Get_Semaphore(m_image_available_semaphore_identifier);
        present_info.swapchainCount = 1;
        present_info.pSwapchains = m_swapchain_wrapper_ptr->Get_Swapchain();
        present_info.pImageIndices = &image_index;
        present_info.pResults = nullptr;

        VkResult queue_present_result = vkQueuePresentKHR(*m_vulkan_graphics_ptr->m_queue_manager_ptr->Get_Queue(Vulkan_Backend::Queue_Manager::Queue_Types::PRESENT_QUEUE), &present_info);

        if (queue_present_result == VK_ERROR_OUT_OF_DATE_KHR || queue_present_result == VK_SUBOPTIMAL_KHR)
        {
            LOG_DEBUG << "Vulkan: Swapchain out of date or suboptimal";
            Recreate_Swapchain();

            return;
        }
        else
        {
            VALIDATE_VKRESULT(acquire_next_image_result, "Vulkan: Failed to present swapchain image");
        }

#ifdef CSD_LOG_FPS
        std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
        LOG_TRACE << "Graphics: FPS    " << 1000000000.0 / (now - m_previous_present).count();
        m_previous_present = now;
#endif
    }

    void Renderer::Update_Objects()
    {
        std::unique_lock<std::mutex> vulkan_object_access_lock(m_vulkan_graphics_ptr->m_vulkan_objects_access_mutex);
        m_vulkan_graphics_ptr->m_vulkan_object_access_notify.wait(vulkan_object_access_lock, [&] { return m_renderer_initialized; });

        VALIDATE_VKRESULT(vkDeviceWaitIdle(*m_vulkan_graphics_ptr->m_logical_device_wrapper_ptr->Get_Device()), "Graphics: Failed to wait for device idle");

        std::vector<Object_Manager::GPU_Object> gpu_objects = m_object_manager_ptr->Get_GPU_Objects();

        if (m_vulkan_graphics_ptr->m_storage_manager_ptr->Get_Buffer_Resource(m_object_buffer_identifier)->buffer_size < sizeof(Object_Manager::GPU_Object) * gpu_objects.size())
        {
            LOG_DEBUG << "Graphics: Increasing object buffer size";

            m_vulkan_graphics_ptr->m_storage_manager_ptr->Resize_Buffer(m_object_buffer_identifier, sizeof(Cascade_Graphics::Object_Manager::GPU_Object) * gpu_objects.size());

            for (uint32_t i = 0; i < m_command_buffer_identifiers.size(); i++)
            {
                m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Remove_Command_Buffer(m_command_buffer_identifiers[i]);
            }
            m_command_buffer_identifiers.clear();
            m_vulkan_graphics_ptr->m_pipeline_manager_ptr->Delete_Pipeline(m_render_pipeline_identifier);
            m_vulkan_graphics_ptr->m_descriptor_set_manager_ptr->Remove_Descriptor_Set(m_render_compute_descriptor_set_identifier);

            m_render_compute_descriptor_set_identifier = m_vulkan_graphics_ptr->m_descriptor_set_manager_ptr->Create_Descriptor_Set(m_render_compute_resource_grouping_identifier);
            m_render_pipeline_identifier = m_vulkan_graphics_ptr->m_pipeline_manager_ptr->Add_Compute_Pipeline("render_pipeline", m_render_compute_descriptor_set_identifier, m_render_shader_identifier);
            Record_Command_Buffers();
        }

        m_vulkan_graphics_ptr->m_storage_manager_ptr->Upload_To_Buffer_Staging(m_object_buffer_identifier, m_staging_buffer_identifier, gpu_objects.data(), sizeof(Cascade_Graphics::Object_Manager::GPU_Object) * gpu_objects.size(),
                                                                               m_vulkan_graphics_ptr);
    }

    void Renderer::Update_Voxels()
    {
        Update_Objects();

        std::unique_lock<std::mutex> vulkan_object_access_lock(m_vulkan_graphics_ptr->m_vulkan_objects_access_mutex);
        m_vulkan_graphics_ptr->m_vulkan_object_access_notify.wait(vulkan_object_access_lock, [&] { return m_renderer_initialized; });

        VALIDATE_VKRESULT(vkDeviceWaitIdle(*m_vulkan_graphics_ptr->m_logical_device_wrapper_ptr->Get_Device()), "Graphics: Failed to wait for device idle");

        std::vector<Object_Manager::GPU_Voxel> gpu_voxels = m_object_manager_ptr->Get_GPU_Voxels();

        if (m_vulkan_graphics_ptr->m_storage_manager_ptr->Get_Buffer_Resource(m_voxel_buffer_identifier)->buffer_size < sizeof(Object_Manager::GPU_Voxel) * gpu_voxels.size())
        {
            LOG_DEBUG << "Graphics: Increasing voxel buffer size";

            m_vulkan_graphics_ptr->m_storage_manager_ptr->Resize_Buffer(m_voxel_buffer_identifier, sizeof(Cascade_Graphics::Object_Manager::GPU_Voxel) * gpu_voxels.size());
            m_vulkan_graphics_ptr->m_storage_manager_ptr->Resize_Buffer(m_hit_buffer_identifier, sizeof(uint32_t) * 4 * gpu_voxels.size());

            for (uint32_t i = 0; i < m_command_buffer_identifiers.size(); i++)
            {
                m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Remove_Command_Buffer(m_command_buffer_identifiers[i]);
            }
            m_command_buffer_identifiers.clear();
            m_vulkan_graphics_ptr->m_pipeline_manager_ptr->Delete_Pipeline(m_render_pipeline_identifier);
            m_vulkan_graphics_ptr->m_descriptor_set_manager_ptr->Remove_Descriptor_Set(m_render_compute_descriptor_set_identifier);

            m_render_compute_descriptor_set_identifier = m_vulkan_graphics_ptr->m_descriptor_set_manager_ptr->Create_Descriptor_Set(m_render_compute_resource_grouping_identifier);
            m_render_pipeline_identifier = m_vulkan_graphics_ptr->m_pipeline_manager_ptr->Add_Compute_Pipeline("render_pipeline", m_render_compute_descriptor_set_identifier, m_render_shader_identifier);
            Record_Command_Buffers();
        }

        m_vulkan_graphics_ptr->m_storage_manager_ptr->Upload_To_Buffer_Staging(m_voxel_buffer_identifier, m_staging_buffer_identifier, gpu_voxels.data(), sizeof(Cascade_Graphics::Object_Manager::GPU_Voxel) * gpu_voxels.size(), m_vulkan_graphics_ptr);
    }

    void Renderer::Start_Rendering()
    {
        std::unique_lock<std::mutex> vulkan_object_access_lock(m_vulkan_graphics_ptr->m_vulkan_objects_access_mutex);
        m_vulkan_graphics_ptr->m_vulkan_object_access_notify.wait(vulkan_object_access_lock, [&] { return m_renderer_initialized; });

        m_rendering_active = true;
        m_vulkan_graphics_ptr->m_vulkan_object_access_notify.notify_all();
    }
} // namespace Cascade_Graphics