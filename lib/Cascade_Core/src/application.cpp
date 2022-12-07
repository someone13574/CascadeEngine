#include "application.hpp"

#include "win32_window.hpp"
#include "xcb_window.hpp"
#include <acorn_logging.hpp>

extern void Cascade_Main(Cascade_Core::Engine_Thread* engine_thread_ptr, void* user_data_ptr);

namespace Cascade_Core
{
    Application::Application() : m_engine_thread_manager()
    {
        LOG_INFO << "Core: Initializing Cascade";

        Cascade_Graphics::Vulkan_Graphics_Factory graphics_factory = Cascade_Graphics::Vulkan_Graphics_Factory();
        m_graphics_ptr = graphics_factory.Create_Graphics();

        Engine_Thread* application_thread_ptr = m_engine_thread_manager.Create_Engine_Thread("application_cascade_main", (void*)this);
        application_thread_ptr->Attach_Start_Function(Cascade_Main);
        application_thread_ptr->Start_Thread();

        m_engine_thread_manager.Wait_For_Threads_To_Finish();
    }

    Application::~Application()
    {
        LOG_INFO << "Core: Destroying application";

        for (int32_t i = static_cast<int32_t>(m_window_ptrs.size()) - 1; i >= 0; i--)
        {
            delete m_window_ptrs[i];
            m_window_ptrs.erase(m_window_ptrs.begin() + i);
        }

        delete m_graphics_ptr;

        LOG_TRACE << "Core: Finished destroying application";
    }

    void Application::Set_Application_Details(std::string application_name, uint32_t application_major_version, uint32_t application_minor_version, uint32_t application_patch_version)
    {
        LOG_DEBUG << "Core: Setting application details to '" << application_name << "-" << application_major_version << "." << application_minor_version << "." << application_patch_version << "'";

        m_application_name = application_name;
        m_application_major_version = application_major_version;
        m_application_minor_version = application_minor_version;
        m_application_patch_version = application_patch_version;
    }

    Window* Application::Create_Window(std::string window_title, uint32_t window_width, uint32_t window_height)
    {
#ifdef __linux__
        XCB_Window_Factory window_factory = XCB_Window_Factory();
#elif defined _WIN32
        Win32_Window_Factory window_factory = Win32_Window_Factory();
#endif

        Window* window_ptr = window_factory.Create_Window(window_title, window_width, window_height, &m_engine_thread_manager, m_graphics_ptr);
        m_window_ptrs.push_back(window_ptr);

        return window_ptr;
    }
} // namespace Cascade_Core