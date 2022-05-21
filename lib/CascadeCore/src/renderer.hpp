#pragma once

#if defined __linux__

#include <xcb/xcb.h>

#elif defined _WIN32 || defined WIN32

#include <windows.h>

#endif

#include "cascade_graphics.hpp"

namespace CGV = CascadeGraphics::Vulkan;

namespace CascadeCore
{
    class Renderer
    {
    private:
        unsigned int m_width;
        unsigned int m_height;

        unsigned int m_current_frame = 0;

        bool m_initialized = false;

    private:
        std::shared_ptr<CGV::Instance> instance_ptr;
        std::shared_ptr<CGV::Surface> surface_ptr;
        std::shared_ptr<CascadeGraphicsDebugging::Vulkan::Validation_Layer> validation_layer_ptr;
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

    public:
        Renderer(CascadeGraphics::Vulkan::Surface::Window_Data window_data, unsigned int width, unsigned int height);
        ~Renderer();

        void Render_Frame();
    };
} // namespace CascadeCore