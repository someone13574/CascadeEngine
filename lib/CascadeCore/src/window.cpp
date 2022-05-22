#include "window.hpp"

#include "cascade_logging.hpp"

namespace Cascade_Core
{
    Window::Window(std::string window_title, unsigned int width, unsigned int height) : m_window_title(window_title), m_width(width), m_height(height)
    {
        LOG_DEBUG << "Core: Created window '" << m_window_title << "' with dimensions " << m_width << "x" << m_height;

        m_threads_active = true;
        m_event_thread = std::thread(Event_Loop, this);
        m_render_thread = std::thread(Render_Loop, this);
    }

    void Window::Event_Loop(Window* window_ptr)
    {
        LOG_DEBUG << "Core: Started event loop for window '" << window_ptr->m_window_title << "'";

        while (window_ptr->m_threads_active)
        {
        }

        window_ptr->m_event_thread_stopped = true;

        LOG_DEBUG << "Core: Event loop stopped for window '" << window_ptr->m_window_title << "'";
    }

    void Window::Render_Loop(Window* window_ptr)
    {
        LOG_DEBUG << "Core: Started render loop for window '" << window_ptr->m_window_title << "'";

        while (window_ptr->m_threads_active)
        {
        }

        window_ptr->m_render_thread_stopped = true;

        LOG_DEBUG << "Core: Render loop stopped for window '" << window_ptr->m_window_title << "'";
    }

    void Window::Close_Window()
    {
        LOG_INFO << "Core: Closing window '" << m_window_title << "'";

        m_threads_active = false;

        m_event_thread.join();
        m_render_thread.join();

        LOG_INFO << "Core: Finished closing window";
    }

    bool Window::Is_Window_Closed()
    {
        return m_event_thread_stopped && m_render_thread_stopped;
    }
} // namespace Cascade_Core