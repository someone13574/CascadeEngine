#pragma once

#include "Vulkan_Backend/surface.hpp"
#include "graphics.hpp"
#include "renderer.hpp"
#include "window_info.hpp"
#include "window_platform.hpp"

namespace Cascade_Graphics
{
    class Vulkan_Renderer : public Renderer
    {
        friend class Vulkan_Graphics_Factory;

    private:
        Graphics* m_graphics_ptr;
        Vulkan::Surface* m_surface_ptr;

    private:
        Vulkan_Renderer(Graphics* graphics_ptr, Window_Platform window_platform, Window_Info* window_info_ptr);

    public:
        ~Vulkan_Renderer();
    };
} // namespace Cascade_Graphics