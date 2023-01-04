#include "cascade.hpp"
#include <acorn_logging.hpp>
#include <chrono>

void Cascade_Main(Cascade_Core::Thread* thread_ptr, void* user_data_ptr)
{
	Cascade_Core::Application* application_ptr = (Cascade_Core::Application*)user_data_ptr;

	application_ptr->Set_Application_Details("test_application", 0, 7, 0);
	Cascade_Core::Window* window_ptr = application_ptr->Get_Window_Factory()->Create_Window("test-window", 500, 500);

	(void)thread_ptr;
	(void)window_ptr;
}