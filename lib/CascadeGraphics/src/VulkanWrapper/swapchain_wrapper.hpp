#pragma once

#include "../vulkan_header.hpp"

#include "physical_device_wrapper.hpp"
#include "surface_wrapper.hpp"

#include <memory>
#include <vector>

namespace CascadeGraphics
{
    namespace Vulkan
    {
        class Swapchain
        {
        private:
            std::shared_ptr<Physical_Device> m_physical_device_ptr;
            std::shared_ptr<Surface> m_surface_ptr;

            VkSurfaceCapabilitiesKHR m_surface_capabilities;
            std::vector<VkSurfaceFormatKHR> m_supported_surface_formats;
            std::vector<VkPresentModeKHR> m_supported_present_modes;

            VkSurfaceFormatKHR m_surface_format;
            VkPresentModeKHR m_present_mode;

        private:
            void Get_Swapchain_Support();
            void Select_Swapchain_Format();
            void Select_Present_Mode();

        public:
            Swapchain(std::shared_ptr<Physical_Device> physical_device_ptr, std::shared_ptr<Surface> surface_ptr);
            ~Swapchain();

        public:
            static bool Is_Swapchain_Adequate(VkPhysicalDevice* physical_device_ptr, std::shared_ptr<Surface> surface_ptr);
        };
    } // namespace Vulkan
} // namespace CascadeGraphics