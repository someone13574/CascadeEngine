#include "xcb_platform_info.hpp"
#include <acorn_logging.hpp>

#ifdef __linux__
    #include <xcb/xcb.h>
#endif

namespace Cascade_Graphics
{
    XCB_Platform_Info::XCB_Platform_Info(void** connection_ptr, void* visual_id_ptr) :
        Platform_Info::Platform_Info(Platform::LINUX_XCB), m_connection_ptr(connection_ptr), m_visual_id_ptr(visual_id_ptr)
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

    XCB_Window_Info::XCB_Window_Info(uint32_t width, uint32_t height, void** connection_ptr, void* window_ptr) :
        Window_Info::Window_Info(Platform::LINUX_XCB, width, height), m_connection_ptr(connection_ptr), m_window_ptr(window_ptr)
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

    void XCB_Window_Info::Update_Window_Info()
    {
#ifdef __linux__
        xcb_get_geometry_cookie_t geometry_cookie = xcb_get_geometry(*reinterpret_cast<xcb_connection_t**>(m_connection_ptr), *reinterpret_cast<xcb_window_t*>(m_window_ptr));
        xcb_generic_error_t* error = nullptr;
        xcb_get_geometry_reply_t* geometry_reply = xcb_get_geometry_reply(*reinterpret_cast<xcb_connection_t**>(m_connection_ptr), geometry_cookie, &error);

        if (error != NULL)
        {
            LOG_FATAL << "Core: XCB request check failed with error code " << error->error_code;
            exit(EXIT_FAILURE);
        }

        if (geometry_reply == NULL)
        {
            LOG_FATAL << "Core: XCB geometry reply is NULL";
            exit(EXIT_FAILURE);
        }

        m_window_width = geometry_reply->width;
        m_window_height = geometry_reply->height;
#endif
    }
}    // namespace Cascade_Graphics