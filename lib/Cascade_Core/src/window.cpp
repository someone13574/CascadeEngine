#include "window.hpp"

#include <acorn_logging.hpp>

namespace Cascade_Core
{
    Window::Window(std::string window_title, uint32_t window_width, uint32_t window_height, Engine_Thread_Manager* thread_manager_ptr, Cascade_Graphics::Graphics* graphics_ptr)
        : m_window_title(window_title), m_window_width(window_width), m_window_height(window_height), m_thread_manager_ptr(thread_manager_ptr), m_graphics_ptr(graphics_ptr)
    {
        std::string window_thread_name = "window-thread-" + window_title;
        m_window_thread_ptr = m_thread_manager_ptr->Create_Engine_Thread(window_thread_name, (void*)this);
        m_window_thread_ptr->Attach_Start_Function(Thread_Start_Function);
        m_window_thread_ptr->Attach_Loop_Function(Thread_Loop_Function, -1.0);
        m_window_thread_ptr->Attach_Exit_Function(Thread_Exit_Function);
        m_window_thread_ptr->Start_Thread();

        Cascade_Graphics::Vulkan_Graphics_Factory graphics_factory = Cascade_Graphics::Vulkan_Graphics_Factory();
        m_renderer_ptr = graphics_factory.Create_Renderer(m_graphics_ptr);
    }

    Window::~Window()
    {
    }

    void Window::Thread_Start_Function(Engine_Thread* window_thread_ptr, void* window_void_ptr)
    {
        (void)window_thread_ptr;

        Window* window_ptr = (Window*)window_void_ptr;
        window_ptr->Create_Window();
    }

    void Window::Thread_Loop_Function(Engine_Thread* window_thread_ptr, void* window_void_ptr)
    {
        (void)window_thread_ptr;

        Window* window_ptr = (Window*)window_void_ptr;
        window_ptr->Process_Events();
    }

    void Window::Thread_Exit_Function(Engine_Thread* window_thread_ptr, void* window_void_ptr)
    {
        (void)window_thread_ptr;

        Window* window_ptr = (Window*)window_void_ptr;
        window_ptr->Destroy_Window();

        delete window_ptr->m_renderer_ptr;
    }

    Window_Factory::~Window_Factory()
    {
    }
} // namespace Cascade_Core