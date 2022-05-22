#pragma once

#include <string>

namespace Cascade_Core
{
    class Application
    {
    public:
        struct Application_Info
        {
            std::string title;
            unsigned int major_version;
            unsigned int minor_version;
        };

    private:
        Application_Info m_application_info;

    public:
        Application(Application_Info application_info);
    };

} // namespace Cascade_Core