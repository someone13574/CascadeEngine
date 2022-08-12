#pragma once

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
        Window_Information m_window_information;

        std::string m_per_frame_descriptors_resource_grouping_label;
        std::string m_swapchain_resource_grouping_label;
        Vulkan_Backend::Resource_ID m_render_target_resource_id;
        Vulkan_Backend::Resource_ID m_camera_buffer_resource_id;
        Vulkan_Backend::Resource_ID m_voxel_buffer_resource_id;
        Vulkan_Backend::Resource_ID m_hit_buffer_resource_id;
        Vulkan_Backend::Resource_ID m_staging_buffer_resource_id;
        std::vector<Vulkan_Backend::Resource_ID> m_swapchain_resource_ids;

        std::string m_descriptor_set_label;
        std::string m_pipeline_label;
        std::vector<Vulkan_Backend::Command_Buffer_Manager::Identifier> m_command_buffer_identifiers;

        std::shared_ptr<Vulkan_Backend::Surface_Wrapper> m_surface_wrapper_ptr;
        std::shared_ptr<Vulkan_Backend::Swapchain_Wrapper> m_swapchain_wrapper_ptr;
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