#include "window.hpp"

#include "cascade_logging.hpp"

#include <chrono>

namespace Cascade_Core
{
#if defined _WIN32 | defined WIN32

    static TCHAR szWindowClass[] = _T("window_class");

    LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#endif

    Window::Window(std::string window_title, unsigned int width, unsigned int height) : m_window_title(window_title), m_width(width), m_height(height)
    {
        LOG_DEBUG << "Core: Created window '" << m_window_title << "' with dimensions " << m_width << "x" << m_height;

        m_threads_active = true;
        m_event_thread = std::thread(Event_Loop, this);
        m_render_thread = std::thread(Render_Loop, this);
    }

    void Window::Initialize_Window()
    {
#ifdef __linux

        LOG_INFO << "Core: Initializing XCB window";

        if (m_initialization_stage != Initialization_Stage::NOT_STARTED)
        {
            LOG_ERROR << "Core: window '" << m_window_title << "' is at the incorrect initialization stage";
            exit(EXIT_FAILURE);
        }

        m_xcb_connection_ptr = xcb_connect(nullptr, nullptr);
        m_xcb_screen_ptr = xcb_setup_roots_iterator(xcb_get_setup(m_xcb_connection_ptr)).data;
        m_xcb_window_ptr = xcb_generate_id(m_xcb_connection_ptr);

        unsigned int mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
        unsigned int values[2] = {m_xcb_screen_ptr->white_pixel, 0};

        xcb_create_window(m_xcb_connection_ptr, XCB_COPY_FROM_PARENT, m_xcb_window_ptr, m_xcb_screen_ptr->root, 0, 0, m_width, m_height, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT, m_xcb_screen_ptr->root_visual, mask, values);
        xcb_change_property(m_xcb_connection_ptr, XCB_PROP_MODE_REPLACE, m_xcb_window_ptr, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, m_window_title.length(), const_cast<char*>(m_window_title.c_str()));

        xcb_intern_atom_cookie_t close_window_tmp_cookie = xcb_intern_atom(m_xcb_connection_ptr, 1, 12, "WM_PROTOCOLS");
        xcb_intern_atom_reply_t* close_window_tmp_reply = xcb_intern_atom_reply(m_xcb_connection_ptr, close_window_tmp_cookie, 0);
        m_xcb_close_window_cookie = xcb_intern_atom(m_xcb_connection_ptr, 0, 16, "WM_DELETE_WINDOW");
        m_xcb_close_window_reply_ptr = xcb_intern_atom_reply(m_xcb_connection_ptr, m_xcb_close_window_cookie, 0);
        xcb_change_property(m_xcb_connection_ptr, XCB_PROP_MODE_REPLACE, m_xcb_window_ptr, (*close_window_tmp_reply).atom, 4, 32, 1, &(*m_xcb_close_window_reply_ptr).atom);

        xcb_map_window(m_xcb_connection_ptr, m_xcb_window_ptr);
        xcb_flush(m_xcb_connection_ptr);

#elif defined _WIN32 || defined WIN32

        LOG_INFO << "Core: Initializing WIN32 window";

        if (m_initialization_stage != Initialization_Stage::NOT_STARTED)
        {
            LOG_ERROR << "Core: window '" << m_window_title << "' is at the incorrect initialization stage";
            exit(EXIT_FAILURE);
        }

        m_hinstance = GetModuleHandle(0);

        static bool window_class_registered = false;
        if (!window_class_registered)
        {
            LOG_TRACE << "Core: registering window class";

            WNDCLASSEX window_class;
            window_class.cbSize = sizeof(WNDCLASSEX);
            window_class.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
            window_class.lpfnWndProc = WndProc;
            window_class.cbClsExtra = 0;
            window_class.cbWndExtra = 0;
            window_class.hInstance = m_hinstance;
            window_class.hIcon = nullptr;
            window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
            window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
            window_class.lpszMenuName = nullptr;
            window_class.lpszClassName = szWindowClass;
            window_class.hIconSm = nullptr;

            if (!RegisterClassEx(&window_class))
            {
                LOG_FATAL << "Core: failed to register the window class";
                exit(EXIT_FAILURE);
            }

            window_class_registered = true;
        }

        RECT window_rect = {0, 0, (LONG)m_width, (LONG)m_height};
        AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW | WS_VISIBLE, FALSE);

        m_hwindow = CreateWindow(szWindowClass, m_window_title.c_str(), WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top, nullptr, nullptr, nullptr, nullptr);
        if (!m_hwindow)
        {
            LOG_FATAL << "Core: failed to create window";
            exit(EXIT_FAILURE);
        }

#endif

        m_initialization_stage = Initialization_Stage::WINDOW_CREATED;
    }

#if defined _WIN32 || defined WIN32

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

#endif

    void Window::Initialize_Renderer()
    {
        LOG_INFO << "Core: Initializing renderer";

        if (m_initialization_stage != Initialization_Stage::WINDOW_CREATED)
        {
            LOG_ERROR << "Core: window '" << m_window_title << "' is at the incorrect initialization stage";
            exit(EXIT_FAILURE);
        }

#ifdef __linux__

        CascadeGraphics::Vulkan::Surface::Window_Data window_data = {};
        window_data.connection_ptr = m_xcb_connection_ptr;
        window_data.window_ptr = &m_xcb_window_ptr;

        m_renderer_ptr = std::make_shared<Renderer>(window_data, m_width, m_height);

#elif defined _WIN32 || defined WIN32

#endif

        m_initialization_stage = Initialization_Stage::RENDERER_CREATED;
    }

    void Window::Event_Loop(Window* window_ptr)
    {
        LOG_DEBUG << "Core: Started event loop for window '" << window_ptr->m_window_title << "'";

        window_ptr->Initialize_Window();

        while (window_ptr->m_threads_active)
        {
#ifdef __linux__

            window_ptr->m_xcb_event_ptr = xcb_wait_for_event(window_ptr->m_xcb_connection_ptr);

            switch (window_ptr->m_xcb_event_ptr->response_type & (~0x80))
            {
                case XCB_CLIENT_MESSAGE:
                {
                    if ((*(xcb_client_message_event_t*)window_ptr->m_xcb_event_ptr).data.data32[0] == (*window_ptr->m_xcb_close_window_reply_ptr).atom)
                    {
                        LOG_INFO << "Core: Received close window event in window '" << window_ptr->m_window_title << "'";
                        window_ptr->m_requesting_close = true;
                        window_ptr->m_threads_active = false;
                    }
                    break;
                }
            }

#elif defined _WIN32 || defined WIN32

            MSG message;
            BOOL get_message_return_value;

            if ((get_message_return_value = GetMessage(&message, nullptr, 0, 0)) == 0)
            {
                window_ptr->m_requesting_close = true;
                window_ptr->m_threads_active = false;
            }
            else if (get_message_return_value == -1)
            {
                LOG_FATAL << "Core: GetMessage() failed";
                exit(EXIT_FAILURE);
            }

            TranslateMessage(&message);
            DispatchMessage(&message);

#endif
        }

        window_ptr->m_event_thread_stopped = true;

        LOG_DEBUG << "Core: Event loop stopped for window '" << window_ptr->m_window_title << "'";
    }

    void Window::Render_Loop(Window* window_ptr)
    {
        LOG_DEBUG << "Core: Started render loop for window '" << window_ptr->m_window_title << "'";

        while (window_ptr->m_initialization_stage != Initialization_Stage::WINDOW_CREATED)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        // window_ptr->Initialize_Renderer();

        while (window_ptr->m_threads_active)
        {
            // window_ptr->m_renderer_ptr->Render_Frame();
        }

        window_ptr->m_render_thread_stopped = true;

        LOG_DEBUG << "Core: Render loop stopped for window '" << window_ptr->m_window_title << "'";
    }

    void Window::Close_Window()
    {
        LOG_INFO << "Core: Closing window '" << m_window_title << "'";

        m_requesting_close = false;
        m_threads_active = false;

        m_event_thread.join();
        m_render_thread.join();

#ifdef __linux__

        switch (m_initialization_stage)
        {
            case Initialization_Stage::WINDOW_CREATED:
            {
                free(m_xcb_event_ptr);
                xcb_destroy_window(m_xcb_connection_ptr, m_xcb_window_ptr);
                xcb_disconnect(m_xcb_connection_ptr);
                m_initialization_stage = Initialization_Stage::CLEANED_UP;
                break;
            }
            case Initialization_Stage::RENDERER_CREATED:
            {
                m_renderer_ptr.reset();

                free(m_xcb_event_ptr);
                xcb_destroy_window(m_xcb_connection_ptr, m_xcb_window_ptr);
                xcb_disconnect(m_xcb_connection_ptr);
                m_initialization_stage = Initialization_Stage::CLEANED_UP;
                break;
            }
        }

#elif defined _WIN32 || defined WIN32


#endif

        LOG_INFO << "Core: Finished closing window";
    }

    bool Window::Is_Window_Closed()
    {
        return m_event_thread_stopped && m_render_thread_stopped;
    }

    bool Window::Is_Requesting_Close()
    {
        return m_requesting_close;
    }
} // namespace Cascade_Core