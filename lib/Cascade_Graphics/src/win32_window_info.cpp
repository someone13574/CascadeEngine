#include "win32_window_info.hpp"

namespace Cascade_Graphics
{
    WIN32_Window_Info::WIN32_Window_Info(uint32_t width, uint32_t height, void* hinstance_ptr, void* hwnd_ptr) : Window_Info::Window_Info(width, height), m_instance_ptr(hinstance_ptr), m_window_ptr(hwnd_ptr)
    {
    }

    void* WIN32_Window_Info::Get_Instance()
    {
        return m_instance_ptr;
    }

    void* WIN32_Window_Info::Get_Window()
    {
        return m_window_ptr;
    }
} // namespace Cascade_Graphics