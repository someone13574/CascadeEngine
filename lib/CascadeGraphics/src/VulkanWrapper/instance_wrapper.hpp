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

        public:
            Instance(const char* application_name, unsigned int application_version);
            ~Instance();

        public:
            static std::vector<const char*> Get_Required_Instance_Extensions();

            VkInstance* Get_Instance();
        };
    } // namespace Vulkan
} // namespace CascadeGraphics