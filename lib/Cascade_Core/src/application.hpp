#pragma once

#include "window.hpp"

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace Cascade_Core
{
    class Application
    {
    public:
        struct Application_Info
        {
            std::string title;
            unsigned int major_version;
            unsigned int minor_version;
        };

    private:
        Application_Info m_application_info;
        std::chrono::high_resolution_clock::time_point m_application_start_time;
        std::vector<std::shared_ptr<Window>> m_window_ptrs;

    private:
        void Wait_For_Window_Initialization();

    public:
        Application(Application_Info application_info);
        ~Application();

    public:
        std::shared_ptr<Window> Create_Window(std::string window_title, unsigned int width, unsigned int height);

        void Wait_For_Windows_To_Exit();
        void Run_Program_Loop(std::function<void(Application*)> function_to_run, unsigned int repetitions_per_second);

        std::chrono::milliseconds Get_Elapsed_Time();
    };

} // namespace Cascade_Core