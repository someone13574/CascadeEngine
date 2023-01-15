#pragma once

#ifdef __linux__
    #define VK_USE_PLATFORM_XCB_KHR
#elif defined _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.h>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Instance
        {
            friend class Instance_Builder;

        private:
            VkInstance m_instance;

        private:
            Instance();

        public:
            ~Instance();

            VkInstance* Get();
        };
    }    // namespace Vulkan
}    // namespace Cascade_Graphics