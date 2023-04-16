#include "window.hpp"

#include <acorn_logging.hpp>

namespace Cascade_Core
{
    Window::Window(std::string window_title, uint32_t window_width, uint32_t window_height, Cascade_Graphics::Graphics** graphics_ptr, Thread_Manager* thread_manager_ptr) :
        m_window_title(window_title), m_window_width(window_width), m_window_height(window_height), m_graphics_ptr(graphics_ptr), m_thread_manager_ptr(thread_manager_ptr)
    {
        std::string window_thread_name = "window-thread-" + window_title;
        m_window_thread_ptr = m_thread_manager_ptr->Create_Thread(window_thread_name, (void*)this);
        m_window_thread_ptr->Attach_Start_Function(Window_Thread_Start_Function);
        m_window_thread_ptr->Attach_Loop_Function(Window_Thread_Loop_Function, -1.0);
        m_window_thread_ptr->Attach_Exit_Function(Window_Thread_Exit_Function);

        std::string rendering_thread_name = "rendering-thread-" + window_title;
        m_rendering_thread_ptr = m_thread_manager_ptr->Create_Thread(rendering_thread_name, &m_renderer_ptr);
        m_rendering_thread_ptr->Attach_Loop_Function(Rendering_Thread_Loop_Function, -1.0);
    }

    Window::~Window()
    {
    }

    void Window::Window_Thread_Start_Function(Thread* window_thread_ptr, void* window_void_ptr)
    {
        (void)window_thread_ptr;

        Window* window_ptr = (Window*)window_void_ptr;
        window_ptr->Create_Window();
    }

    void Window::Window_Thread_Loop_Function(Thread* window_thread_ptr, void* window_void_ptr)
    {
        (void)window_thread_ptr;

        Window* window_ptr = (Window*)window_void_ptr;
        window_ptr->Process_Events();
    }

    void Window::Window_Thread_Exit_Function(Thread* window_thread_ptr, void* window_void_ptr)
    {
        (void)window_thread_ptr;

        Window* window_ptr = (Window*)window_void_ptr;
        window_ptr->Destroy_Window();

        delete window_ptr->m_renderer_ptr;
    }

    void Window::Rendering_Thread_Loop_Function(Thread* rendering_thread_ptr, void* renderer_void_ptr_ptr)
    {
        (void)rendering_thread_ptr;

        Cascade_Graphics::Renderer* renderer_ptr = *(Cascade_Graphics::Renderer**)renderer_void_ptr_ptr;
        renderer_ptr->Render_Frame();
    }
}    // namespace Cascade_Core