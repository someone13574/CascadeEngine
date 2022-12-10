#ifdef _WIN32

#include "win32_window.hpp"
#include <acorn_logging.hpp>

namespace Cascade_Core
{
    const static char window_class_name[] = "window_class";
    LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

    WIN32_Window::WIN32_Window(std::string window_title, uint32_t window_width, uint32_t window_height, Cascade_Graphics::Graphics** graphics_ptr, Engine_Thread_Manager* thread_manager_ptr, Cascade_Graphics::Graphics_Factory* m_graphics_factory_ptr)
        : Window::Window(window_title, window_width, window_height, graphics_ptr, thread_manager_ptr)
    {
        if (*m_graphics_ptr == nullptr)
        {
            *m_graphics_ptr = m_graphics_factory_ptr->Create_Graphics();
        }

        m_window_thread_ptr->Start_Thread();
        m_window_thread_ptr->Await_State(Engine_Thread::Thread_State::LOOP_FUNC);

        m_renderer_window_info_ptr = new Cascade_Graphics::WIN32_Window_Info(window_width, window_height, reinterpret_cast<void*>(&m_instance), reinterpret_cast<void*>(&m_window));
        m_renderer_ptr = m_graphics_factory_ptr->Create_Renderer(*m_graphics_ptr, m_renderer_window_info_ptr);
    }

    WIN32_Window::~WIN32_Window()
    {
    }

    void WIN32_Window::Create_Window()
    {
        LOG_INFO << "Core: Create a Win32 window with title '" << m_window_title << "' and dimensions " << m_window_width << "x" << m_window_height;

        m_instance = GetModuleHandle(0);

        static bool window_class_registered = false;
        if (!window_class_registered)
        {
            Register_Window_Class();
            window_class_registered = true;
        }

        RECT window_rect = {0, 0, (LONG)m_window_width, (LONG)m_window_height};
        AdjustWindowRect(&window_rect, m_window_style, false);

        m_window = CreateWindow(window_class_name, m_window_title.c_str(), m_window_style, CW_USEDEFAULT, CW_USEDEFAULT, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top, nullptr, nullptr, m_instance, this);
        if (!m_window)
        {
            LOG_FATAL << "Core: Failed to create Win32 window with error " << GetLastError();
            exit(EXIT_FAILURE);
        }
    }

    void WIN32_Window::Process_Events()
    {
        MSG message;
        BOOL get_message_return_value;

        if ((get_message_return_value = GetMessage(&message, NULL, 0, 0)) == 0)
        {
            m_window_thread_ptr->Exit_Thread();
        }
        else if (get_message_return_value < 0)
        {
            LOG_ERROR << "Core: Win32 window GetMessage() failed, closing window";
            m_window_thread_ptr->Exit_Thread();
        }

        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    void WIN32_Window::Destroy_Window()
    {
    }

    void WIN32_Window::Register_Window_Class()
    {
        LOG_DEBUG << "Core: Registering Win32 window class";

        WNDCLASSEX window_class;
        window_class.cbSize = sizeof(WNDCLASSEX);
        window_class.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        window_class.lpfnWndProc = WndProc;
        window_class.cbClsExtra = 0;
        window_class.cbWndExtra = 0;
        window_class.hInstance = m_instance;
        window_class.hIcon = nullptr;
        window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
        window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        window_class.lpszMenuName = nullptr;
        window_class.lpszClassName = window_class_name;
        window_class.hIconSm = nullptr;

        if (!RegisterClassEx(&window_class))
        {
            LOG_FATAL << "Core: Failed to register Win32 window class with error " << GetLastError();
            exit(EXIT_FAILURE);
        }
    }

    LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
    {
        switch (message)
        {
            case WM_CLOSE:
                PostQuitMessage(0);
                break;
            default:
                return DefWindowProc(hwnd, message, wparam, lparam);
                break;
        }

        return 0;
    }
} // namespace Cascade_Core

#endif