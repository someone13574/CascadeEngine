#include "xcb_window.hpp"
#include <acorn_logging.hpp>

namespace Cascade_Core
{
    XCB_Window::XCB_Window(std::string window_title, uint32_t window_width, uint32_t window_height, Engine_Thread_Manager* thread_manager_ptr) : Window::Window(window_title, window_width, window_height, thread_manager_ptr)
    {
        LOG_INFO << "Core: Creating an XCB window with title '" << m_window_title << "' and dimensions " << m_window_width << "x" << m_window_height;

        m_xcb_connection_ptr = xcb_connect(nullptr, nullptr);
        m_xcb_screen_ptr = xcb_setup_roots_iterator(xcb_get_setup(m_xcb_connection_ptr)).data;
        m_xcb_window = xcb_generate_id(m_xcb_connection_ptr);

        uint32_t values[2] = {m_xcb_screen_ptr->white_pixel, 0};

        m_xcb_request_check_cookie = xcb_create_window_checked(m_xcb_connection_ptr, XCB_COPY_FROM_PARENT, m_xcb_window, m_xcb_screen_ptr->root, 0, 0, m_window_width, m_window_height, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT, m_xcb_screen_ptr->root_visual,
                                                               XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK, values);
        m_xcb_request_check_cookie = xcb_change_property_checked(m_xcb_connection_ptr, XCB_PROP_MODE_REPLACE, m_xcb_window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, m_window_title.length(), const_cast<char*>(m_window_title.c_str()));
        m_xcb_request_check_cookie = xcb_map_window_checked(m_xcb_connection_ptr, m_xcb_window);

        m_xcb_error_ptr = xcb_request_check(m_xcb_connection_ptr, m_xcb_request_check_cookie);
        if (m_xcb_error_ptr != NULL)
        {
            LOG_ERROR << "Core: XCB request check failed with error code " << m_xcb_error_ptr->error_code;
            exit(EXIT_FAILURE);
        }

        int xcb_flush_result = xcb_flush(m_xcb_connection_ptr);
        if (xcb_flush_result <= 0)
        {
            LOG_ERROR << "Core: XCB flush failed with error code " << xcb_flush_result;
        }
    }

    XCB_Window::~XCB_Window()
    {
    }

    void XCB_Window::Process_Events() const
    {
    }

    Window* XCB_Window_Factory::Create_Window(std::string window_title, uint32_t window_width, uint32_t window_height, Engine_Thread_Manager* thread_manager_ptr) const
    {
        return new XCB_Window(window_title, window_width, window_height, thread_manager_ptr);
    }
} // namespace Cascade_Core