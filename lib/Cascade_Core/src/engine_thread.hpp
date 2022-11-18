#pragma once

#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>
#include <thread>

namespace Cascade_Core
{
    class Engine_Thread
    {
        friend class Engine_Thread_Manager;

    private:
        enum Thread_State
        {
            NOT_STARTED,
            START_FUNC,
            LOOP_FUNC,
            AWAITING_LOOP_EXIT,
            EXIT_FUNC,
            FINISHED
        };

    private:
        std::string m_thread_label;
        Thread_State m_thread_state = Thread_State::NOT_STARTED;

        std::thread m_thread;
        void* m_user_data_ptr = nullptr;

        double m_max_loop_frequency;
        std::mutex m_exit_loop_mutex;
        std::condition_variable m_exit_loop_notify;

        uint32_t* m_finished_thread_count_ptr;
        std::mutex* m_thread_finished_notify_mutex_ptr;
        std::condition_variable* m_thread_finished_notify_ptr;

        bool m_start_function_enabled = false;
        bool m_loop_function_enabled = false;
        bool m_exit_function_enabled = false;
        std::function<void(Engine_Thread*, void*)> m_start_function;
        std::function<void(Engine_Thread*, void*)> m_loop_function;
        std::function<void(Engine_Thread*, void*)> m_exit_function;

    private:
        Engine_Thread(std::string thread_label, void* user_data_ptr, uint32_t* finished_thread_count_ptr, std::mutex* thread_finished_notify_mutex_ptr, std::condition_variable* thread_finished_notify_ptr);
        ~Engine_Thread();

        static void Thread_Function(Engine_Thread* instance);

    public:
        bool Attach_Start_Function(std::function<void(Engine_Thread*, void*)> start_function);
        bool Attach_Loop_Function(std::function<void(Engine_Thread*, void*)> loop_function, double max_loop_frequency);
        bool Attach_Exit_Function(std::function<void(Engine_Thread*, void*)> exit_function);

        void Start_Thread();
        void Exit_Thread();
        void Exit_Thread_Immediate();
    };
} // namespace Cascade_Core