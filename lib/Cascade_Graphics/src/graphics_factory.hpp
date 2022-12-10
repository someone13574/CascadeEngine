#pragma once

#include "graphics.hpp"
#include "platform.hpp"
#include "platform_info.hpp"
#include "renderer.hpp"

namespace Cascade_Graphics
{
    class Graphics_Factory
    {
    protected:
        Platform m_platform;

    public:
        Graphics_Factory(Platform platform);
        virtual ~Graphics_Factory() = default;

        virtual Graphics* Create_Graphics(Platform_Info* platform_info) const = 0;
        virtual Renderer* Create_Renderer(Graphics* graphics_ptr, Window_Info* window_info_ptr) const = 0;
    };
} // namespace Cascade_Graphics