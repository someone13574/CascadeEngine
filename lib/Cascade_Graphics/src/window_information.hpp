#pragma once


#ifdef __linux

#include <cstdint>
#include <xcb/xcb.h>

namespace Cascade_Graphics
{
    struct Window_Information
    {
        uint32_t* width_ptr;
        uint32_t* height_ptr;

        xcb_window_t* xcb_window_ptr;
        xcb_connection_t* xcb_connection_ptr;
    };
} // namespace Cascade_Graphics

#elif defined _WIN32 || defined WIN32

#include <cstdint>
#include <windows.h>

namespace Cascade_Graphics
{
    struct Window_Information
    {
        uint32_t* width_ptr;
        uint32_t* height_ptr;

        HWND* hwindow_ptr;
        HINSTANCE* hinstance_ptr;
    };
} // namespace Cascade_Graphics

#endif