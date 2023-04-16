#pragma once

#ifdef __linux__
    #define VK_USE_PLATFORM_XCB_KHR
#elif defined _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "device.hpp"
#include <vector>
#include <vulkan/vulkan.h>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Semaphore
        {
        private:
            Device* m_device_ptr;

            std::vector<VkSemaphore> m_semaphores;

        public:
            Semaphore(Device* device_ptr, uint32_t semaphore_count);
            ~Semaphore();

        public:
            VkSemaphore* Get(uint32_t index = 0);
        };
    }    // namespace Vulkan
}    // namespace Cascade_Graphics