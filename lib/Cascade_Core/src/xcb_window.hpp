#pragma once

#include "engine_thread_manager.hpp"
#include "window.hpp"
#include <acorn_logging.hpp>
#include <xcb/xcb.h>

namespace Cascade_Core
{
    class XCB_Window : public Window
    {
        friend class XCB_Window_Factory;

    private:
        xcb_connection_t* m_xcb_connection_ptr;
        xcb_screen_t* m_xcb_screen_ptr;
        xcb_window_t m_xcb_window;
        xcb_generic_error_t* m_xcb_error_ptr;
        xcb_void_cookie_t m_xcb_request_check_cookie;

    private:
        XCB_Window(std::string window_title, uint32_t window_width, uint32_t window_height, Engine_Thread_Manager* thread_manager_ptr);

    public:
        ~XCB_Window();

        void Process_Events() const override;
    };

    class XCB_Window_Factory : public Window_Factory
    {
    public:
        Window* Create_Window(std::string window_title, uint32_t window_width, uint32_t window_height, Engine_Thread_Manager* thread_manager_ptr) const override;
    };
} // namespace Cascade_Core