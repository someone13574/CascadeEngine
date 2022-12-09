#pragma once

#include "graphics.hpp"
#include "graphics_platform.hpp"
#include "renderer.hpp"
#include "window_info.hpp"

namespace Cascade_Graphics
{
    class Graphics_Factory
    {
    protected:
        Graphics_Platform m_graphics_platform;

    public:
        Graphics_Factory(Graphics_Platform graphics_platform);
        virtual ~Graphics_Factory() = default;

        virtual Graphics* Create_Graphics() const = 0;
        virtual Renderer* Create_Renderer(Graphics* graphics_ptr, Window_Info* window_info_ptr) const = 0;
    };
} // namespace Cascade_Graphics