#pragma once

#include "graphics.hpp"
#include "renderer.hpp"
#include "window_info.hpp"
#include "window_platform.hpp"

namespace Cascade_Graphics
{
    class Graphics_Factory
    {
    protected:
        Window_Platform m_window_platform;

    public:
        Graphics_Factory(Window_Platform window_platform);
        virtual ~Graphics_Factory() = default;

        virtual Graphics* Create_Graphics() const = 0;
        virtual Renderer* Create_Renderer(Graphics* graphics_ptr, Window_Info* window_info_ptr) const = 0;
    };
} // namespace Cascade_Graphics