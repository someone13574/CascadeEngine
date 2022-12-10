#pragma once

#include "physical_device.hpp"
#include "surface.hpp"
#include "swapchain.hpp"
#include "vulkan_header.hpp"
#include <vector>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Swapchain_Builder
        {
        private:
            Swapchain* m_swapchain_ptr;
            Physical_Device* m_physical_device_ptr;
            Surface* m_surface_ptr;

            VkSurfaceCapabilitiesKHR m_surface_capabilities;
            std::vector<VkSurfaceFormatKHR> m_surface_formats;
            std::vector<VkPresentModeKHR> m_present_modes;

        private:
            void Get_Surface_Capabilities();
            void Get_Surface_Formats();
            void Get_Present_Modes();

        public:
            Swapchain_Builder(Physical_Device* physical_device_ptr, Surface* surface_ptr);

            Swapchain* Build();
        };
    } // namespace Vulkan
} // namespace Cascade_Graphics