#include "cascade_logging.hpp"
#include "event_manager.hpp"

namespace CascadeCore
{
    Event_Manager::Event::Event(void (*function_to_execute)(void* data), Event_Type event_type) : m_execute_function(function_to_execute), m_event_type(event_type)
    {
    }

    Event_Manager::Event_Type Event_Manager::Event::Get_Event_Type()
    {
        return m_event_type;
    }

    Event_Manager::Event_Manager(Window* window_ptr) : m_window_ptr(window_ptr)
    {
    }

    void Event_Manager::Add_Event(Event event)
    {
        LOG_TRACE << "Adding a new event";

        m_events.push_back(event);
        m_active_event_types[(unsigned int)event.Get_Event_Type()] = true;

        m_window_ptr->Update_Event_Types();
    }

    void Event_Manager::Add_Event(void (*function_to_execute)(void* data), Event_Type event_type)
    {
        Event_Manager::Event event = Event_Manager::Event(function_to_execute, event_type);

        Add_Event(event);
    }

    void Event_Manager::Execute_Window_Close_Event(Event_Manager::Window_Close_Event event_data)
    {
        for (unsigned int i = 0; i < m_events.size(); i++)
        {
            if (m_events[i].Get_Event_Type() == Event_Manager::Event_Type::WINDOW_CLOSE)
            {
                if (m_events[i].Get_Execute_Function() != nullptr)
                {
                    m_events[i].Get_Execute_Function()(&event_data);
                }
                else
                {
                    LOG_WARN << "Event execute function for type 'Window close' is a null pointer. Skipping execution.";
                }
            }
        }
    }

    void Event_Manager::Execute_Button_Press_Event(Event_Manager::Button_Press_Event event_data)
    {
        for (unsigned int i = 0; i < m_events.size(); i++)
        {
            if (m_events[i].Get_Event_Type() == Event_Manager::Event_Type::BUTTON_PRESS)
            {
                if (m_events[i].Get_Execute_Function() != nullptr)
                {
                    m_events[i].Get_Execute_Function()(&event_data);
                }
                else
                {
                    LOG_WARN << "Event execute function for type 'Button press' is a null pointer. Skipping execution.";
                }
            }
        }
    }

    void Event_Manager::Execute_Button_Release_Event(Event_Manager::Button_Release_Event event_data)
    {
        for (unsigned int i = 0; i < m_events.size(); i++)
        {
            if (m_events[i].Get_Event_Type() == Event_Manager::Event_Type::BUTTON_RELEASE)
            {
                if (m_events[i].Get_Execute_Function() != nullptr)
                {
                    m_events[i].Get_Execute_Function()(&event_data);
                }
                else
                {
                    LOG_WARN << "Event execute function for type 'Button release' is a null pointer. Skipping execution.";
                }
            }
        }
    }

    void Event_Manager::Execute_Pointer_Motion_Event(Event_Manager::Pointer_Movement_Event event_data)
    {
        for (unsigned int i = 0; i < m_events.size(); i++)
        {
            if (m_events[i].Get_Event_Type() == Event_Manager::Event_Type::POINTER_MOTION)
            {
                if (m_events[i].Get_Execute_Function() != nullptr)
                {
                    m_events[i].Get_Execute_Function()(&event_data);
                }
                else
                {
                    LOG_WARN << "Event execute function for type 'Pointer motion' is a null pointer. Skipping execution.";
                }
            }
        }
    }

    const bool* Event_Manager::Get_Enabled_Event_Types()
    {
        return m_active_event_types;
    }
} // namespace CascadeCore