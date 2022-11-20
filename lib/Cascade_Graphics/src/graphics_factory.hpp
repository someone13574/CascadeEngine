#pragma once

#include "graphics.hpp"
#include "renderer.hpp"

namespace Cascade_Graphics
{
    class Graphics_Factory
    {
    public:
        virtual Graphics* Create_Graphics() const = 0;
        virtual Renderer* Create_Renderer(Graphics* graphics_ptr) const = 0;
    };
} // namespace Cascade_Graphics