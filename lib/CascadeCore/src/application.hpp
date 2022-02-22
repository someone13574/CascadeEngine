#pragma once

#include <string>

namespace CascadeCore
{
    class Application
    {
      private:
        std::string m_application_name;
        std::string m_application_version;

      public:
        Application(std::string name, std::string version);
    };

} // namespace CascadeCore