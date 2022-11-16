#pragma once

#include <string>

namespace Cascade_Core
{
    class Application_Factory;

    class Application
    {
    private:
        std::string m_application_name;
        uint32_t m_application_major_version;
        uint32_t m_application_minor_version;
        uint32_t m_application_patch_version;

    private:
        friend class Application_Factory;
        Application();
        ~Application();
    };
} // namespace Cascade_Core