#pragma once

#include "thread_manager.hpp"
#include "window_factory.hpp"
#include <cascade_graphics.hpp>
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

        Thread_Manager* m_thread_manager_ptr;
        Cascade_Graphics::Graphics* m_graphics_ptr = nullptr;

        std::vector<Window*> m_window_ptrs;

        Window_Factory* m_window_factory_ptr = nullptr;
        Cascade_Graphics::Graphics_Factory* m_graphics_factory_ptr = nullptr;

    private:
        friend class Application_Factory;
        Application();
        ~Application();

    public:
        void Set_Application_Details(std::string application_name, uint32_t application_major_version, uint32_t application_minor_version, uint32_t application_patch_version);
        Window_Factory* Get_Window_Factory();
    };
}    // namespace Cascade_Core