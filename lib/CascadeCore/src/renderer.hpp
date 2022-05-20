#pragma once

#if defined __linux__

#include <xcb/xcb.h>

#elif defined _WIN32 || defined WIN32

#include <windows.h>

#endif

#include "cascade_graphics.hpp"

namespace CascadeCore
{
    class Renderer
    {
    private:
        unsigned int m_width;
        unsigned int m_height;

    public:
        Renderer(CascadeGraphics::Vulkan::Surface::Window_Data window_data, unsigned int width, unsigned int height);
        ~Renderer();

        void Render_Frame();
    };
} // namespace CascadeCore