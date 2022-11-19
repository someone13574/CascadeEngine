#ifdef __linux__

#include "xcb_window.hpp"
#include <acorn_logging.hpp>

namespace Cascade_Core
{
    XCB_Window::XCB_Window(std::string window_title, uint32_t window_width, uint32_t window_height, Engine_Thread_Manager* thread_manager_ptr) : Window::Window(window_title, window_width, window_height, thread_manager_ptr)
    {
        LOG_INFO << "Core: Creating a XCB window with title '" << m_window_title << "' and dimensions " << m_window_width << "x" << m_window_height;

        m_xcb_connection_ptr = xcb_connect(nullptr, nullptr);
        m_xcb_screen_ptr = xcb_setup_roots_iterator(xcb_get_setup(m_xcb_connection_ptr)).data;
        m_xcb_window = xcb_generate_id(m_xcb_connection_ptr);

        uint32_t values[2] = {m_xcb_screen_ptr->white_pixel, 0};

        m_xcb_request_check_cookie = xcb_create_window_checked(m_xcb_connection_ptr, XCB_COPY_FROM_PARENT, m_xcb_window, m_xcb_screen_ptr->root, 0, 0, m_window_width, m_window_height, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT, m_xcb_screen_ptr->root_visual,
                                                               XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK, values);
        m_xcb_request_check_cookie = xcb_change_property_checked(m_xcb_connection_ptr, XCB_PROP_MODE_REPLACE, m_xcb_window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, m_window_title.length(), const_cast<char*>(m_window_title.c_str()));
        m_xcb_request_check_cookie = xcb_map_window_checked(m_xcb_connection_ptr, m_xcb_window);
        m_xcb_request_check_cookie = xcb_change_window_attributes_checked(m_xcb_connection_ptr, m_xcb_window, XCB_CW_EVENT_MASK, m_enabled_event_types);

        xcb_intern_atom_cookie_t close_window_tmp_cookie = xcb_intern_atom(m_xcb_connection_ptr, 1, 12, "WM_PROTOCOLS");
        xcb_intern_atom_reply_t* close_window_tmp_reply = xcb_intern_atom_reply(m_xcb_connection_ptr, close_window_tmp_cookie, 0);
        m_xcb_close_window_cookie = xcb_intern_atom(m_xcb_connection_ptr, 0, 16, "WM_DELETE_WINDOW");
        m_xcb_close_window_reply_ptr = xcb_intern_atom_reply(m_xcb_connection_ptr, m_xcb_close_window_cookie, 0);
        m_xcb_request_check_cookie = xcb_change_property_checked(m_xcb_connection_ptr, XCB_PROP_MODE_REPLACE, m_xcb_window, (*close_window_tmp_reply).atom, 4, 32, 1, &(*m_xcb_close_window_reply_ptr).atom);

        m_xcb_error_ptr = xcb_request_check(m_xcb_connection_ptr, m_xcb_request_check_cookie);
        if (m_xcb_error_ptr != NULL)
        {
            LOG_FATAL << "Core: XCB request check failed with error code " << m_xcb_error_ptr->error_code;
            exit(EXIT_FAILURE);
        }

        int xcb_flush_result = xcb_flush(m_xcb_connection_ptr);
        if (xcb_flush_result <= 0)
        {
            LOG_FATAL << "Core: XCB flush failed with error code " << xcb_flush_result;
        }

        m_initialized_window = true;
    }

    XCB_Window::~XCB_Window()
    {
        LOG_INFO << "Core: Destroying XCB window '" << m_window_title << "'";
    }

    void XCB_Window::Process_Events(Engine_Thread* event_thread_ptr) const
    {
        if (m_initialized_window)
        {
            xcb_generic_event_t* event_ptr = xcb_wait_for_event(m_xcb_connection_ptr);

            switch (event_ptr->response_type & (~0x80))
            {
                case XCB_CLIENT_MESSAGE:
                    if ((*(xcb_client_message_event_t*)event_ptr).data.data32[0] == (*m_xcb_close_window_reply_ptr).atom)
                    {
                        LOG_INFO << "Core: Received close window event in window '" << m_window_title << "'";
                        event_thread_ptr->Exit_Thread();
                    }
                    break;
                default:
                    break;
            }
        }
    }

    Window* XCB_Window_Factory::Create_Window(std::string window_title, uint32_t window_width, uint32_t window_height, Engine_Thread_Manager* thread_manager_ptr) const
    {
        return new XCB_Window(window_title, window_width, window_height, thread_manager_ptr);
    }
} // namespace Cascade_Core

#endif