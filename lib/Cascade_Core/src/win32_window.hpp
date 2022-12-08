#ifdef _WIN32

#pragma once

#include "engine_thread_manager.hpp"
#include "window.hpp"
#include <windows.h>
#include <windowsx.h>

namespace Cascade_Core
{
    class WIN32_Window : public Window
    {
        friend class WIN32_Window_Factory;

    private:
        const DWORD m_window_style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

        HWND m_window;
        HINSTANCE m_instance;

    protected:
        void Create_Window() override;
        void Process_Events() override;
        void Destroy_Window() override;

        void Register_Window_Class();

    private:
        WIN32_Window(std::string window_title, uint32_t window_width, uint32_t window_height, Cascade_Graphics::Graphics* graphics_ptr, Engine_Thread_Manager* thread_manager_ptr, Cascade_Graphics::Graphics_Factory* m_graphics_factory_ptr);

    public:
        ~WIN32_Window();
    };
} // namespace Cascade_Core

#endif