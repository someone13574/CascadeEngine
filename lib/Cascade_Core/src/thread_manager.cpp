#include "thread_manager.hpp"

#include <acorn_logging.hpp>

namespace Cascade_Core
{
    Thread_Manager::Thread_Manager()
    {
    }

    Thread_Manager::~Thread_Manager()
    {
        for (int32_t i = static_cast<int32_t>(m_thread_ptrs.size()) - 1; i >= 0; i--)
        {
            Thread* current_thread_ptr = m_thread_ptrs[i];
            m_thread_ptrs.erase(m_thread_ptrs.begin() + i);

            current_thread_ptr->m_thread.join();
            delete current_thread_ptr;
        }
    }

    Thread* Thread_Manager::Create_Thread(std::string label, void* user_data_ptr)
    {
        Thread* new_thread_ptr = new Thread(label, user_data_ptr, &m_finished_thread_count, &m_thread_finished_notify_mutex, &m_thread_finished_notify);
        m_thread_ptrs.push_back(new_thread_ptr);

        return new_thread_ptr;
    }

    void Thread_Manager::Wait_For_Threads_To_Finish()
    {
        std::unique_lock<std::mutex> thread_finished_notify_lock(m_thread_finished_notify_mutex);
        m_thread_finished_notify.wait(thread_finished_notify_lock, [this] { return m_thread_ptrs.size() == m_finished_thread_count; });

        LOG_INFO << "Core: All threads have finished";

        for (int32_t i = static_cast<int32_t>(m_thread_ptrs.size()) - 1; i >= 0; i--)
        {
            Thread* current_thread_ptr = m_thread_ptrs[i];
            m_thread_ptrs.erase(m_thread_ptrs.begin() + i);

            current_thread_ptr->m_thread.join();
            delete current_thread_ptr;
        }
    }
} // namespace Cascade_Core