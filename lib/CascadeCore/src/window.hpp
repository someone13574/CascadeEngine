#pragma once

#include <string>
#include <thread>

namespace Cascade_Core
{
    class Window
    {
    private:
        enum Initialization_Stage
        {
            NOT_STARTED,
            WINDOW_CREATED,
            RENDERER_CREATED
        };

    private:
        std::string m_window_title;
        unsigned int m_width;
        unsigned int m_height;

        Initialization_Stage m_initialization_stage = Initialization_Stage::NOT_STARTED;

        bool m_threads_active = false;
        bool m_event_thread_stopped = false;
        bool m_render_thread_stopped = false;
        std::thread m_event_thread;
        std::thread m_render_thread;

    private:
        static void Event_Loop(Window* window_ptr);
        static void Render_Loop(Window* window_ptr);

    public:
        Window(std::string window_title, unsigned int width, unsigned int height);

    public:
        void Close_Window();

        bool Is_Window_Closed();
    };
} // namespace Cascade_Core