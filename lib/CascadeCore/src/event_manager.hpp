#pragma once

#include "window.hpp"

#include <memory>
#include <vector>

namespace CascadeCore
{
    class Window;

    class Event_Manager
    {
    public:
        struct Window_Close_Event
        {
            Window* window_to_close;
        };
        struct Button_Press_Event
        {
            unsigned int x;
            unsigned int y;
            unsigned int button;
        };
        struct Button_Release_Event
        {
            unsigned int x;
            unsigned int y;
            unsigned int button;
        };
        struct Pointer_Movement_Event
        {
            unsigned int x;
            unsigned int y;
        };

        enum Event_Type
        {
            WINDOW_CLOSE,
            BUTTON_PRESS,
            BUTTON_RELEASE,
            POINTER_MOTION,
            ENTER_WINDOW,
            LEAVE_WINDOW,
            KEY_PRESS,
            KEY_RELEASE
        };

        class Event
        {
        private:
            Event_Type m_event_type;
            void (*m_execute_function)(void* data);

        public:
            Event(void (*function_to_execute)(void* data), Event_Type event_type);

            Event_Type Get_Event_Type();

            typedef void (*sig_ptr)(void* data);
            sig_ptr Get_Execute_Function()
            {
                return m_execute_function;
            }
        };

    private:
        bool m_active_event_types[8] = {false, false, false, false, false, false, false, false};
        std::vector<Event> m_events;

        Window* m_window_ptr;

    public:
        Event_Manager(Window* window_ptr);

        void Add_Event(Event event);
        void Add_Event(void (*function_to_execute)(void* data), Event_Type event_type);

        void Execute_Window_Close_Event(Window_Close_Event event_data);
        void Execute_Button_Press_Event(Button_Press_Event event_data);
        void Execute_Button_Release_Event(Button_Release_Event event_data);
        void Execute_Pointer_Motion_Event(Pointer_Movement_Event event_data);

        const bool* Get_Enabled_Event_Types();
    };
} // namespace CascadeCore
