#pragma once

#ifdef __linux__
    #define VK_USE_PLATFORM_XCB_KHR
#elif defined _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "../platform_info.hpp"
#include "device.hpp"
#include "physical_device.hpp"
#include "queue_data.hpp"
#include "surface.hpp"
#include "swapchain.hpp"
#include <vector>
#include <vulkan/vulkan.h>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Swapchain_Builder
        {
        private:
            Physical_Device* m_physical_device_ptr;
            Surface* m_surface_ptr;

            Swapchain* m_swapchain_ptr;

            VkSurfaceCapabilitiesKHR m_surface_capabilities;
            std::vector<VkSurfaceFormatKHR> m_surface_formats;
            std::vector<VkPresentModeKHR> m_present_modes;

            uint32_t m_image_count = 0;
            VkSurfaceFormatKHR m_surface_format = {VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
            VkExtent2D m_swapchain_extent = {0, 0};
            VkImageUsageFlags m_swapchain_image_usage = 0;
            VkPresentModeKHR m_present_mode;
            std::vector<uint32_t> m_allowed_queue_families;

        private:
            void Get_Surface_Capabilities();
            void Get_Surface_Formats();
            void Get_Present_Modes();

            void Select_Image_Count();

            void Create_Swapchain(Device* device_ptr);
            void Get_Swapchain_Images(Device* device_ptr);
            void Create_Swapchain_Image_Views(Device* device_ptr);

        public:
            Swapchain_Builder& Select_Image_Format(std::vector<VkSurfaceFormatKHR> preferred_formats);
            Swapchain_Builder& Select_Image_Extent(Window_Info* window_info_ptr);
            Swapchain_Builder& Set_Swapchain_Image_Usage(VkImageUsageFlags image_usage_flags);
            Swapchain_Builder& Select_Present_Mode(std::vector<VkPresentModeKHR> preferred_present_modes);
            Swapchain_Builder& Set_Allowed_Queue_Requirements(std::vector<Device_Queue_Requirement*> allowed_queue_requirements);

        public:
            Swapchain_Builder(Physical_Device* physical_device_ptr, Surface* surface_ptr);

            Swapchain* Build(Device* device_ptr);
        };
    }    // namespace Vulkan
}    // namespace Cascade_Graphics