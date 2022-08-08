#pragma once

#include <chrono>
#include <utility>
#include <vector>

namespace Cascade_Core
{
    class Event_Manager
    {
    public:
        enum Event_Type
        {
            BUTTON_PRESS,
            BUTTON_RELEASE,
            POINTER_MOTION,
            ENTER_WINDOW,
            LEAVE_WINDOW,
            KEY_PRESS,
            KEY_RELEASE
        };

        struct Mouse_State
        {
            int32_t x_position;
            int32_t y_position;

            bool left_down;
            bool middle_down;
            bool right_down;
        };

        struct Button_Press_Event
        {
            Event_Type event_type;

            int32_t x_position;
            int32_t y_position;

            enum Button
            {
                LEFT_BUTTON,
                MIDDLE_BUTTON,
                RIGHT_BUTTON
            } button;
        };

        struct Button_Release_Event
        {
            Event_Type event_type;

            int32_t x_position;
            int32_t y_position;

            enum Button
            {
                LEFT_BUTTON,
                MIDDLE_BUTTON,
                RIGHT_BUTTON
            } button;
        };

        struct Pointer_Motion_Event
        {
            Event_Type event_type;

            int32_t x_position;
            int32_t y_position;
        };

    private:
        Mouse_State m_mouse_state;

        uint32_t m_max_events;
        std::vector<std::pair<void*, std::chrono::time_point<std::chrono::system_clock>>> m_all_events;
        std::vector<std::pair<Button_Press_Event, std::chrono::time_point<std::chrono::system_clock>>> m_button_press_events;
        std::vector<std::pair<Button_Release_Event, std::chrono::time_point<std::chrono::system_clock>>> m_button_release_events;
        std::vector<std::pair<Pointer_Motion_Event, std::chrono::time_point<std::chrono::system_clock>>> m_pointer_motion_events;

    private:
        Event_Type Get_Event_Type(void* event_data);

    public:
        Event_Manager(uint32_t max_events);

    public:
        void Add_Event(void* event_data);

        Mouse_State Get_Mouse_State();
    };
} // namespace Cascade_Core