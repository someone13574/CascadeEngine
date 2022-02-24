#include "application.hpp"
#include "cascade_logging.hpp"

#include <chrono>
#include <thread>

namespace CascadeCore
{
    Application::Application(std::string name, std::string version) : m_application_name(name), m_application_version(version)
    {
        LOG_INFO << "Started '" << m_application_name << "' " << m_application_version;
    }

    Application::~Application()
    {
        LOG_DEBUG << "Cleaning up '" << m_application_name << "'";

        for (unsigned int i = 0; i < m_windows.size(); i++)
        {
            if (m_event_thread_active[i])
            {
                m_event_thread_active[i] = false;
                m_windows[i]->Send_Close_Event();
                m_event_threads[i].join();
            }
            else
            {
                m_event_threads[i].join();
            }
        }

        LOG_TRACE << "Finished cleaning up '" << m_application_name << "'";
    }

    void Application::Event_Loop(Application* instance, unsigned int window_index)
    {
        LOG_DEBUG << "Starting event loop #" << window_index << " for '" << instance->m_application_name << "'";

        while (instance->m_event_thread_active[window_index])
        {
            instance->m_windows[window_index]->Process_Events();
        }

        instance->m_windows[window_index].reset();
        instance->m_active_event_threads--;

        LOG_DEBUG << "Event loop #" << window_index << " has finished for '" << instance->m_application_name << "'";
    }

    std::string Application::Get_Application_Name()
    {
        return m_application_name;
    }

    void Window::Close_Window_Event(void* data)
    {
        Event_Manager::Window_Close_Event* event_data = (struct Event_Manager::Window_Close_Event*)data;

        LOG_TRACE << "'" << event_data->window_to_close->Get_Owner_Application()->m_application_name << "' has received a close window event";

        unsigned int window_index_in_application = 0;
        for (unsigned int i = 0; i < event_data->window_to_close->Get_Owner_Application()->m_windows.size(); i++)
        {
            if (event_data->window_to_close->Get_Owner_Application()->m_windows[i].get() == event_data->window_to_close)
            {
                window_index_in_application = i;
            }
        }

        event_data->window_to_close->Get_Owner_Application()->m_event_thread_active[window_index_in_application] = false;
    }

    std::shared_ptr<Window> Application::Create_Window(unsigned int width, unsigned int height)
    {
        std::shared_ptr<Window> window_ptr = std::make_shared<Window>(width, height, m_application_name, this);

        m_windows.push_back(window_ptr);
        m_event_thread_active.push_back(true);
        m_active_event_threads++;
        m_event_threads.push_back(std::thread(Event_Loop, this, m_windows.size() - 1));

        window_ptr->Get_Event_Manager()->Add_Event(Window::Close_Window_Event, Event_Manager::Event_Type::WINDOW_CLOSE);

        return window_ptr;
    }

    void Application::Wait_For_Windows_To_Exit()
    {
        while (m_active_event_threads != 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(15));
        }
    }
} // namespace CascadeCore