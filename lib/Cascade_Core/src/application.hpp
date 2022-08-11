#pragma once

#include "cascade_graphics.hpp"
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
            uint32_t major_version;
            uint32_t minor_version;
        };

    private:
        Application_Info m_application_info;
        std::chrono::high_resolution_clock::time_point m_application_start_time;

        std::shared_ptr<Cascade_Graphics::Vulkan_Backend::Vulkan_Graphics> m_graphics;

        std::vector<std::shared_ptr<Window>> m_window_ptrs;

    private:
        void Wait_For_Window_Initialization();

    public:
        Application(Application_Info application_info);
        ~Application();

    public:
        std::shared_ptr<Window> Create_Window(std::string window_title, uint32_t width, uint32_t height);

        void Wait_For_Windows_To_Exit();
        void Run_Program_Loop(std::function<void(Application*)> function_to_run, uint32_t repetitions_per_second);

        std::chrono::milliseconds Get_Elapsed_Time();
    };

} // namespace Cascade_Core