#pragma once


#include "camera.hpp"
#include "object_manager.hpp"
#include "window_information.hpp"

#include <chrono>
#include <memory>

#ifdef CSD_USE_VULKAN
#include "Vulkan_Wrapper/command_buffer_manager.hpp"
#include "Vulkan_Wrapper/debug_tools.hpp"
#include "Vulkan_Wrapper/descriptor_set_manager.hpp"
#include "Vulkan_Wrapper/device_wrapper.hpp"
#include "Vulkan_Wrapper/instance_wrapper.hpp"
#include "Vulkan_Wrapper/physical_device_wrapper.hpp"
#include "Vulkan_Wrapper/pipeline_manager.hpp"
#include "Vulkan_Wrapper/queue_manager.hpp"
#include "Vulkan_Wrapper/shader_manager.hpp"
#include "Vulkan_Wrapper/storage_manager.hpp"
#include "Vulkan_Wrapper/surface_wrapper.hpp"
#include "Vulkan_Wrapper/swapchain_wrapper.hpp"
#include "Vulkan_Wrapper/synchronization_manager.hpp"
#include "Vulkan_Wrapper/validation_layer_wrapper.hpp"
#endif

namespace Cascade_Graphics
{
    class Renderer
    {
    private:
        bool m_renderer_initialized = false;
        bool m_rendering_active = false;

        std::shared_ptr<Camera> m_camera_ptr;
        std::shared_ptr<Object_Manager> m_object_manager_ptr;
        Window_Information m_window_information;

#ifdef CSD_LOG_FPS
        std::chrono::time_point<std::chrono::high_resolution_clock> m_previous_present;
#endif

    private:
#ifdef CSD_USE_VULKAN
        unsigned int m_current_frame = 0;

        std::shared_ptr<Vulkan::Instance> m_instance_ptr;
        std::shared_ptr<Vulkan::Surface> m_surface_ptr;
        std::shared_ptr<Vulkan::Queue_Manager> m_queue_manager_ptr;
        std::shared_ptr<Vulkan::Physical_Device> m_physical_device_ptr;
        std::shared_ptr<Vulkan::Device> m_logical_device_ptr;
        std::shared_ptr<Vulkan::Swapchain> m_swapchain_ptr;
        std::shared_ptr<Vulkan::Storage_Manager> m_storage_manager_ptr;
        std::shared_ptr<Vulkan::Shader_Manager> m_shader_manager_ptr;
        std::shared_ptr<Vulkan::Descriptor_Set_Manager> m_descriptor_set_manager_ptr;
        std::shared_ptr<Vulkan::Pipeline_Manager> m_pipeline_manager_ptr;
        std::shared_ptr<Vulkan::Command_Buffer_Manager> m_command_buffer_manager_ptr;
        std::shared_ptr<Vulkan::Synchronization_Manager> m_synchronization_manager_ptr;
        std::shared_ptr<Cascade_Graphics_Debugging::Vulkan::Validation_Layer> m_validation_layer_ptr;
#endif
    private:
#ifdef CSD_USE_VULKAN
        void Initialize_Vulkan();
        void Record_Vulkan_Command_Buffers(unsigned int width, unsigned int height);
        void Recreate_Swapchain();
#endif
    public:
        Renderer(Window_Information window_information);
        ~Renderer();

    public:
        void Render_Frame();
        void Update_Voxels();
        void Start_Rendering();

        std::shared_ptr<Camera> Get_Camera();
        std::shared_ptr<Object_Manager> Get_Object_Manager();
    };
} // namespace Cascade_Graphics