#ifdef _WIN32

#pragma once

#include "engine_thread_manager.hpp"
#include "window.hpp"
#include <Windows.h>

namespace Cascade_Core
{
    class Win32_Window : public Window
    {
        friend class Win32_Window_Factory;

    private:
        const DWORD m_window_style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

        HWND m_window;
        HINSTANCE m_instance;

    private:
        Win32_Window(std::string window_title, uint32_t window_width, uint32_t window_height, Engine_Thread_Manager* thread_manager_ptr);
        void Register_Window_Class();

    public:
        ~Win32_Window();

        void Process_Events() const override;
    };

    class Win32_Window_Factory : public Window_Factory
    {
    public:
        Window* Create_Window(std::string window_title, uint32_t window_width, uint32_t window_height, Engine_Thread_Manager* thread_manager_ptr) const override;
    };
} // namespace Cascade_Core

#endif