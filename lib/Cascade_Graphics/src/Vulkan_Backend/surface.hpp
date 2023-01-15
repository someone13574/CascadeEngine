#pragma once

#ifdef __linux__
    #define VK_USE_PLATFORM_XCB_KHR
#elif defined _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "instance.hpp"
#include <vulkan/vulkan.h>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Surface
        {
        protected:
            Instance* m_instance_ptr;

            VkSurfaceKHR m_surface;

        protected:
            Surface(Instance* instance_ptr);

        public:
            virtual ~Surface();

            VkSurfaceKHR Get();
        };
    }    // namespace Vulkan
}    // namespace Cascade_Graphics