#ifdef __linux__

#include "xcb_window.hpp"
#include <acorn_logging.hpp>

namespace Cascade_Core
{
    XCB_Window::XCB_Window(std::string window_title, uint32_t window_width, uint32_t window_height, Cascade_Graphics::Graphics* graphics_ptr, Engine_Thread_Manager* thread_manager_ptr, Cascade_Graphics::Graphics_Factory* m_graphics_factory_ptr)
        : Window::Window(window_title, window_width, window_height, graphics_ptr, thread_manager_ptr, m_graphics_factory_ptr)
    {
        m_window_thread_ptr->Start_Thread();
        m_window_thread_ptr->Await_State(Engine_Thread::Thread_State::LOOP_FUNC);

        m_renderer_ptr = m_graphics_factory_ptr->Create_Renderer(m_graphics_ptr);
    }

    XCB_Window::~XCB_Window()
    {
    }

    void XCB_Window::Create_Window()
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
    }

    void XCB_Window::Process_Events()
    {
        m_event_ptr = xcb_wait_for_event(m_xcb_connection_ptr);

        switch (m_event_ptr->response_type & (~0x80))
        {
            case XCB_CLIENT_MESSAGE:
                if ((*(xcb_client_message_event_t*)m_event_ptr).data.data32[0] == (*m_xcb_close_window_reply_ptr).atom)
                {
                    LOG_INFO << "Core: Received close window event in window '" << m_window_title << "'";
                    m_window_thread_ptr->Exit_Thread();
                }
                break;
            default:
                break;
        }
    }

    void XCB_Window::Destroy_Window()
    {
        LOG_DEBUG << "Core: Destroying window '" << m_window_title << "' and disconnecting XCB";

        free(m_event_ptr);
        xcb_destroy_window(m_xcb_connection_ptr, m_xcb_window);
        xcb_disconnect(m_xcb_connection_ptr);
    }
} // namespace Cascade_Core

#endif