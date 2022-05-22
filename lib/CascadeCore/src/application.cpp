#include "application.hpp"

#include "cascade_logging.hpp"

#include <chrono>

namespace Cascade_Core
{
    Application::Application(Application_Info application_info) : m_application_info(application_info)
    {
        LOG_DEBUG << "Core: Started application '" << m_application_info.title << "' v" << m_application_info.major_version << "." << m_application_info.minor_version;
    }

    Application::~Application()
    {
        LOG_INFO << "Core: Cleaning up application '" << m_application_info.title << "' v" << m_application_info.major_version << "." << m_application_info.minor_version;

        for (unsigned int i = 0; i < m_window_ptrs.size(); i++)
        {
            if (!m_window_ptrs[i]->Is_Window_Closed())
            {
                m_window_ptrs[i]->Close_Window();
            }
        }

        LOG_INFO << "Core: Finished cleaning up application";
    }

    std::shared_ptr<Window> Application::Create_Window(std::string window_title, unsigned int width, unsigned int height)
    {
        m_window_ptrs.push_back(std::make_shared<Window>(window_title, width, height));

        return m_window_ptrs.back();
    }

    void Application::Wait_For_Windows_To_Exit()
    {
        while (true)
        {
            bool all_windows_exited = true;
            for (unsigned int i = 0; i < m_window_ptrs.size(); i++)
            {
                if (!m_window_ptrs[i]->Is_Window_Closed())
                {
                    all_windows_exited = false;
                    break;
                }
            }

            if (all_windows_exited)
            {
                break;
            }

            std::this_thread::sleep_for(std::chrono::microseconds(16667));
        }
    }
} // namespace Cascade_Core