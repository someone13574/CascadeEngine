#pragma once

#include "engine_thread_manager.hpp"
#include <acorn_logging.hpp>
#include <string>

namespace Cascade_Core
{
    class Window
    {
    protected:
        std::string m_window_title;
        uint32_t m_window_width;
        uint32_t m_window_height;


        Engine_Thread* m_event_thread_ptr;
        Engine_Thread_Manager* m_thread_manager_ptr;

    public:
        Window(std::string window_title, uint32_t window_width, uint32_t window_height, Engine_Thread_Manager* thread_manager_ptr)
            : m_window_title(window_title), m_window_width(window_width), m_window_height(window_height), m_thread_manager_ptr(thread_manager_ptr)
        {
            std::string event_thread_name = "window-event-thread-" + window_title;

            m_event_thread_ptr = m_thread_manager_ptr->Create_Engine_Thread(event_thread_name, (void*)this);
            m_event_thread_ptr->Attach_Loop_Function(Event_Processing_Loop_Function, 1.0);
            m_event_thread_ptr->Start_Thread();
        }

        virtual ~Window(){};

    public:
        static void Event_Processing_Loop_Function(Cascade_Core::Engine_Thread* engine_thread_ptr, void* user_data_ptr)
        {
            Window* window_instance = (Window*)user_data_ptr;
            window_instance->Process_Events();
        }

        virtual void Process_Events() const = 0;
    };

    class Window_Factory
    {
    public:
        virtual ~Window_Factory(){};
        virtual Window* Create_Window(std::string window_title, uint32_t window_width, uint32_t window_height, Engine_Thread_Manager* thread_manager_ptr) const = 0;
    };
} // namespace Cascade_Core