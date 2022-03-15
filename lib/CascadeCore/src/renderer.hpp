#pragma once

#if defined __linux__
#include <xcb/xcb.h>
#endif

namespace CascadeCore
{
    class Renderer
    {
    private:
        unsigned int m_width;
        unsigned int m_height;

    public:
#if defined __linux__
        Renderer(xcb_connection_t* connection_ptr, xcb_window_t* window_ptr);
#elif defined _WIN32 || WIN32
        Renderer();
#endif
        ~Renderer();
    };
} // namespace CascadeCore