#include "event_manager.hpp"

#include "cascade_logging.hpp"

namespace Cascade_Core
{
    Event_Manager::Event_Manager(uint32_t max_events) : m_max_events(max_events)
    {
        LOG_INFO << "Core: Created event manager";

        m_mouse_state = {0, 0, false, false, false};
    }

    Event_Manager::Event_Type Event_Manager::Get_Event_Type(void* event_data)
    {
        struct Event_Type_Struct
        {
            Event_Type event_type;
        };

        Event_Type_Struct* event_type = (Event_Type_Struct*)event_data;

        return event_type->event_type;
    }

    void Event_Manager::Add_Event(void* event_data)
    {
        m_all_events.push_back(std::pair<void*, std::chrono::time_point<std::chrono::system_clock>>(event_data, std::chrono::system_clock::now()));

        while (m_all_events.size() > m_max_events)
        {
            m_all_events.erase(m_all_events.begin());
        }

        switch (Get_Event_Type(event_data))
        {
            case Event_Type::BUTTON_PRESS:
            {
                m_button_press_events.push_back(std::pair<Button_Press_Event, std::chrono::time_point<std::chrono::system_clock>>(*(Button_Press_Event*)event_data, std::chrono::system_clock::now()));

                while (m_button_press_events.size() > m_max_events)
                {
                    m_button_press_events.erase(m_button_press_events.begin());
                }

                m_mouse_state.x_position = m_button_press_events.back().first.x_position;
                m_mouse_state.y_position = m_button_press_events.back().first.y_position;

                switch (m_button_press_events.back().first.button)
                {
                    case Button_Press_Event::Button::LEFT_BUTTON:
                    {
                        m_mouse_state.left_down = true;
                        break;
                    }
                    case Button_Press_Event::Button::MIDDLE_BUTTON:
                    {
                        m_mouse_state.middle_down = true;
                        break;
                    }
                    case Button_Press_Event::Button::RIGHT_BUTTON:
                    {
                        m_mouse_state.right_down = true;
                        break;
                    }
                }

                break;
            }
            case Event_Type::BUTTON_RELEASE:
            {
                m_button_release_events.push_back(std::pair<Button_Release_Event, std::chrono::time_point<std::chrono::system_clock>>(*(Button_Release_Event*)event_data, std::chrono::system_clock::now()));

                while (m_button_release_events.size() > m_max_events)
                {
                    m_button_release_events.erase(m_button_release_events.begin());
                }

                m_mouse_state.x_position = m_button_release_events.back().first.x_position;
                m_mouse_state.y_position = m_button_release_events.back().first.y_position;

                switch (m_button_release_events.back().first.button)
                {
                    case Button_Release_Event::Button::LEFT_BUTTON:
                    {
                        m_mouse_state.left_down = false;
                        break;
                    }
                    case Button_Release_Event::Button::MIDDLE_BUTTON:
                    {
                        m_mouse_state.middle_down = false;
                        break;
                    }
                    case Button_Release_Event::Button::RIGHT_BUTTON:
                    {
                        m_mouse_state.right_down = false;
                        break;
                    }
                }

                break;
            }
            case Event_Type::POINTER_MOTION:
            {
                m_pointer_motion_events.push_back(std::pair<Pointer_Motion_Event, std::chrono::time_point<std::chrono::system_clock>>(*(Pointer_Motion_Event*)event_data, std::chrono::system_clock::now()));

                while (m_pointer_motion_events.size() > m_max_events)
                {
                    m_pointer_motion_events.erase(m_pointer_motion_events.begin());
                }

                m_mouse_state.x_position = m_pointer_motion_events.back().first.x_position;
                m_mouse_state.y_position = m_pointer_motion_events.back().first.y_position;

                break;
            }
            default:
            {
                LOG_ERROR << "Core: Unknown event type";
                exit(EXIT_FAILURE);
            }
        }
    }

    Event_Manager::Mouse_State Event_Manager::Get_Mouse_State()
    {
        return m_mouse_state;
    }
} // namespace Cascade_Core