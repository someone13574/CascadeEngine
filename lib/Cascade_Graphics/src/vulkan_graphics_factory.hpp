#pragma once

#include "graphics_factory.hpp"
#include "vulkan_graphics.hpp"
#include "vulkan_renderer.hpp"

namespace Cascade_Graphics
{
    class Vulkan_Graphics_Factory : public Graphics_Factory
    {
    public:
        Graphics* Create_Graphics() const override;
        Renderer* Create_Renderer(Graphics* graphics_ptr) const override;
    };
} // namespace Cascade_Graphics