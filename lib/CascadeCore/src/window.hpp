#pragma once

#include "event_manager.hpp"
#include "renderer.hpp"

#include <memory>
#include <string>
#include <thread>

#ifdef __linux__

#include <xcb/xcb.h>

#elif defined _WIN32 || defined WIN32

#include <tchar.h>
#include <windows.h>

#endif

namespace Cascade_Core
{
    class Window
    {
    private:
        enum Initialization_Stage
        {
            NOT_STARTED,
            WINDOW_CREATED,
            RENDERER_CREATED,
            CLEANED_UP,
        };

    private:
        std::string m_window_title;
        unsigned int m_width;
        unsigned int m_height;

        std::shared_ptr<Event_Manager> m_event_manager_ptr;
        std::shared_ptr<Renderer> m_renderer_ptr;

        Initialization_Stage m_initialization_stage = Initialization_Stage::NOT_STARTED;

        bool m_requesting_close = false;
        bool m_threads_active = false;
        bool m_event_thread_stopped = false;
        bool m_render_thread_stopped = false;
        std::thread m_event_thread;
        std::thread m_render_thread;

#ifdef __linux__

        xcb_connection_t* m_xcb_connection_ptr;
        xcb_screen_t* m_xcb_screen_ptr;
        xcb_window_t m_xcb_window_ptr;
        xcb_generic_event_t* m_xcb_event_ptr;
        xcb_intern_atom_cookie_t m_xcb_close_window_cookie;
        xcb_intern_atom_reply_t* m_xcb_close_window_reply_ptr;

#elif defined _WIN32 || defined WIN32

        HWND m_hwindow;
        HINSTANCE m_hinstance;

#endif

    private:
        void Initialize_Window();
        void Initialize_Renderer();

        static void Event_Loop(Window* window_ptr);
        static void Render_Loop(Window* window_ptr);

    public:
        Window(std::string window_title, unsigned int width, unsigned int height);

    public:
        void Close_Window();

        bool Is_Window_Closed();
        bool Is_Requesting_Close();
    };
} // namespace Cascade_Core