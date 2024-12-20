#pragma once

#include "cascade_graphics.hpp"
#include "event_manager.hpp"

#include <memory>
#include <string>
#include <thread>
#include <utility>

#ifdef __linux__

#include <xcb/xcb.h>

#elif defined _WIN32 || defined WIN32

#include <tchar.h>
#include <windows.h>
#include <windowsx.h>

#endif

namespace Cascade_Core
{
    class Window
    {
    public:
        enum Initialization_Stage
        {
            NOT_STARTED,
            WINDOW_CREATED,
            RENDERER_CREATED,
            CLEANED_UP,
        };

    private:
        std::string m_window_title;
        uint32_t m_width;
        uint32_t m_height;

        std::shared_ptr<Event_Manager> m_event_manager_ptr;

        std::shared_ptr<Cascade_Graphics::Renderer> m_renderer_ptr;
        std::shared_ptr<Cascade_Graphics::Vulkan_Backend::Vulkan_Graphics> m_graphics_ptr;

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
        xcb_window_t m_xcb_window;
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
        Window(std::string window_title, uint32_t width, uint32_t height, std::shared_ptr<Cascade_Graphics::Vulkan_Backend::Vulkan_Graphics> graphics_ptr);

    public:
        void Close_Window();

        bool Is_Window_Closed();
        bool Is_Requesting_Close();

        void Update_Size(uint32_t width, uint32_t height);

        std::pair<uint32_t, uint32_t> Get_Window_Dimensions();
        Initialization_Stage Get_Initialization_Stage();
        std::shared_ptr<Event_Manager> Get_Event_Manager();
        std::shared_ptr<Cascade_Graphics::Renderer> Get_Renderer();
    };
} // namespace Cascade_Core