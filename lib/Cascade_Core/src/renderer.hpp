#pragma once

#include "window.hpp"

#if defined __linux__

#include <xcb/xcb.h>

#elif defined _WIN32 || defined WIN32

#include <windows.h>

#endif

#include "cascade_graphics.hpp"

namespace CGV = Cascade_Graphics::Vulkan;

namespace Cascade_Core
{
    class Window;

    class Renderer
    {
    private:
        unsigned int m_width;
        unsigned int m_height;

        Window* m_window_ptr;

        unsigned int m_current_frame = 0;

        bool m_initialized = false;

    private:
        std::shared_ptr<Cascade_Graphics::Camera> m_camera_ptr;
        std::shared_ptr<Cascade_Graphics::Object_Manager> m_object_manager_ptr;

        std::shared_ptr<CGV::Instance> instance_ptr;
        std::shared_ptr<CGV::Surface> surface_ptr;
        std::shared_ptr<Cascade_Graphics_Debugging::Vulkan::Validation_Layer> validation_layer_ptr;
        std::shared_ptr<CGV::Queue_Manager> queue_manager_ptr;
        std::shared_ptr<CGV::Physical_Device> physical_device_ptr;
        std::shared_ptr<CGV::Device> logical_device_ptr;
        std::shared_ptr<CGV::Swapchain> swapchain_ptr;
        std::shared_ptr<CGV::Storage_Manager> storage_manager_ptr;
        std::shared_ptr<CGV::Shader_Manager> shader_manager_ptr;
        std::shared_ptr<CGV::Resource_Grouping_Manager> resource_grouping_manager;
        std::shared_ptr<CGV::Pipeline_Manager> pipeline_manager_ptr;
        std::shared_ptr<CGV::Command_Buffer_Manager> command_buffer_manager_ptr;
        std::shared_ptr<CGV::Synchronization_Manager> synchronization_manager_ptr;

    private:
        void Recreate_Swapchain();

    public:
        Renderer(Window* window_ptr, Cascade_Graphics::Vulkan::Surface::Window_Data window_data, unsigned int width, unsigned int height);
        ~Renderer();

    public:
        void Render_Frame();

        std::shared_ptr<Cascade_Graphics::Camera> Get_Camera();
        std::shared_ptr<Cascade_Graphics::Object_Manager> Get_Object_Manager();
    };
} // namespace Cascade_Core