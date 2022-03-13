#pragma once

#include "../vulkan_header.hpp"

#include <vector>

namespace CascadeGraphics
{
    namespace Vulkan
    {
        class Instance
        {
        private:
            VkInstance m_instance;
            VkApplicationInfo m_application_info;
            std::vector<const char*> m_required_instance_extensions;

        private:
            void Get_Required_Instance_Extensions();

        public:
            Instance(const char* application_name, unsigned int application_version);
            ~Instance();
        };
    } // namespace Vulkan
} // namespace CascadeGraphics