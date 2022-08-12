#include "renderer.hpp"

#include "cascade_logging.hpp"


namespace Cascade_Graphics
{
    Renderer::Renderer(std::shared_ptr<Vulkan_Backend::Vulkan_Graphics> vulkan_graphics_ptr, Window_Information window_information) : m_vulkan_graphics_ptr(vulkan_graphics_ptr), m_window_information(window_information)
    {
        std::unique_lock<std::mutex> vulkan_object_access_lock(vulkan_graphics_ptr->m_vulkan_objects_access_mutex);
        vulkan_graphics_ptr->m_vulkan_object_access_notify.wait(vulkan_object_access_lock, [vulkan_graphics_ptr] { return vulkan_graphics_ptr->Is_Vulkan_Initialized(); });

        LOG_DEBUG << "Graphics: Creating renderer";

        m_camera_ptr = std::make_shared<Camera>(Vector_3<double>(-3.0, 0.0, 0.0), Vector_3<double>(1.0, 0.0, 0.0));
        m_object_manager_ptr = std::make_shared<Object_Manager>();

        m_surface_wrapper_ptr = std::make_shared<Vulkan_Backend::Surface_Wrapper>(m_vulkan_graphics_ptr->m_instance_wrapper_ptr, m_window_information);
        m_swapchain_wrapper_ptr = std::make_shared<Vulkan_Backend::Swapchain_Wrapper>(m_vulkan_graphics_ptr->m_logical_device_wrapper_ptr, m_vulkan_graphics_ptr->m_physical_device_wrapper_ptr, m_surface_wrapper_ptr,
                                                                                      m_vulkan_graphics_ptr->m_queue_manager_ptr, *window_information.width_ptr, *window_information.height_ptr);


        std::vector<Vulkan_Backend::Storage_Manager::Image_Resource> swapchain_image_resources = m_swapchain_wrapper_ptr->Get_Swapchain_Image_Resources();
        for (uint32_t i = 0; i < swapchain_image_resources.size(); i++)
        {
            m_swapchain_resource_ids.push_back(m_vulkan_graphics_ptr->m_storage_manager_ptr->Add_Image("swapchain", swapchain_image_resources[i]));
        }

        m_render_target_resource_id = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Image(
            "render_target", m_vulkan_graphics_ptr->m_storage_manager_ptr->Get_Image_Resource({"swapchain", 0, Vulkan_Backend::Resource_ID::IMAGE_RESOURCE})->image_format, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
            VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, m_vulkan_graphics_ptr->m_storage_manager_ptr->Get_Image_Resource({"swapchain", 0, Vulkan_Backend::Resource_ID::IMAGE_RESOURCE})->image_size,
            Vulkan_Backend::Queue_Manager::COMPUTE_QUEUE | Vulkan_Backend::Queue_Manager::TRANSFER_QUEUE);
        m_camera_buffer_resource_id = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Buffer("camera_data", sizeof(Camera::GPU_Camera_Data), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                                                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                                                                  Vulkan_Backend::Queue_Manager::COMPUTE_QUEUE | Vulkan_Backend::Queue_Manager::TRANSFER_QUEUE);
        m_voxel_buffer_resource_id
            = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Buffer("voxel_buffer", sizeof(Cascade_Graphics::Object_Manager::GPU_Voxel) * 1, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                                          VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, Vulkan_Backend::Queue_Manager::COMPUTE_QUEUE | Vulkan_Backend::Queue_Manager::TRANSFER_QUEUE);
        m_hit_buffer_resource_id = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Buffer("hit_buffer", sizeof(uint32_t) * 4, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                                                               Vulkan_Backend::Queue_Manager::COMPUTE_QUEUE);
        m_staging_buffer_resource_id
            = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Buffer("staging_buffer", 0, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, Vulkan_Backend::Queue_Manager::TRANSFER_QUEUE);

#ifdef __linux__
        m_vulkan_graphics_ptr->m_shader_manager_ptr->Add_Shader("render_shader", "/home/owen/Documents/Code/C++/CascadeEngine/build/build/build/Cascade_Graphics/src/Shaders/render.comp.spv");
#elif defined __WIN32 || defined WIN32
        m_vulkan_graphics_ptr->m_shader_manager_ptr->Add_Shader("render_shader", "C:/Users/owenl/Documents/Code/C++/CascadeEngine/build/src/Shaders/render.comp.spv");
#endif

        m_per_frame_descriptors_resource_grouping_label
            = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Resource_Grouping("per_frame_descriptors", {m_render_target_resource_id, m_camera_buffer_resource_id, m_voxel_buffer_resource_id, m_hit_buffer_resource_id});
        m_swapchain_resource_grouping_label = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Resource_Grouping(
            "swapchain_images", {{"swapchain", 0, Vulkan_Backend::Resource_ID::IMAGE_RESOURCE}, {"swapchain", 1, Vulkan_Backend::Resource_ID::IMAGE_RESOURCE}, {"swapchain", 2, Vulkan_Backend::Resource_ID::IMAGE_RESOURCE}});

        m_descriptor_set_label = m_vulkan_graphics_ptr->m_descriptor_set_manager_ptr->Create_Descriptor_Set("per_frame_descriptors");
        m_pipeline_label = m_vulkan_graphics_ptr->m_pipeline_manager_ptr->Add_Compute_Pipeline("rendering_pipeline", "per_frame_descriptors", "render_shader");

        Record_Command_Buffers();

        m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Create_Semaphore("image_available_semaphore");
        m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Create_Semaphore("render_finished_semaphore");
        m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Create_Fence("in_flight_fence");

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
        m_command_buffer_identifiers.clear();

        for (uint32_t i = 0; i < m_swapchain_wrapper_ptr->Get_Swapchain_Image_Count(); i++)
        {
            m_command_buffer_identifiers.push_back(m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Add_Command_Buffer("render_frame",
                                                                                                                           m_vulkan_graphics_ptr->m_queue_manager_ptr->Get_Queue_Family_Index(Vulkan_Backend::Queue_Manager::Queue_Types::COMPUTE_QUEUE),
                                                                                                                           {m_per_frame_descriptors_resource_grouping_label, m_swapchain_resource_grouping_label}, m_pipeline_label));

            m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Begin_Recording(m_command_buffer_identifiers.back(), (VkCommandBufferUsageFlagBits)0);
            m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Image_Memory_Barrier(m_command_buffer_identifiers.back(), m_swapchain_resource_ids[i], VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                                                      VK_PIPELINE_STAGE_TRANSFER_BIT);
            m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Image_Memory_Barrier(m_command_buffer_identifiers.back(), m_render_target_resource_id, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
            m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Dispatch_Compute_Shader(m_command_buffer_identifiers.back(), static_cast<uint32_t>(std::ceil(m_swapchain_wrapper_ptr->Get_Swapchain_Extent().width / 32.0)),
                                                                                         static_cast<uint32_t>(std::ceil(m_swapchain_wrapper_ptr->Get_Swapchain_Extent().height / 32.0)), 1);
            m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Image_Memory_Barrier(m_command_buffer_identifiers.back(), m_render_target_resource_id, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                                                                      VK_PIPELINE_STAGE_TRANSFER_BIT);
            m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Copy_Image(m_command_buffer_identifiers.back(), m_render_target_resource_id, m_swapchain_resource_ids[i], m_swapchain_wrapper_ptr->Get_Swapchain_Extent().width,
                                                                            m_swapchain_wrapper_ptr->Get_Swapchain_Extent().height);
            m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Image_Memory_Barrier(m_command_buffer_identifiers.back(), m_render_target_resource_id, VK_ACCESS_NONE, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_TRANSFER_BIT);
            m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Image_Memory_Barrier(m_command_buffer_identifiers.back(), m_swapchain_resource_ids[i], VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_TRANSFER_BIT);
            m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->End_Recording(m_command_buffer_identifiers.back());
        }
    }

    void Renderer::Recreate_Swapchain()
    {
        LOG_DEBUG << "Vulkan: Recreating swapchain";

        vkDeviceWaitIdle(*m_vulkan_graphics_ptr->m_logical_device_wrapper_ptr->Get_Device());

        for (uint32_t i = 0; i < m_command_buffer_identifiers.size(); i++)
        {
            m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Remove_Command_Buffer(m_command_buffer_identifiers[i]);
        }
        m_command_buffer_identifiers.clear();

        m_vulkan_graphics_ptr->m_pipeline_manager_ptr->Delete_Pipeline(m_pipeline_label);
        m_vulkan_graphics_ptr->m_descriptor_set_manager_ptr->Remove_Descriptor_Set(m_descriptor_set_label);

        for (uint32_t i = 0; i < m_swapchain_resource_ids.size(); i++)
        {
            m_vulkan_graphics_ptr->m_storage_manager_ptr->Destroy_Image(m_swapchain_resource_ids[i]);
        }
        m_swapchain_resource_ids.clear();
        m_vulkan_graphics_ptr->m_storage_manager_ptr->Destroy_Image(m_render_target_resource_id);
        m_vulkan_graphics_ptr->m_storage_manager_ptr->Destroy_Buffer(m_camera_buffer_resource_id);
        m_vulkan_graphics_ptr->m_storage_manager_ptr->Destroy_Buffer(m_voxel_buffer_resource_id);
        m_vulkan_graphics_ptr->m_storage_manager_ptr->Destroy_Buffer(m_hit_buffer_resource_id);
        m_vulkan_graphics_ptr->m_storage_manager_ptr->Destroy_Buffer(m_staging_buffer_resource_id);
        m_vulkan_graphics_ptr->m_storage_manager_ptr->Remove_Resource_Grouping(m_swapchain_resource_grouping_label);
        m_vulkan_graphics_ptr->m_storage_manager_ptr->Remove_Resource_Grouping(m_per_frame_descriptors_resource_grouping_label);


        m_swapchain_wrapper_ptr.reset();
        m_vulkan_graphics_ptr->m_synchronization_manager_ptr.reset();

        uint32_t width = *m_window_information.width_ptr;
        uint32_t height = *m_window_information.height_ptr;
        LOG_DEBUG << "Vulkan: New window size " << width << "x" << height;

        m_swapchain_wrapper_ptr = std::make_shared<Vulkan_Backend::Swapchain_Wrapper>(m_vulkan_graphics_ptr->m_logical_device_wrapper_ptr, m_vulkan_graphics_ptr->m_physical_device_wrapper_ptr, m_surface_wrapper_ptr,
                                                                                      m_vulkan_graphics_ptr->m_queue_manager_ptr, width, height);
        width = m_swapchain_wrapper_ptr->Get_Swapchain_Extent().width;
        height = m_swapchain_wrapper_ptr->Get_Swapchain_Extent().height;

        std::vector<Object_Manager::GPU_Voxel> gpu_voxels = m_object_manager_ptr->Get_GPU_Voxels();

        std::vector<Vulkan_Backend::Storage_Manager::Image_Resource> swapchain_image_resources = m_swapchain_wrapper_ptr->Get_Swapchain_Image_Resources();
        for (uint32_t i = 0; i < swapchain_image_resources.size(); i++)
        {
            m_swapchain_resource_ids.push_back(m_vulkan_graphics_ptr->m_storage_manager_ptr->Add_Image("swapchain", swapchain_image_resources[i]));
        }

        m_render_target_resource_id = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Image(
            "render_target", m_vulkan_graphics_ptr->m_storage_manager_ptr->Get_Image_Resource({"swapchain", 0, Vulkan_Backend::Resource_ID::IMAGE_RESOURCE})->image_format, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
            VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, {width, height}, Vulkan_Backend::Queue_Manager::COMPUTE_QUEUE | Vulkan_Backend::Queue_Manager::TRANSFER_QUEUE);
        m_camera_buffer_resource_id = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Buffer("camera_data", sizeof(Cascade_Graphics::Camera::GPU_Camera_Data), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                                                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                                                                  Vulkan_Backend::Queue_Manager::COMPUTE_QUEUE | Vulkan_Backend::Queue_Manager::TRANSFER_QUEUE);
        m_voxel_buffer_resource_id
            = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Buffer("voxel_buffer", sizeof(Cascade_Graphics::Object_Manager::GPU_Voxel) * gpu_voxels.size(), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                                          VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, Vulkan_Backend::Queue_Manager::COMPUTE_QUEUE | Vulkan_Backend::Queue_Manager::TRANSFER_QUEUE);
        m_hit_buffer_resource_id = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Buffer("hit_buffer", sizeof(uint32_t) * 4 * gpu_voxels.size(), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                                                                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, Vulkan_Backend::Queue_Manager::COMPUTE_QUEUE);
        m_staging_buffer_resource_id
            = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Buffer("staging_buffer", 0, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, Vulkan_Backend::Queue_Manager::TRANSFER_QUEUE);

        m_per_frame_descriptors_resource_grouping_label
            = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Resource_Grouping("per_frame_descriptors", {m_render_target_resource_id, m_camera_buffer_resource_id, m_voxel_buffer_resource_id, m_hit_buffer_resource_id});

        m_swapchain_resource_grouping_label = m_vulkan_graphics_ptr->m_storage_manager_ptr->Create_Resource_Grouping("swapchain_images", m_swapchain_resource_ids);

        m_descriptor_set_label = m_vulkan_graphics_ptr->m_descriptor_set_manager_ptr->Create_Descriptor_Set("per_frame_descriptors");

        m_pipeline_label = m_vulkan_graphics_ptr->m_pipeline_manager_ptr->Add_Compute_Pipeline("rendering_pipeline", "per_frame_descriptors", "render_shader");

        Record_Command_Buffers();

        m_vulkan_graphics_ptr->m_storage_manager_ptr->Upload_To_Buffer_Staging(m_voxel_buffer_resource_id, m_staging_buffer_resource_id, gpu_voxels.data(), sizeof(Cascade_Graphics::Object_Manager::GPU_Voxel) * gpu_voxels.size(),
                                                                               m_vulkan_graphics_ptr->m_command_buffer_manager_ptr, m_vulkan_graphics_ptr->m_descriptor_set_manager_ptr);

        m_vulkan_graphics_ptr->m_synchronization_manager_ptr = std::make_shared<Vulkan_Backend::Synchronization_Manager>(m_vulkan_graphics_ptr->m_logical_device_wrapper_ptr);
        m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Create_Semaphore("image_available_semaphore");
        m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Create_Semaphore("render_finished_semaphore");
        m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Create_Fence("in_flight_fence");
    }

    void Renderer::Render_Frame()
    {
        std::unique_lock<std::mutex> vulkan_object_access_lock(m_vulkan_graphics_ptr->m_vulkan_objects_access_mutex);
        m_vulkan_graphics_ptr->m_vulkan_object_access_notify.wait(vulkan_object_access_lock, [&] { return m_rendering_active; });

        vkWaitForFences(*m_vulkan_graphics_ptr->m_logical_device_wrapper_ptr->Get_Device(), 1, m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Get_Fence({"in_flight_fence", 0}), VK_TRUE, UINT64_MAX);
        vkResetFences(*m_vulkan_graphics_ptr->m_logical_device_wrapper_ptr->Get_Device(), 1, m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Get_Fence({"in_flight_fence", 0}));

        uint32_t image_index;
        VkResult acquire_next_image_result = vkAcquireNextImageKHR(*m_vulkan_graphics_ptr->m_logical_device_wrapper_ptr->Get_Device(), *m_swapchain_wrapper_ptr->Get_Swapchain(), UINT64_MAX,
                                                                   *m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Get_Semaphore({"image_available_semaphore", 0}), VK_NULL_HANDLE, &image_index);

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
        m_vulkan_graphics_ptr->m_storage_manager_ptr->Upload_To_Buffer_Direct(m_camera_buffer_resource_id, &camera_data, sizeof(Cascade_Graphics::Camera::GPU_Camera_Data));
        which_hit_buffer = (which_hit_buffer + 1) % 4;

        VkPipelineStageFlags pipeline_wait_stage_mask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pNext = nullptr;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Get_Semaphore({"image_available_semaphore", 0});
        submit_info.pWaitDstStageMask = &pipeline_wait_stage_mask;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Get_Command_Buffer(m_command_buffer_identifiers[image_index]);
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Get_Semaphore({"render_finished_semaphore", 0});

        VALIDATE_VKRESULT(
            vkQueueSubmit(*m_vulkan_graphics_ptr->m_queue_manager_ptr->Get_Queue(Vulkan_Backend::Queue_Manager::Queue_Types::COMPUTE_QUEUE), 1, &submit_info, *m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Get_Fence({"in_flight_fence", 0})),
            "Vulkan: Failed to submit queue");

        VkPresentInfoKHR present_info = {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.pNext = nullptr;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = m_vulkan_graphics_ptr->m_synchronization_manager_ptr->Get_Semaphore({"render_finished_semaphore", 0});
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
    }

    void Renderer::Update_Voxels()
    {
        std::unique_lock<std::mutex> vulkan_object_access_lock(m_vulkan_graphics_ptr->m_vulkan_objects_access_mutex);
        m_vulkan_graphics_ptr->m_vulkan_object_access_notify.wait(vulkan_object_access_lock, [&] { return m_renderer_initialized; });

        m_rendering_active = false;

        vkDeviceWaitIdle(*m_vulkan_graphics_ptr->m_logical_device_wrapper_ptr->Get_Device());

        std::vector<Object_Manager::GPU_Voxel> gpu_voxels = m_object_manager_ptr->Get_GPU_Voxels();

        if (m_vulkan_graphics_ptr->m_storage_manager_ptr->Get_Buffer_Resource(m_voxel_buffer_resource_id)->buffer_size < sizeof(Object_Manager::GPU_Voxel) * gpu_voxels.size())
        {
            m_vulkan_graphics_ptr->m_storage_manager_ptr->Resize_Buffer(m_voxel_buffer_resource_id, sizeof(Cascade_Graphics::Object_Manager::GPU_Voxel) * gpu_voxels.size());
            m_vulkan_graphics_ptr->m_storage_manager_ptr->Resize_Buffer(m_hit_buffer_resource_id, sizeof(uint32_t) * 4 * gpu_voxels.size());

            for (uint32_t i = 0; i < m_command_buffer_identifiers.size(); i++)
            {
                m_vulkan_graphics_ptr->m_command_buffer_manager_ptr->Remove_Command_Buffer(m_command_buffer_identifiers[i]);
            }
            m_vulkan_graphics_ptr->m_pipeline_manager_ptr->Delete_Pipeline(m_pipeline_label);
            m_vulkan_graphics_ptr->m_descriptor_set_manager_ptr->Remove_Descriptor_Set(m_descriptor_set_label);

            m_vulkan_graphics_ptr->m_descriptor_set_manager_ptr->Create_Descriptor_Set("per_frame_descriptors");
            m_vulkan_graphics_ptr->m_pipeline_manager_ptr->Add_Compute_Pipeline("rendering_pipeline", "per_frame_descriptors", "render_shader");

            Record_Command_Buffers();
        }

        m_vulkan_graphics_ptr->m_storage_manager_ptr->Upload_To_Buffer_Staging(m_voxel_buffer_resource_id, m_staging_buffer_resource_id, gpu_voxels.data(), sizeof(Cascade_Graphics::Object_Manager::GPU_Voxel) * gpu_voxels.size(),
                                                                               m_vulkan_graphics_ptr->m_command_buffer_manager_ptr, m_vulkan_graphics_ptr->m_descriptor_set_manager_ptr);

        m_rendering_active = true;
    }

    void Renderer::Start_Rendering()
    {
        m_rendering_active = true;

        m_vulkan_graphics_ptr->m_vulkan_object_access_notify.notify_all();
    }
} // namespace Cascade_Graphics