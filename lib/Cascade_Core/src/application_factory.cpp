#include "application_factory.hpp"

namespace Cascade_Core
{
    Application* Application_Factory::Construct_Application()
    {
        return new Application();
    }

    void Application_Factory::Destroy_Application(Application* application_ptr)
    {
        delete application_ptr;
    }
}