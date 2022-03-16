#pragma once

#include "application.hpp"
#include "event_manager.hpp"
#include "renderer.hpp"

#if defined __linux__

#include <memory>
#include <string>
#include <xcb/xcb.h>

namespace CascadeCore
{
    class Application;
    class Event_Manager;

    class Window
    {
    public:
        static void Close_Window_Event(void* data);

    private:
        unsigned int m_window_width;
        unsigned int m_window_height;
        std::string m_window_title;

        Application* m_owner_application;
        std::shared_ptr<Event_Manager> m_event_manager_ptr;
        std::shared_ptr<Renderer> m_renderer;

        xcb_connection_t* m_xcb_connection;
        xcb_screen_t* m_xcb_screen;
        xcb_window_t m_xcb_window;
        xcb_generic_event_t* m_xcb_event;

        xcb_intern_atom_cookie_t m_xcb_close_window_cookie;
        xcb_intern_atom_reply_t* m_xcb_close_window_reply;

    private:
        void Set_Property(xcb_atom_t property, xcb_atom_enum_t data_type, unsigned int format, unsigned int data_size, void* data);

    public:
        Window(unsigned int width, unsigned int height, std::string title, Application* owner);
        ~Window();

        void Update_Event_Types();
        void Process_Events();
        void Send_Close_Event();

        std::shared_ptr<Event_Manager> Get_Event_Manager();
        Application* Get_Owner_Application();
    };
} // namespace CascadeCore

#elif defined _WIN32 || defined WIN32

#include <memory>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <tchar.h>
#include <windows.h>

namespace CascadeCore
{
    class Application;
    class Event_Manager;

    class Window
    {
    public:
        static void Close_Window_Event(void* data);

    private:
        unsigned int m_window_width;
        unsigned int m_window_height;
        std::string m_window_title;

        Application* m_owner_application;
        std::shared_ptr<Event_Manager> m_event_manager_ptr;
        std::shared_ptr<Renderer> m_renderer;

        HWND m_window;
        HINSTANCE m_hinstance;

    public:
        Window(unsigned int width, unsigned int height, std::string title, Application* owner);
        ~Window();

        void Update_Event_Types();
        void Process_Events();
        void Send_Close_Event();

        std::shared_ptr<Event_Manager> Get_Event_Manager();
        Application* Get_Owner_Application();
    };
} // namespace CascadeCore

#endif