#pragma once

#include "graphics.hpp"
#include "renderer.hpp"

namespace Cascade_Graphics
{
    class Vulkan_Renderer : public Renderer
    {
        friend class Vulkan_Graphics_Factory;

    private:
        Graphics* m_graphics_ptr;

    private:
        Vulkan_Renderer(Graphics* graphics_ptr);

    public:
        ~Vulkan_Renderer();
    };
} // namespace Cascade_Graphics