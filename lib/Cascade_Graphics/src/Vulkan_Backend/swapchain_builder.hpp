#pragma once

#include "../platform_info.hpp"
#include "device.hpp"
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

			uint32_t m_image_count = 0;
			VkSurfaceFormatKHR m_surface_format = {VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
			VkExtent2D m_swapchain_extent = {0, 0};
			VkImageUsageFlags m_swapchain_image_usage = 0;
			VkPresentModeKHR m_present_mode;

		private:
			void Get_Surface_Capabilities();
			void Get_Surface_Formats();
			void Get_Present_Modes();

		public:
			Swapchain_Builder& Select_Image_Format(std::vector<VkSurfaceFormatKHR> preferred_formats);
			Swapchain_Builder& Select_Image_Extent(Window_Info* window_info_ptr);
			Swapchain_Builder& Set_Swapchain_Image_Usage(VkImageUsageFlags image_usage_flags);
			Swapchain_Builder& Select_Present_Mode(std::vector<VkPresentModeKHR> preferred_present_modes);

		public:
			Swapchain_Builder(Physical_Device* physical_device_ptr, Surface* surface_ptr);

			Swapchain* Build(Device* device_ptr);
		};
	}	 // namespace Vulkan
}	 // namespace Cascade_Graphics