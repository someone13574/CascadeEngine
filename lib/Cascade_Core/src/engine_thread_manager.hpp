#pragma once

#include "engine_thread.hpp"
#include <condition_variable>
#include <mutex>
#include <string>
#include <vector>

namespace Cascade_Core
{
    class Engine_Thread_Manager
    {
    private:
        std::vector<Engine_Thread*> m_engine_thread_ptrs;

        uint32_t m_finished_thread_count = 0;
        std::mutex m_thread_finished_notify_mutex;
        std::condition_variable m_thread_finished_notify;

    public:
        Engine_Thread_Manager();
        ~Engine_Thread_Manager();

    public:
        Engine_Thread* Create_Engine_Thread(std::string label, void* user_data_ptr);

        void Wait_For_Threads_To_Finish();
    };
} // namespace Cascade_Core