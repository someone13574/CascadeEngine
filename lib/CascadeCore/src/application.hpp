#pragma once

#include "window.hpp"

#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace CascadeCore
{
    class Window;

    class Application
    {
        friend class Window;

    private:
        std::string m_application_name;
        std::string m_application_version;

        unsigned int m_active_event_threads = 0;
        std::vector<bool> m_event_thread_active;
        std::vector<std::thread> m_event_threads;

        unsigned int m_active_rendering_threads = 0;
        std::vector<bool> m_rendering_thread_active;
        std::vector<std::thread> m_rendering_threads;

        std::vector<std::shared_ptr<Window>> m_windows;

    private:
        static void Event_Loop(Application* instance, unsigned int window_index);
        static void Rendering_Loop(Application* instance, unsigned int window_index);

    public:
        Application(std::string name, std::string version);
        ~Application();

        std::string Get_Application_Name();
        std::shared_ptr<Window> Create_Window(unsigned int width, unsigned int height);

        void Wait_For_Windows_To_Exit();
    };

} // namespace CascadeCore