#include "thread.hpp"

#include <acorn_logging.hpp>
#include <algorithm>

namespace Cascade_Core
{
    Thread::Thread(std::string thread_label, void* user_data_ptr, uint32_t* finished_thread_count_ptr, std::mutex* thread_finished_notify_mutex_ptr, std::condition_variable* thread_finished_notify_ptr) :
        m_thread_label(thread_label), m_user_data_ptr(user_data_ptr), m_finished_thread_count_ptr(finished_thread_count_ptr), m_thread_finished_notify_mutex_ptr(thread_finished_notify_mutex_ptr), m_thread_finished_notify_ptr(thread_finished_notify_ptr)
    {
        LOG_INFO << "Core: Creating thread '" << thread_label << "'";
    }

    Thread::~Thread()
    {
        LOG_TRACE << "Core: Destroyed engine thread '" << m_thread_label << "'";
    }

    void Thread::Thread_Function(Thread* instance)
    {
        LOG_INFO << "Core: Starting thread '" << instance->m_thread_label << "'";

        if (instance->m_start_function_enabled)
        {
            LOG_DEBUG << "Core: Calling start function of thread '" << instance->m_thread_label << "'";

            {
                std::scoped_lock<std::mutex> state_change_lock(instance->m_state_change_mutex);
                instance->m_thread_state = Thread_State::START_FUNC;
                instance->m_state_change_notify.notify_all();
            }

            instance->m_start_function(instance, instance->m_user_data_ptr);
        }

        if (instance->m_loop_function_enabled)
        {
            LOG_INFO << "Core: Starting loop on thread '" << instance->m_thread_label << "'";

            {
                std::scoped_lock<std::mutex> state_change_lock(instance->m_state_change_mutex);
                instance->m_thread_state = Thread_State::LOOP_FUNC;
                instance->m_state_change_notify.notify_all();
            }

            while (instance->m_thread_state != Thread_State::AWAITING_LOOP_EXIT)
            {
                std::chrono::time_point<std::chrono::high_resolution_clock> loop_start_timepoint = std::chrono::high_resolution_clock::now();
                instance->m_loop_function(instance, instance->m_user_data_ptr);
                int64_t function_execution_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - loop_start_timepoint).count();
                int64_t sleep_for = std::max((int64_t)(1000000.0 / instance->m_max_loop_frequency) - function_execution_time, (int64_t)0);

                std::unique_lock<std::mutex> exit_loop_lock(instance->m_exit_loop_mutex);
                instance->m_exit_loop_notify.wait_for(exit_loop_lock, std::chrono::microseconds(sleep_for), [instance]
                                                      { return instance->m_thread_state == Cascade_Core::Thread::Thread_State::AWAITING_LOOP_EXIT; });
            }
        }

        if (instance->m_exit_function_enabled)
        {
            {
                std::scoped_lock<std::mutex> state_change_lock(instance->m_state_change_mutex);
                instance->m_thread_state = Thread_State::EXIT_FUNC;
                instance->m_state_change_notify.notify_all();
            }

            instance->m_exit_function(instance, instance->m_user_data_ptr);
        }

        LOG_INFO << "Core: Thread '" << instance->m_thread_label << "' finished execution";

        {
            std::scoped_lock<std::mutex> thread_finished_notify_lock(*instance->m_thread_finished_notify_mutex_ptr);
            std::scoped_lock<std::mutex> state_change_lock(instance->m_state_change_mutex);
            instance->m_thread_state = Thread_State::FINISHED;
            (*instance->m_finished_thread_count_ptr)++;
        }

        instance->m_thread_finished_notify_ptr->notify_all();
        instance->m_state_change_notify.notify_all();
    }

    bool Thread::Attach_Start_Function(std::function<void(Thread*, void*)> start_function)
    {
        LOG_DEBUG << "Core: Attaching start function to thread '" << m_thread_label << "'";

        if (m_thread_state != Thread_State::NOT_STARTED)
        {
            LOG_WARN << "Core: Cannot attach start function to already started thread (" << m_thread_label << ")";
            return false;
        }

        if (m_start_function_enabled)
        {
            LOG_WARN << "Core: Start function already attached to thread '" << m_thread_label << "'";
            return false;
        }

        m_start_function_enabled = true;
        m_start_function = start_function;

        return true;
    }

    bool Thread::Attach_Loop_Function(std::function<void(Thread*, void*)> loop_function, double max_loop_frequency)
    {
        LOG_DEBUG << "Core: Attaching loop function to thread '" << m_thread_label << "'";

        if (m_thread_state != Thread_State::NOT_STARTED)
        {
            LOG_WARN << "Core: Cannot attach loop function to already started thread (" << m_thread_label << ")";
            return false;
        }

        if (m_exit_function_enabled)
        {
            LOG_WARN << "Core: Loop function already attached to thread '" << m_thread_label << "'";
            return false;
        }

        m_loop_function_enabled = true;
        m_max_loop_frequency = max_loop_frequency;
        m_loop_function = loop_function;

        return true;
    }

    bool Thread::Attach_Exit_Function(std::function<void(Thread*, void*)> exit_function)
    {
        LOG_DEBUG << "Core: Attaching exit function to thread '" << m_thread_label << "'";

        if (m_thread_state != Thread_State::NOT_STARTED)
        {
            LOG_WARN << "Core: Cannot attach exit function to already started thread (" << m_thread_label << ")";
            return false;
        }

        if (m_exit_function_enabled)
        {
            LOG_WARN << "Core: Exit function already attached to thread '" << m_thread_label << "'";
            return false;
        }

        m_exit_function_enabled = true;
        m_exit_function = exit_function;

        return true;
    }

    void Thread::Start_Thread()
    {
        if (m_thread_state == Thread_State::NOT_STARTED)
        {
            m_thread = std::thread(Thread_Function, this);
        }
        else
        {
            LOG_WARN << "Core: Thread '" << m_thread_label << "' already started";
        }
    }

    void Thread::Exit_Thread()
    {
        LOG_DEBUG << "Core: Exiting thread '" << m_thread_label << "'";

        if (m_thread_state == Thread_State::NOT_STARTED)
        {
            m_start_function_enabled = false;
            m_loop_function_enabled = false;

            Start_Thread();
            return;
        }
        else if (m_thread_state == Thread_State::START_FUNC)
        {
            m_loop_function_enabled = false;
            return;
        }
        else if (m_thread_state == Thread_State::LOOP_FUNC)
        {
            {
                std::scoped_lock<std::mutex> exit_loop_lock(m_exit_loop_mutex);
                m_thread_state = Thread_State::AWAITING_LOOP_EXIT;
            }
            m_exit_loop_notify.notify_all();
        }
    }

    void Thread::Exit_Thread_Immediate()
    {
        LOG_DEBUG << "Core: Exiting thread '" << m_thread_label << "' without exit function";

        if (m_thread_state == Thread_State::NOT_STARTED)
        {
            m_start_function_enabled = false;
            m_loop_function_enabled = false;
            m_exit_function_enabled = false;

            Start_Thread();
            return;
        }
        else if (m_thread_state == Thread_State::START_FUNC)
        {
            m_loop_function_enabled = false;
            m_exit_function_enabled = false;
            return;
        }
        else if (m_thread_state == Thread_State::LOOP_FUNC)
        {
            {
                std::scoped_lock<std::mutex> exit_loop_lock(m_exit_loop_mutex);
                m_exit_function_enabled = false;
                m_thread_state = Thread_State::AWAITING_LOOP_EXIT;
            }
            m_exit_loop_notify.notify_all();

            return;
        }
    }

    void Thread::Await_State(Thread_State target_state)
    {
        LOG_TRACE << "Core: Waiting for '" << m_thread_label << "' to reach state " << target_state;

        std::unique_lock<std::mutex> thread_await_state_lock(m_state_change_mutex);
        m_state_change_notify.wait(thread_await_state_lock, [this, target_state]
                                   { return m_thread_state == target_state; });

        LOG_TRACE << "Core: '" << m_thread_label << "' has reached state " << target_state;
    }
}    // namespace Cascade_Core