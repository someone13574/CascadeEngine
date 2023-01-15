#pragma once

#include "application.hpp"

namespace Cascade_Core
{
    class Application_Factory
    {
    private:
        Application_Factory() {};

    public:
        static Application* Construct_Application();
        static void Destroy_Application(Application* application_ptr);
    };
}    // namespace Cascade_Core