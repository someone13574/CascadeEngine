#pragma once

#include "Vulkan_Backend/command_buffer.hpp"
#include "Vulkan_Backend/compute_pipeline.hpp"
#include "Vulkan_Backend/descriptor_set.hpp"
#include "Vulkan_Backend/fence.hpp"
#include "Vulkan_Backend/image.hpp"
#include "Vulkan_Backend/semaphore.hpp"
#include "Vulkan_Backend/surface.hpp"
#include "Vulkan_Backend/swapchain.hpp"
#include "graphics.hpp"
#include "platform.hpp"
#include "platform_info.hpp"
#include "renderer.hpp"
#include "vulkan_graphics.hpp"

namespace Cascade_Graphics
{
    class Vulkan_Renderer : public Renderer
    {
        friend class Vulkan_Graphics_Factory;

    private:
        Vulkan_Graphics* m_vulkan_graphics_ptr;

        Vulkan::Surface* m_surface_ptr;
        Vulkan::Swapchain* m_swapchain_ptr;
        std::vector<Vulkan::Image*> m_render_target_ptrs;
        std::vector<Vulkan::Descriptor_Set*> m_rendering_descriptor_set_ptrs;
        Vulkan::Compute_Pipeline* m_rendering_pipeline_ptr;
        Vulkan::Command_Buffer* m_rendering_command_buffers_ptr;
        Vulkan::Semaphore* m_rendering_complete_semaphores_ptr;
        Vulkan::Semaphore* m_image_available_semaphores_ptr;
        Vulkan::Fence* m_command_buffer_complete_fences_ptr;

        uint32_t m_active_command_buffer_index = 0;

    private:
        Vulkan_Renderer(Graphics* graphics_ptr, Platform platform, Window_Info* window_info_ptr);

    public:
        ~Vulkan_Renderer();

    public:
        void Render_Frame() override;
    };
}    // namespace Cascade_Graphics