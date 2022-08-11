#include "window.hpp"

#include "cascade_logging.hpp"

#include <chrono>

namespace Cascade_Core
{
#if defined _WIN32 | defined WIN32

    static TCHAR szWindowClass[] = _T("window_class");

    LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#endif

    Window::Window(std::string window_title, uint32_t width, uint32_t height, std::shared_ptr<Cascade_Graphics::Vulkan_Backend::Vulkan_Graphics> graphics_ptr)
        : m_window_title(window_title), m_width(width), m_height(height), m_graphics_ptr(graphics_ptr)
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
        m_xcb_window = xcb_generate_id(m_xcb_connection_ptr);

        uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
        uint32_t values[2] = {m_xcb_screen_ptr->white_pixel, 0};

        xcb_create_window(m_xcb_connection_ptr, XCB_COPY_FROM_PARENT, m_xcb_window, m_xcb_screen_ptr->root, 0, 0, m_width, m_height, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT, m_xcb_screen_ptr->root_visual, mask, values);
        xcb_change_property(m_xcb_connection_ptr, XCB_PROP_MODE_REPLACE, m_xcb_window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, m_window_title.length(), const_cast<char*>(m_window_title.c_str()));

        xcb_intern_atom_cookie_t close_window_tmp_cookie = xcb_intern_atom(m_xcb_connection_ptr, 1, 12, "WM_PROTOCOLS");
        xcb_intern_atom_reply_t* close_window_tmp_reply = xcb_intern_atom_reply(m_xcb_connection_ptr, close_window_tmp_cookie, 0);
        m_xcb_close_window_cookie = xcb_intern_atom(m_xcb_connection_ptr, 0, 16, "WM_DELETE_WINDOW");
        m_xcb_close_window_reply_ptr = xcb_intern_atom_reply(m_xcb_connection_ptr, m_xcb_close_window_cookie, 0);
        xcb_change_property(m_xcb_connection_ptr, XCB_PROP_MODE_REPLACE, m_xcb_window, (*close_window_tmp_reply).atom, 4, 32, 1, &(*m_xcb_close_window_reply_ptr).atom);

        uint32_t enabled_events[] = {XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE
                                     | XCB_EVENT_MASK_STRUCTURE_NOTIFY};
        xcb_change_window_attributes(m_xcb_connection_ptr, m_xcb_window, XCB_CW_EVENT_MASK, enabled_events);

        xcb_map_window(m_xcb_connection_ptr, m_xcb_window);
        xcb_flush(m_xcb_connection_ptr);

#elif defined _WIN32 || defined WIN32

        LOG_INFO << "Core: Initializing WIN32 window";

        if (m_initialization_stage != Initialization_Stage::NOT_STARTED)
        {
            LOG_ERROR << "Core: Window '" << m_window_title << "' is at the incorrect initialization stage";
            exit(EXIT_FAILURE);
        }

        m_hinstance = GetModuleHandle(0);

        static bool window_class_registered = false;
        if (!window_class_registered)
        {
            LOG_TRACE << "Core: Registering window class";

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
                LOG_FATAL << "Core: Failed to register the window class";
                exit(EXIT_FAILURE);
            }

            window_class_registered = true;
        }

        RECT window_rect = {0, 0, (LONG)m_width, (LONG)m_height};
        AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW | WS_VISIBLE, FALSE);

        m_hwindow = CreateWindow(szWindowClass, m_window_title.c_str(), WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top, nullptr, nullptr, m_hinstance, this);
        if (!m_hwindow)
        {
            LOG_FATAL << "Core: Failed to create window";
            exit(EXIT_FAILURE);
        }

#endif

        m_event_manager_ptr = std::make_shared<Event_Manager>(1024);

        m_initialization_stage = Initialization_Stage::WINDOW_CREATED;
    }

#if defined _WIN32 || defined WIN32

    LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
    {
        Window* window_ptr;
        window_ptr = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

        switch (message)
        {
            case WM_CREATE:
            {
                CREATESTRUCT* create_struct = (CREATESTRUCT*)lparam;
                Window* window = (Window*)create_struct->lpCreateParams;
                SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
                break;
            }
            case WM_CLOSE:
            {
                PostQuitMessage(0);
                break;
            }
            case WM_LBUTTONDOWN:
            {
                Event_Manager::Button_Press_Event event_data = {};
                event_data.event_type = Event_Manager::Event_Type::BUTTON_PRESS;
                event_data.x_position = GET_X_LPARAM(lparam);
                event_data.y_position = GET_Y_LPARAM(lparam);
                event_data.button = Event_Manager::Button_Press_Event::LEFT_BUTTON;

                window_ptr->Get_Event_Manager()->Add_Event(&event_data);

                break;
            }
            case WM_MBUTTONDOWN:
            {
                Event_Manager::Button_Press_Event event_data = {};
                event_data.event_type = Event_Manager::Event_Type::BUTTON_PRESS;
                event_data.x_position = GET_X_LPARAM(lparam);
                event_data.y_position = GET_Y_LPARAM(lparam);
                event_data.button = Event_Manager::Button_Press_Event::MIDDLE_BUTTON;

                window_ptr->Get_Event_Manager()->Add_Event(&event_data);

                break;
            }
            case WM_RBUTTONDOWN:
            {
                Event_Manager::Button_Press_Event event_data = {};
                event_data.event_type = Event_Manager::Event_Type::BUTTON_PRESS;
                event_data.x_position = GET_X_LPARAM(lparam);
                event_data.y_position = GET_Y_LPARAM(lparam);
                event_data.button = Event_Manager::Button_Press_Event::RIGHT_BUTTON;

                window_ptr->Get_Event_Manager()->Add_Event(&event_data);

                break;
            }
            case WM_LBUTTONUP:
            {
                Event_Manager::Button_Release_Event event_data = {};
                event_data.event_type = Event_Manager::Event_Type::BUTTON_RELEASE;
                event_data.x_position = GET_X_LPARAM(lparam);
                event_data.y_position = GET_Y_LPARAM(lparam);
                event_data.button = Event_Manager::Button_Release_Event::LEFT_BUTTON;

                window_ptr->Get_Event_Manager()->Add_Event(&event_data);

                break;
            }
            case WM_MBUTTONUP:
            {
                Event_Manager::Button_Release_Event event_data = {};
                event_data.event_type = Event_Manager::Event_Type::BUTTON_RELEASE;
                event_data.x_position = GET_X_LPARAM(lparam);
                event_data.y_position = GET_Y_LPARAM(lparam);
                event_data.button = Event_Manager::Button_Release_Event::MIDDLE_BUTTON;

                window_ptr->Get_Event_Manager()->Add_Event(&event_data);

                break;
            }
            case WM_RBUTTONUP:
            {
                Event_Manager::Button_Release_Event event_data = {};
                event_data.event_type = Event_Manager::Event_Type::BUTTON_RELEASE;
                event_data.x_position = GET_X_LPARAM(lparam);
                event_data.y_position = GET_Y_LPARAM(lparam);
                event_data.button = Event_Manager::Button_Release_Event::RIGHT_BUTTON;

                window_ptr->Get_Event_Manager()->Add_Event(&event_data);

                break;
            }
            case WM_MOUSEMOVE:
            {
                Event_Manager::Pointer_Motion_Event event_data = {};
                event_data.event_type = Event_Manager::Event_Type::POINTER_MOTION;
                event_data.x_position = GET_X_LPARAM(lparam);
                event_data.y_position = GET_Y_LPARAM(lparam);

                window_ptr->Get_Event_Manager()->Add_Event(&event_data);

                break;
            }
            case WM_SIZE:
            {
                RECT client_rectangle;
                BOOL get_client_rectangle_result = GetClientRect(hwnd, &client_rectangle);

                if (get_client_rectangle_result == 0)
                {
                    LOG_ERROR << "Core: Failed to get window dimensions with code " << GetLastError();
                }

                window_ptr->Update_Size(client_rectangle.right - client_rectangle.left, client_rectangle.bottom - client_rectangle.top);

                break;
            }
            default:
            {
                return DefWindowProc(hwnd, message, wparam, lparam);
                break;
            }
        }

        return 0;
    }

#endif

    void Window::Initialize_Renderer()
    {
        LOG_INFO << "Core: Initializing renderer";

        if (m_initialization_stage != Initialization_Stage::WINDOW_CREATED)
        {
            LOG_ERROR << "Core: Window '" << m_window_title << "' is at the incorrect initialization stage";
            exit(EXIT_FAILURE);
        }

#ifdef __linux__

        Cascade_Graphics::Window_Information window_information = {};
        window_information.width_ptr = &m_width;
        window_information.height_ptr = &m_height;
        window_information.xcb_window_ptr = &m_xcb_window;
        window_information.xcb_connection_ptr = m_xcb_connection_ptr;

        m_renderer_ptr = std::make_shared<Cascade_Graphics::Renderer>(m_graphics_ptr, window_information);

#elif defined _WIN32 || defined WIN32

        Cascade_Graphics::Window_Information window_information = {};
        window_information.width_ptr = &m_width;
        window_information.height_ptr = &m_height;
        window_information.hwindow_ptr = &m_hwindow;
        window_information.hinstance_ptr = &m_hinstance;

        m_renderer_ptr = std::make_shared<Cascade_Graphics::Renderer>(m_graphics_ptr, window_information);

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
                case XCB_BUTTON_PRESS:
                {
                    xcb_button_press_event_t* button_press_event = (xcb_button_press_event_t*)window_ptr->m_xcb_event_ptr;

                    Event_Manager::Button_Press_Event event_data = {};
                    event_data.event_type = Event_Manager::Event_Type::BUTTON_PRESS;
                    event_data.x_position = button_press_event->event_x;
                    event_data.y_position = button_press_event->event_y;

                    switch ((uint32_t)button_press_event->detail)
                    {
                        case 1:
                        {
                            event_data.button = Event_Manager::Button_Press_Event::Button::LEFT_BUTTON;
                            window_ptr->m_event_manager_ptr->Add_Event(&event_data);
                            break;
                        }
                        case 2:
                        {
                            event_data.button = Event_Manager::Button_Press_Event::Button::MIDDLE_BUTTON;
                            window_ptr->m_event_manager_ptr->Add_Event(&event_data);
                            break;
                        }
                        case 3:
                        {
                            event_data.button = Event_Manager::Button_Press_Event::Button::RIGHT_BUTTON;
                            window_ptr->m_event_manager_ptr->Add_Event(&event_data);
                            break;
                        }
                    }

                    break;
                }
                case XCB_BUTTON_RELEASE:
                {
                    xcb_button_release_event_t* button_release_event = (xcb_button_release_event_t*)window_ptr->m_xcb_event_ptr;

                    Event_Manager::Button_Release_Event event_data = {};
                    event_data.event_type = Event_Manager::Event_Type::BUTTON_RELEASE;
                    event_data.x_position = button_release_event->event_x;
                    event_data.y_position = button_release_event->event_y;

                    switch ((uint32_t)button_release_event->detail)
                    {
                        case 1:
                        {
                            event_data.button = Event_Manager::Button_Release_Event::Button::LEFT_BUTTON;
                            window_ptr->m_event_manager_ptr->Add_Event(&event_data);
                            break;
                        }
                        case 2:
                        {
                            event_data.button = Event_Manager::Button_Release_Event::Button::MIDDLE_BUTTON;
                            window_ptr->m_event_manager_ptr->Add_Event(&event_data);
                            break;
                        }
                        case 3:
                        {
                            event_data.button = Event_Manager::Button_Release_Event::Button::RIGHT_BUTTON;
                            window_ptr->m_event_manager_ptr->Add_Event(&event_data);
                            break;
                        }
                    }

                    break;
                }
                case XCB_MOTION_NOTIFY:
                {
                    xcb_motion_notify_event_t* motion_notify_event = (xcb_motion_notify_event_t*)window_ptr->m_xcb_event_ptr;

                    Event_Manager::Pointer_Motion_Event event_data = {};
                    event_data.event_type = Event_Manager::Event_Type::POINTER_MOTION;
                    event_data.x_position = motion_notify_event->event_x;
                    event_data.y_position = motion_notify_event->event_y;

                    window_ptr->m_event_manager_ptr->Add_Event(&event_data);

                    break;
                }
                case XCB_CONFIGURE_NOTIFY:
                {
                    xcb_configure_notify_event_t* configure_notify_event = (xcb_configure_notify_event_t*)window_ptr->m_xcb_event_ptr;

                    window_ptr->m_width = configure_notify_event->width;
                    window_ptr->m_height = configure_notify_event->height;

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

        window_ptr->Initialize_Renderer();

        std::chrono::high_resolution_clock::time_point iteration_start;
        std::chrono::microseconds microseconds_per_repetition = std::chrono::microseconds((uint64_t)(1000000.0 / 120.0));

        while (window_ptr->m_threads_active)
        {
            iteration_start = std::chrono::high_resolution_clock::now();

            window_ptr->m_renderer_ptr->Render_Frame();

            std::chrono::microseconds execution_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - iteration_start);
            // std::this_thread::sleep_for(std::chrono::microseconds(std::abs((microseconds_per_repetition - execution_time).count())));
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
                xcb_destroy_window(m_xcb_connection_ptr, m_xcb_window);
                xcb_disconnect(m_xcb_connection_ptr);
                m_initialization_stage = Initialization_Stage::CLEANED_UP;
                break;
            }
            case Initialization_Stage::RENDERER_CREATED:
            {
                m_renderer_ptr.reset();

                free(m_xcb_event_ptr);
                xcb_destroy_window(m_xcb_connection_ptr, m_xcb_window);
                xcb_disconnect(m_xcb_connection_ptr);
                m_initialization_stage = Initialization_Stage::CLEANED_UP;
                break;
            }
        }
#elif defined _WIN32 || defined WIN32

        if (m_initialization_stage == Initialization_Stage::RENDERER_CREATED)
        {
            m_renderer_ptr.reset();
            m_initialization_stage = Initialization_Stage::CLEANED_UP;
        }

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

    std::pair<uint32_t, uint32_t> Window::Get_Window_Dimensions()
    {
#ifdef __linux__

        xcb_get_geometry_cookie_t get_geometry_cookie;
        xcb_get_geometry_reply_t* get_geometry_reply_ptr;

        get_geometry_cookie = xcb_get_geometry(m_xcb_connection_ptr, m_xcb_window);
        get_geometry_reply_ptr = xcb_get_geometry_reply(m_xcb_connection_ptr, get_geometry_cookie, NULL);

        if (!get_geometry_reply_ptr)
        {
            LOG_ERROR << "Core: Failed to get window dimensions";
            exit(EXIT_FAILURE);
        }

        return std::pair<uint32_t, uint32_t>(get_geometry_reply_ptr->width, get_geometry_reply_ptr->height);

#elif defined _WIN32 || defined WIN32
        if (m_initialization_stage != Initialization_Stage::NOT_STARTED)
        {
            return std::pair<uint32_t, uint32_t>(m_width, m_height);
        }
        else
        {
            return std::pair<uint32_t, uint32_t>(0, 0);
        }
#endif
    }

    void Window::Update_Size(uint32_t width, uint32_t height)
    {
        m_width = width;
        m_height = height;
    }

    Window::Initialization_Stage Window::Get_Initialization_Stage()
    {
        return m_initialization_stage;
    }

    std::shared_ptr<Event_Manager> Window::Get_Event_Manager()
    {
        return m_event_manager_ptr;
    }

    std::shared_ptr<Cascade_Graphics::Renderer> Window::Get_Renderer()
    {
        return m_renderer_ptr;
    }
} // namespace Cascade_Core