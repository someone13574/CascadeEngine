#pragma once

#include "window.hpp"

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
        std::vector<std::shared_ptr<Window>> m_window_ptrs;

    public:
        Application(Application_Info application_info);
        ~Application();

    public:
        std::shared_ptr<Window> Create_Window(std::string window_title, unsigned int width, unsigned int height);

        void Wait_For_Windows_To_Exit();
    };

} // namespace Cascade_Core