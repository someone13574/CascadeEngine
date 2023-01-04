#include "entry.hpp"

#include "application_factory.hpp"
#include <acorn_logging.hpp>

int main()
{
	Cascade_Core::Application* application_ptr = Cascade_Core::Application_Factory::Construct_Application();
	Cascade_Core::Application_Factory::Destroy_Application(application_ptr);
}