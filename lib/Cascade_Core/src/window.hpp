#pragma once

#include "engine_thread_manager.hpp"
#include <string>

namespace Cascade_Core
{
    class Window
    {
    protected:
        std::string m_window_title;
        uint32_t m_window_width;
        uint32_t m_window_height;

        Engine_Thread* m_window_thread_ptr;
        Engine_Thread_Manager* m_thread_manager_ptr;

    protected:
        static void Thread_Start_Function(Engine_Thread* window_thread_ptr, void* window_void_ptr);
        static void Thread_Loop_Function(Engine_Thread* window_thread_ptr, void* window_void_ptr);
        static void Thread_Exit_Function(Engine_Thread* window_thread_ptr, void* window_void_ptr);

        virtual void Create_Window() = 0;
        virtual void Process_Events() = 0;
        virtual void Destroy_Window() = 0;

    public:
        Window(std::string window_title, uint32_t window_width, uint32_t window_height, Engine_Thread_Manager* thread_manager_ptr);
        virtual ~Window();
    };

    class Window_Factory
    {
    public:
        virtual ~Window_Factory();
        virtual Window* Create_Window(std::string window_title, uint32_t window_width, uint32_t window_height, Engine_Thread_Manager* thread_manager_ptr) const = 0;
    };
} // namespace Cascade_Core