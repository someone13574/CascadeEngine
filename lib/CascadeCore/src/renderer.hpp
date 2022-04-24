#pragma once

#if defined __linux__

#include <xcb/xcb.h>

namespace CascadeCore
{
    class Renderer
    {
    private:
        unsigned int m_width;
        unsigned int m_height;

    public:
        Renderer(xcb_connection_t* connection_ptr, xcb_window_t* window_ptr, unsigned int width, unsigned int height);
        ~Renderer();
    };
} // namespace CascadeCore

#elif defined _WIN32 || defined WIN32

#include <windows.h>

namespace CascadeCore
{
    class Renderer
    {
    private:
        unsigned int m_width;
        unsigned int m_height;

    public:
        Renderer(HINSTANCE* hinstance_ptr, HWND* hwnd_ptr, unsigned int width, unsigned int height);
        ~Renderer();
    };
} // namespace CascadeCore

#endif