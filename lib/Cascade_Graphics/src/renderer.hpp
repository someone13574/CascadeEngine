#pragma once

#include "Vulkan_Wrapper/identifier.hpp"
#include "Vulkan_Wrapper/surface_wrapper.hpp"
#include "Vulkan_Wrapper/swapchain_wrapper.hpp"
#include "Vulkan_Wrapper/vulkan_graphics.hpp"
#include "camera.hpp"
#include "object_manager.hpp"
#include "window_information.hpp"
#include <memory>
#include <vector>


namespace Cascade_Graphics
{
    class Renderer
    {
    private:
        uint32_t which_hit_buffer = 0;

        bool m_renderer_initialized = false;
        bool m_rendering_active = false;

        Vulkan_Backend::Identifier m_image_available_semaphore_identifier;
        Vulkan_Backend::Identifier m_render_finished_semaphore_identifier;
        Vulkan_Backend::Identifier m_in_flight_fence_identifier;

        std::shared_ptr<Vulkan_Backend::Surface_Wrapper> m_surface_wrapper_ptr;
        std::shared_ptr<Vulkan_Backend::Swapchain_Wrapper> m_swapchain_wrapper_ptr;

        Vulkan_Backend::Identifier m_render_shader_identifier;
        Vulkan_Backend::Identifier m_render_pipeline_identifier;
        std::vector<Vulkan_Backend::Identifier> m_command_buffer_identifiers;

        Vulkan_Backend::Identifier m_swapchain_resource_grouping_identifier;
        Vulkan_Backend::Identifier m_render_compute_resource_grouping_identifier;
        Vulkan_Backend::Identifier m_render_compute_descriptor_set_identifier;

        Vulkan_Backend::Identifier m_render_target_image_identifier;
        Vulkan_Backend::Identifier m_camera_data_identifier;
        Vulkan_Backend::Identifier m_voxel_buffer_identifier;
        Vulkan_Backend::Identifier m_hit_buffer_identifier;
        Vulkan_Backend::Identifier m_staging_buffer_identifier;
        std::vector<Vulkan_Backend::Identifier> m_swapchain_image_identifiers;

        Window_Information m_window_information;
        std::shared_ptr<Vulkan_Backend::Vulkan_Graphics> m_vulkan_graphics_ptr;

    private:
        void Record_Command_Buffers();
        void Recreate_Swapchain();

    public:
        Renderer(std::shared_ptr<Vulkan_Backend::Vulkan_Graphics> vulkan_graphics_ptr, Window_Information window_information);
        ~Renderer();

    public:
        std::shared_ptr<Camera> m_camera_ptr;
        std::shared_ptr<Object_Manager> m_object_manager_ptr;

    public:
        void Render_Frame();
        void Update_Voxels();
        void Start_Rendering();
    };
} // namespace Cascade_Graphics