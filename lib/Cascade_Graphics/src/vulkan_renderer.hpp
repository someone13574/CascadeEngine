#pragma once

#include "Vulkan_Backend/buffer.hpp"
#include "Vulkan_Backend/compute_pipeline.hpp"
#include "Vulkan_Backend/descriptor_set.hpp"
#include "Vulkan_Backend/surface.hpp"
#include "Vulkan_Backend/swapchain.hpp"
#include "graphics.hpp"
#include "platform.hpp"
#include "platform_info.hpp"
#include "renderer.hpp"

namespace Cascade_Graphics
{
    class Vulkan_Renderer : public Renderer
    {
        friend class Vulkan_Graphics_Factory;

    private:
        Vulkan::Surface* m_surface_ptr;
        Vulkan::Swapchain* m_swapchain_ptr;
        Vulkan::Buffer* m_camera_buffer_ptr;
        Vulkan::Descriptor_Set* m_rendering_descriptor_set_ptr;
        Vulkan::Compute_Pipeline* m_rendering_pipeline_ptr;

    private:
        Vulkan_Renderer(Graphics* graphics_ptr, Platform platform, Window_Info* window_info_ptr);

    public:
        ~Vulkan_Renderer();
    };
}    // namespace Cascade_Graphics