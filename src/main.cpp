#include "cascade.hpp"
#include <acorn_logging.hpp>
#include <chrono>

void Cascade_Main(Cascade_Core::Engine_Thread* engine_thread_ptr, void* user_data_ptr)
{
    Cascade_Core::Application* application_ptr = (Cascade_Core::Application*)user_data_ptr;

    application_ptr->Set_Application_Details("test_application", 0, 7, 0);
}