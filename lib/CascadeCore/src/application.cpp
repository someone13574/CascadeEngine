#include "application.hpp"

#include "cascade_logging.hpp"

#include <chrono>
#include <cmath>

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
            for (unsigned int i = 0; i < m_window_ptrs.size(); i++)
            {
                if (m_window_ptrs[i]->Is_Requesting_Close())
                {
                    m_window_ptrs[i]->Close_Window();
                }
            }

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

    void Application::Run_Program_Loop(std::function<void()> function_to_run, unsigned int repetitions_per_second)
    {
        std::chrono::high_resolution_clock::time_point iteration_start;
        std::chrono::microseconds microseconds_per_repetition = std::chrono::microseconds((uint64_t)(1000000.0 / (double)repetitions_per_second));

        while (true)
        {
            iteration_start = std::chrono::high_resolution_clock::now();

            for (unsigned int i = 0; i < m_window_ptrs.size(); i++)
            {
                if (m_window_ptrs[i]->Is_Requesting_Close())
                {
                    m_window_ptrs[i]->Close_Window();
                }
            }

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

            function_to_run();

            std::chrono::microseconds execution_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - iteration_start);
            std::this_thread::sleep_for(std::chrono::microseconds(std::abs((microseconds_per_repetition - execution_time).count())));
        }
    }
} // namespace Cascade_Core