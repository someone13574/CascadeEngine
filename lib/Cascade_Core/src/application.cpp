#include "application.hpp"

#include "xcb_window.hpp"
#include <acorn_logging.hpp>

extern void Cascade_Main(Cascade_Core::Engine_Thread* engine_thread_ptr, void* user_data_ptr);

namespace Cascade_Core
{
    Application::Application() : m_engine_thread_manager()
    {
        LOG_INFO << "Core: Initializing Cascade";

        Engine_Thread* application_thread_ptr = m_engine_thread_manager.Create_Engine_Thread("application_cascade_main", (void*)this);
        application_thread_ptr->Attach_Start_Function(Cascade_Main);
        application_thread_ptr->Start_Thread();

        m_engine_thread_manager.Wait_For_Threads_To_Finish();
    }

    Application::~Application()
    {
        LOG_INFO << "Core: Destroying application";

        for (int32_t i = m_window_ptrs.size() - 1; i >= 0; i--)
        {
            delete m_window_ptrs[i];
            m_window_ptrs.erase(m_window_ptrs.begin() + i);
        }
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
        XCB_Window_Factory xcb_window_factory = XCB_Window_Factory();
        Window* window_ptr = xcb_window_factory.Create_Window(window_title, window_width, window_height, &m_engine_thread_manager);
        m_window_ptrs.push_back(window_ptr);

        return window_ptr;
#else
        return nullptr;
#endif
    }
} // namespace Cascade_Core