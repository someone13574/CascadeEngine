#pragma once

#include "engine_thread_manager.hpp"
#include "window.hpp"
#include <string>
#include <vector>

namespace Cascade_Core
{
    class Application_Factory;

    class Application
    {
    private:
        std::string m_application_name;
        uint32_t m_application_major_version;
        uint32_t m_application_minor_version;
        uint32_t m_application_patch_version;

        Engine_Thread_Manager m_engine_thread_manager;
        std::vector<Window*> m_window_ptrs;

    private:
        friend class Application_Factory;
        Application();
        ~Application();

    public:
        void Set_Application_Details(std::string application_name, uint32_t application_major_version, uint32_t application_minor_version, uint32_t application_patch_version);
        Window* Create_Window(std::string window_title, uint32_t window_width, uint32_t window_height);
    };
} // namespace Cascade_Core