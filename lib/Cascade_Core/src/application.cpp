#include "application.hpp"

#include "win32_window_factory.hpp"
#include "xcb_window_factory.hpp"
#include <acorn_logging.hpp>

extern void Cascade_Main(Cascade_Core::Engine_Thread* engine_thread_ptr, void* user_data_ptr);

namespace Cascade_Core
{
    Application::Application()
    {
        LOG_INFO << "Core: Initializing Cascade";

        m_engine_thread_manager_ptr = new Engine_Thread_Manager();

#ifdef CSD_GRAPHICS_VULKAN
#ifdef __linux__
        m_graphics_factory_ptr = new Cascade_Graphics::Vulkan_Graphics_Factory(Cascade_Graphics::Graphics_Platform::LINUX_XCB);
#elif defined _WIN32
        m_graphics_factory_ptr = new Cascade_Graphics::Vulkan_Graphics_Factory(Cascade_Graphics::Graphics_Platform::WINDOWS_WIN32);
#endif
#else
#error "No graphics backend selected, define backend with CSD_BACKEND_{backend}"
#endif

#ifdef __linux__
        m_window_factory_ptr = new XCB_Window_Factory(&m_graphics_ptr, m_engine_thread_manager_ptr, m_graphics_factory_ptr);
#elif defined _WIN32
        m_window_factory_ptr = new WIN32_Window_Factory(&m_graphics_ptr, m_engine_thread_manager_ptr, m_graphics_factory_ptr);
#endif

        Engine_Thread* application_thread_ptr = m_engine_thread_manager_ptr->Create_Engine_Thread("application_cascade_main", (void*)this);
        application_thread_ptr->Attach_Start_Function(Cascade_Main);
        application_thread_ptr->Start_Thread();

        m_engine_thread_manager_ptr->Wait_For_Threads_To_Finish();
    }

    Application::~Application()
    {
        LOG_INFO << "Core: Destroying application";

        for (int32_t i = static_cast<int32_t>(m_window_ptrs.size()) - 1; i >= 0; i--)
        {
            delete m_window_ptrs[i];
            m_window_ptrs.erase(m_window_ptrs.begin() + i);
        }

        delete m_window_factory_ptr;
        delete m_graphics_factory_ptr;
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

    Window_Factory* Application::Get_Window_Factory()
    {
        return m_window_factory_ptr;
    }
} // namespace Cascade_Core