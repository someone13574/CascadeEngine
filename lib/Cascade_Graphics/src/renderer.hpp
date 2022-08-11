#pragma once

#include "Vulkan_Wrapper/vulkan_graphics.hpp"
#include "camera.hpp"
#include "object_manager.hpp"
#include "window_information.hpp"
#include <memory>


namespace Cascade_Graphics
{
    class Renderer
    {
    private:
        uint32_t which_hit_buffer = 0;

        bool m_renderer_initialized = false;
        bool m_rendering_active = false;
        Window_Information m_window_information;

        std::shared_ptr<Vulkan_Backend::Surface_Wrapper> m_surface_wrapper_ptr;
        std::shared_ptr<Vulkan_Backend::Swapchain_Wrapper> m_swapchain_wrapper_ptr;
        std::shared_ptr<Vulkan_Backend::Vulkan_Graphics> m_vulkan_graphics_ptr;

    private:
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