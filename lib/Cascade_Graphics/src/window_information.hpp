#pragma once


#ifdef __linux

#include <xcb/xcb.h>

namespace Cascade_Graphics
{
    struct Window_Information
    {
        unsigned int* width_ptr;
        unsigned int* height_ptr;

        xcb_window_t* xcb_window_ptr;
        xcb_connection_t* xcb_connection_ptr;
    };
} // namespace Cascade_Graphics

#elif defined _WIN32 || defined WIN32

#include <windows.h>

namespace Cascade_Graphics
{
    struct Window_Information
    {
        unsigned int* width_ptr;
        unsigned int* height_ptr;

        HWND* hwindow_ptr;
        HINSTANCE* hinstance_ptr;
    };
} // namespace Cascade_Graphics

#endif