#include "xcb_platform_info.hpp"

namespace Cascade_Graphics
{
    XCB_Platform_Info::XCB_Platform_Info(void** connection_ptr, void* visual_id_ptr) : Platform_Info::Platform_Info(Platform::LINUX_XCB), m_connection_ptr(connection_ptr), m_visual_id_ptr(visual_id_ptr)
    {
    }

    void** XCB_Platform_Info::Get_Connection()
    {
        return m_connection_ptr;
    }

    void* XCB_Platform_Info::Get_Visual_Id()
    {
        return m_visual_id_ptr;
    }

    XCB_Window_Info::XCB_Window_Info(uint32_t width, uint32_t height, void** connection_ptr, void* window_ptr) : Window_Info::Window_Info(Platform::LINUX_XCB, width, height), m_connection_ptr(connection_ptr), m_window_ptr(window_ptr)
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