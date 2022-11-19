#ifdef _WIN32

#include "win32_window.hpp"
#include <acorn_logging.hpp>

namespace Cascade_Core
{
    const static char window_class_name[] = "window_class";
    LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

    Win32_Window::Win32_Window(std::string window_title, uint32_t window_width, uint32_t window_height, Engine_Thread_Manager* thread_manager_ptr) : Window::Window(window_title, window_width, window_height, thread_manager_ptr)
    {
        LOG_INFO << "Core: Create a Win32 window with title '" << m_window_title << "' and dimensions " << m_window_width << "x" << m_window_height;

        m_instance = GetModuleHandle(0);

        static bool window_class_registered = false;
        if (!window_class_registered)
        {
            Register_Window_Class();
            window_class_registered = true;
        }

        RECT window_rect = {0, 0, (LONG)window_width, (LONG)window_height};
        AdjustWindowRect(&window_rect, m_window_style, false);

        m_window = CreateWindow(window_class_name, window_title.c_str(), m_window_style, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, m_instance, this);
        if (m_window == NULL)
        {
            LOG_FATAL << "Core: Failed to create Win32 window with error " << GetLastError();
            exit(EXIT_FAILURE);
        }
    }

    Win32_Window::~Win32_Window()
    {
    }

    void Win32_Window::Register_Window_Class()
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
        return DefWindowProc(hwnd, message, wparam, lparam);
        ;
    }

    void Win32_Window::Process_Events() const
    {
    }

    Window* Win32_Window_Factory::Create_Window(std::string window_title, uint32_t window_width, uint32_t window_height, Engine_Thread_Manager* thread_manager_ptr) const
    {
        return new Win32_Window(window_title, window_width, window_height, thread_manager_ptr);
    }
} // namespace Cascade_Core

#endif