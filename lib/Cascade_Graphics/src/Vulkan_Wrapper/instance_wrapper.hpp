#pragma once

#include "vulkan_header.hpp"
#include <set>


namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        class Instance_Wrapper
        {
        private:
            VkInstance m_instance;
            VkApplicationInfo m_application_info;
            std::set<const char*> m_required_instance_extensions;

        private:
            uint32_t Get_Supported_Extension_Count();
            bool Is_Vulkan_Supported();

        public:
            Instance_Wrapper(const char* application_name, uint32_t application_version, std::set<const char*> required_instance_extensions);
            ~Instance_Wrapper();

        public:
            VkInstance* Get_Instance();
        };
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics