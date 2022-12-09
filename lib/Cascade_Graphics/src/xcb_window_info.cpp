#include "xcb_window_info.hpp"

namespace Cascade_Graphics
{
    XCB_Window_Info::XCB_Window_Info(uint32_t width, uint32_t height, void** connection_ptr, void* window_ptr) : Window_Info::Window_Info(width, height), m_connection_ptr(connection_ptr), m_window_ptr(window_ptr)
    {
    }

    void** XCB_Window_Info::Get_Connection()
    {
        return m_connection_ptr;
    }

    void* XCB_Window_Info::Get_Window()
    {
        return m_window_ptr;
    }
} // namespace Cascade_Graphics