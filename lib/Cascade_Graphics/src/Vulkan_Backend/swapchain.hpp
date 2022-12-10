#pragma once

#include "vulkan_header.hpp"

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Swapchain
        {
        private:
            VkSwapchainKHR m_swapchain;

        public:
            VkSwapchainKHR* Get();
        };
    } // namespace Vulkan
} // namespace Cascade_Graphics