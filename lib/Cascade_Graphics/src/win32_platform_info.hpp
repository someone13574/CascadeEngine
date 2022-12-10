#pragma once

#include "platform_info.hpp"

namespace Cascade_Graphics
{
    class WIN32_Platform_Info : public Platform_Info
    {
    public:
        WIN32_Platform_Info();
        virtual ~WIN32_Platform_Info() = default;
    };

    class WIN32_Window_Info : public Window_Info
    {
    private:
        void* m_instance_ptr; // A pointer to an HINSTANCE of the target window
        void* m_window_ptr;   // A pointer to an HWND of the target window

    public:
        WIN32_Window_Info(uint32_t width, uint32_t height, void* hinstance_ptr, void* hwnd_ptr);
        virtual ~WIN32_Window_Info() = default;

        void* Get_Instance();
        void* Get_Window();
    };
} // namespace Cascade_Graphics