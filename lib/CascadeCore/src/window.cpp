#include "window.hpp"
#include "cascade_logging.hpp"

#if defined __unix__

#include <cstring>
#include <unistd.h>

namespace CascadeCore
{
    Window::Window(unsigned int width, unsigned int height, std::string title, Application* owner)
        : m_window_width(width), m_window_height(height), m_window_title(title), m_event_manager_ptr(std::make_shared<Event_Manager>(this)), m_owner_application(owner)
    {
        LOG_DEBUG << "Creating a new window for '" << m_owner_application->Get_Application_Name() << "'";

        m_xcb_connection = xcb_connect(NULL, NULL);

        m_xcb_screen = xcb_setup_roots_iterator(xcb_get_setup(m_xcb_connection)).data;
        m_xcb_window = xcb_generate_id(m_xcb_connection);

        unsigned int mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
        unsigned int values[2];
        values[0] = m_xcb_screen->white_pixel;
        values[1] = 0;

        xcb_create_window(m_xcb_connection, XCB_COPY_FROM_PARENT, m_xcb_window, m_xcb_screen->root, 0, 0, m_window_width, m_window_height, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                          m_xcb_screen->root_visual, mask, values);

        Set_Property(XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, m_window_title.length(), const_cast<char*>(m_window_title.c_str()));

        xcb_intern_atom_cookie_t close_window_tmp_cookie = xcb_intern_atom(m_xcb_connection, 1, 12, "WM_PROTOCOLS");
        xcb_intern_atom_reply_t* close_window_tmp_reply = xcb_intern_atom_reply(m_xcb_connection, close_window_tmp_cookie, 0);

        m_xcb_close_window_cookie = xcb_intern_atom(m_xcb_connection, 0, 16, "WM_DELETE_WINDOW");
        m_xcb_close_window_reply = xcb_intern_atom_reply(m_xcb_connection, m_xcb_close_window_cookie, 0);

        xcb_change_property(m_xcb_connection, XCB_PROP_MODE_REPLACE, m_xcb_window, (*close_window_tmp_reply).atom, 4, 32, 1, &(*m_xcb_close_window_reply).atom);

        xcb_map_window(m_xcb_connection, m_xcb_window);
        xcb_flush(m_xcb_connection);

        LOG_DEBUG << "Window created";
    }

    Window::~Window()
    {
        LOG_DEBUG << "Started window cleanup";

        free(m_xcb_event);
        xcb_destroy_window(m_xcb_connection, m_xcb_window);
        xcb_disconnect(m_xcb_connection);

        LOG_TRACE << "Window cleanup complete";
    }

    void Window::Set_Property(xcb_atom_t property, xcb_atom_enum_t data_type, unsigned int format, unsigned int data_size, void* data)
    {
        xcb_change_property(m_xcb_connection, XCB_PROP_MODE_REPLACE, m_xcb_window, property, data_type, format, data_size, data);
    }

    void Window::Update_Event_Types()
    {
        unsigned int event_types[8] = {0,
                                       XCB_EVENT_MASK_BUTTON_PRESS,
                                       XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE,
                                       XCB_EVENT_MASK_POINTER_MOTION,
                                       XCB_EVENT_MASK_ENTER_WINDOW,
                                       XCB_EVENT_MASK_LEAVE_WINDOW,
                                       XCB_EVENT_MASK_KEY_PRESS,
                                       XCB_EVENT_MASK_KEY_RELEASE};

        unsigned int enabled_events[] = {0};

        for (unsigned int i = 0; i < 7; i++)
        {
            if (m_event_manager_ptr->Get_Enabled_Event_Types()[i])
            {
                enabled_events[0] |= event_types[i];
            }
        }

        xcb_change_window_attributes(m_xcb_connection, m_xcb_window, XCB_CW_EVENT_MASK, enabled_events);
        xcb_flush(m_xcb_connection);

        LOG_TRACE << "Updated event masks";
    }

    void Window::Process_Events()
    {
        m_xcb_event = xcb_wait_for_event(m_xcb_connection);

        LOG_TRACE << "Received a xcb event";

        switch (m_xcb_event->response_type & (~0x80))
        {
            case XCB_CLIENT_MESSAGE:
            {
                if ((*(xcb_client_message_event_t*)m_xcb_event).data.data32[0] == (*m_xcb_close_window_reply).atom)
                {
                    Event_Manager::Window_Close_Event event_struct = {};
                    event_struct.window_to_close = this;

                    m_event_manager_ptr->Execute_Window_Close_Event(event_struct);
                }
            }
            case XCB_BUTTON_PRESS:
            {
                xcb_button_press_event_t* button_press_event = (xcb_button_press_event_t*)m_xcb_event;

                Event_Manager::Button_Press_Event event_struct = {};
                event_struct.x = button_press_event->event_x;
                event_struct.y = button_press_event->event_y;
                event_struct.button = (unsigned int)button_press_event->detail;

                m_event_manager_ptr->Execute_Button_Press_Event(event_struct);
                break;
            }
            case XCB_BUTTON_RELEASE:
            {
                xcb_button_release_event_t* butten_release_event = (xcb_button_release_event_t*)m_xcb_event;

                Event_Manager::Button_Release_Event event_struct = {};
                event_struct.x = butten_release_event->event_x;
                event_struct.y = butten_release_event->event_y;
                event_struct.button = (unsigned int)butten_release_event->detail;

                m_event_manager_ptr->Execute_Button_Release_Event(event_struct);
                break;
            }
            case XCB_MOTION_NOTIFY:
            {
                xcb_motion_notify_event_t* motion_notify_event = (xcb_motion_notify_event_t*)m_xcb_event;

                Event_Manager::Pointer_Movement_Event event_struct = {};
                event_struct.x = motion_notify_event->event_x;
                event_struct.y = motion_notify_event->event_y;

                m_event_manager_ptr->Execute_Pointer_Motion_Event(event_struct);

                break;
            }
        }
    }

    void Window::Send_Close_Event()
    {
        LOG_TRACE << "Sending close event to event loop";

        xcb_client_message_event_t event;
        memset(&event, 0, sizeof(event));

        event.response_type = XCB_CLIENT_MESSAGE;
        event.format = 32;
        event.sequence = 0;
        event.window = m_xcb_window;
        event.type = (*m_xcb_close_window_reply).atom;
        event.data.data32[0] = 0;

        xcb_send_event(m_xcb_connection, false, m_xcb_window, XCB_EVENT_MASK_NO_EVENT, reinterpret_cast<const char*>(&event));
        xcb_flush(m_xcb_connection);
    }

    std::shared_ptr<Event_Manager> Window::Get_Event_Manager()
    {
        return m_event_manager_ptr;
    }

    Application* Window::Get_Owner_Application()
    {
        return m_owner_application;
    }
} // namespace CascadeCore

#elif defined _WIN32 || WIN32

namespace CascadeCore
{
    Window::Window(unsigned int width, unsigned int height, std::string title, Application* owner)
        : m_window_width(width), m_window_height(height), m_window_title(title), m_event_manager_ptr(std::make_shared<Event_Manager>(this)), m_owner_application(owner)
    {
        LOG_DEBUG << "Creating a new window for '" << m_owner_application->Get_Application_Name() << "'";

        LOG_DEBUG << "Window created";
    }

    Window::~Window()
    {
        LOG_DEBUG << "Started window cleanup";

        LOG_TRACE << "Window cleanup complete";
    }

    void Window::Update_Event_Types()
    {
        LOG_TRACE << "Updated event masks";
    }

    void Window::Process_Events()
    {
    }

    void Window::Send_Close_Event()
    {
        LOG_TRACE << "Sending close event to event loop";
    }

    std::shared_ptr<Event_Manager> Window::Get_Event_Manager()
    {
        return m_event_manager_ptr;
    }

    Application* Window::Get_Owner_Application()
    {
        return m_owner_application;
    }
} // namespace CascadeCore

#endif