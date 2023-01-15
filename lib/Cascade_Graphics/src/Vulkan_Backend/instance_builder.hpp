#pragma once

#ifdef __linux__
    #define VK_USE_PLATFORM_XCB_KHR
#elif defined _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "instance.hpp"
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Instance_Builder
        {
        private:
            Instance* m_instance_ptr;

            VkApplicationInfo m_application_info;
            std::vector<const char*> m_enabled_layers;
            std::vector<const char*> m_enabled_extensions;

        private:
            void Ensure_All_Required_Layers_Are_Present();
            void Ensure_All_Required_Extensions_Are_Present();

        public:
            Instance_Builder();
            Instance_Builder& Set_Application_Details(std::string application_name, uint32_t application_version);
            Instance_Builder& Set_Engine_Details(std::string engine_name, uint32_t engine_version);
            Instance_Builder& Set_Minimum_Vulkan_Version(uint32_t api_version);
            Instance_Builder& Add_Layer(const char* layer_name);
            Instance_Builder& Add_Extension(const char* extension_name);

            Instance* Build();
        };
    }    // namespace Vulkan
}    // namespace Cascade_Graphics