#pragma once

#include "vulkan_header.hpp"

#include <set>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Instance
        {
        private:
            VkInstance m_instance;
            VkApplicationInfo m_application_info;
            std::set<const char*> m_required_instance_extensions;

        private:
            unsigned int Get_Supported_Extension_Count();
            bool Is_Vulkan_Supported();

        public:
            Instance(const char* application_name, unsigned int application_version, std::set<const char*> required_instance_extensions);
            ~Instance();

        public:
            VkInstance* Get_Instance();
        };
    } // namespace Vulkan
} // namespace Cascade_Graphics