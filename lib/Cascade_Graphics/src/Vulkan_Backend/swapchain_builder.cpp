#include "swapchain_builder.hpp"

#include "queue_data.hpp"
#include <acorn_logging.hpp>
#include <algorithm>
#include <cassert>
#include <vulkan/vk_enum_string_helper.h>

namespace Cascade_Graphics
{
	namespace Vulkan
	{
		Swapchain_Builder::Swapchain_Builder(Physical_Device* physical_device_ptr, Surface* surface_ptr) :
			m_physical_device_ptr(physical_device_ptr), m_surface_ptr(surface_ptr)
		{
			m_swapchain_ptr = new Swapchain();

			Get_Surface_Capabilities();
			Get_Surface_Formats();
			Get_Present_Modes();
		}

		void Swapchain_Builder::Get_Surface_Capabilities()
		{
			LOG_TRACE << "Graphics (Vulkan): Getting swapchain surface capabilities";

			VkResult get_surface_capabilities_result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*m_physical_device_ptr->Get(), *m_surface_ptr->Get(), &m_surface_capabilities);
			if (get_surface_capabilities_result != VK_SUCCESS)
			{
				LOG_FATAL << "Graphics (Vulkan): Failed to get surface capabilities with VkResult " << get_surface_capabilities_result << " (" << string_VkResult(get_surface_capabilities_result) << ")";
				exit(EXIT_FAILURE);
			}
		}

		void Swapchain_Builder::Get_Surface_Formats()
		{
			LOG_TRACE << "Graphics (Vulkan): Getting swapchain surface formats";

			uint32_t surface_format_count;

			VkResult get_surface_format_count_result = vkGetPhysicalDeviceSurfaceFormatsKHR(*m_physical_device_ptr->Get(), *m_surface_ptr->Get(), &surface_format_count, nullptr);
			if (get_surface_format_count_result != VK_SUCCESS)
			{
				LOG_FATAL << "Graphics (Vulkan): Failed to get number of supported surface formats with VkResult " << get_surface_format_count_result << " (" << string_VkResult(get_surface_format_count_result) << ")";
				exit(EXIT_FAILURE);
			}

			m_surface_formats.resize(surface_format_count);

			VkResult get_surface_formats_result = vkGetPhysicalDeviceSurfaceFormatsKHR(*m_physical_device_ptr->Get(), *m_surface_ptr->Get(), &surface_format_count, m_surface_formats.data());
			if (get_surface_formats_result != VK_SUCCESS)
			{
				LOG_FATAL << "Graphics (Vulkan): Failed to get supported surface formats with VkResult " << get_surface_formats_result << " (" << string_VkResult(get_surface_formats_result) << ")";
				exit(EXIT_FAILURE);
			}
		}

		void Swapchain_Builder::Get_Present_Modes()
		{
			LOG_TRACE << "Graphics (Vulkan): Getting swapchain surface present modes";

			uint32_t present_mode_count;

			VkResult get_present_mode_count_result = vkGetPhysicalDeviceSurfacePresentModesKHR(*m_physical_device_ptr->Get(), *m_surface_ptr->Get(), &present_mode_count, nullptr);
			if (get_present_mode_count_result != VK_SUCCESS)
			{
				LOG_FATAL << "Graphics (Vulkan): Failed to get number of supported present modes with VkResult " << get_present_mode_count_result << " (" << string_VkResult(get_present_mode_count_result) << ")";
				exit(EXIT_FAILURE);
			}

			m_present_modes.resize(present_mode_count);

			VkResult get_present_modes_result = vkGetPhysicalDeviceSurfacePresentModesKHR(*m_physical_device_ptr->Get(), *m_surface_ptr->Get(), &present_mode_count, m_present_modes.data());
			if (get_present_modes_result != VK_SUCCESS)
			{
				LOG_FATAL << "Graphics (Vulkan): Failed to get supported present modes with VkResult " << get_present_modes_result << " (" << string_VkResult(get_present_modes_result) << ")";
				exit(EXIT_FAILURE);
			}
		}

		Swapchain_Builder& Swapchain_Builder::Select_Image_Format(std::vector<VkSurfaceFormatKHR> preferred_formats)
		{
			LOG_DEBUG << "Graphics (Vulkan): Selecting surface format";

			// Check for preferred surface formats
			for (uint32_t preferred_format_index = 0; preferred_format_index < preferred_formats.size(); preferred_format_index++)
			{
				for (uint32_t supported_format_index = 0; supported_format_index < m_surface_formats.size(); supported_format_index++)
				{
					if (preferred_formats[preferred_format_index].format == m_surface_formats[supported_format_index].format && preferred_formats[preferred_format_index].colorSpace == m_surface_formats[supported_format_index].colorSpace)
					{
						LOG_TRACE << "Graphics (Vulkan): Selected preferred format '" << string_VkFormat(m_surface_formats[supported_format_index].format) << "' and colorspace '" << string_VkColorSpaceKHR(m_surface_formats[supported_format_index].colorSpace);

						m_surface_format = m_surface_formats[supported_format_index];
						return *this;
					}
				}
			}

			LOG_TRACE << "Graphics (Vulkan): Couldn't find preferred surface format and color space";

			// Check for preferred format with different color space
			for (uint32_t preferred_format_index = 0; preferred_format_index < preferred_formats.size(); preferred_format_index++)
			{
				for (uint32_t supported_format_index = 0; supported_format_index < m_surface_formats.size(); supported_format_index++)
				{
					if (preferred_formats[preferred_format_index].format == m_surface_formats[supported_format_index].format)
					{
						LOG_TRACE << "Graphics (Vulkan): Selected preferred format '" << string_VkFormat(m_surface_formats[supported_format_index].format) << "' with non-preferred colorspace '" << string_VkColorSpaceKHR(m_surface_formats[supported_format_index].colorSpace);

						m_surface_format = m_surface_formats[supported_format_index];
						return *this;
					}
				}
			}

			LOG_TRACE << "Graphics (Vulkan): Couldn't find preferred surface format with non-preferred color space";

			// Check for non-preferred format with preferred color space
			for (uint32_t preferred_format_index = 0; preferred_format_index < preferred_formats.size(); preferred_format_index++)
			{
				for (uint32_t supported_format_index = 0; supported_format_index < m_surface_formats.size(); supported_format_index++)
				{
					if (preferred_formats[preferred_format_index].colorSpace == m_surface_formats[supported_format_index].colorSpace)
					{
						LOG_TRACE << "Graphics (Vulkan): Selected non-preferred format '" << string_VkFormat(m_surface_formats[supported_format_index].format) << "' with preferred colorspace '" << string_VkColorSpaceKHR(m_surface_formats[supported_format_index].colorSpace);

						m_surface_format = m_surface_formats[supported_format_index];
						return *this;
					}
				}
			}

			LOG_TRACE << "Graphics (Vulkan): Couldn't find non-preferred surface format with preferred color space, falling back to first supported surface format";
			LOG_TRACE << "Graphics (Vulkan): Selected non-preferred format '" << string_VkFormat(m_surface_formats[0].format) << "' with non-preferred colorspace '" << string_VkColorSpaceKHR(m_surface_formats[0].colorSpace);
			m_surface_format = m_surface_formats[0];
			return *this;
		}

		Swapchain_Builder& Swapchain_Builder::Select_Image_Extent(Window_Info* window_info_ptr)
		{
			m_swapchain_extent.width = std::clamp<uint32_t>(window_info_ptr->Get_Window_Width(), m_surface_capabilities.minImageExtent.width, m_surface_capabilities.maxImageExtent.width);
			m_swapchain_extent.height = std::clamp<uint32_t>(window_info_ptr->Get_Window_Height(), m_surface_capabilities.minImageExtent.height, m_surface_capabilities.maxImageExtent.height);

			LOG_DEBUG << "Graphics (Vulkan): Selected swapchain extent of " << m_swapchain_extent.width << "x" << m_swapchain_extent.height << ", preferred extent is " << window_info_ptr->Get_Window_Width() << "x" << window_info_ptr->Get_Window_Height();

			return *this;
		}

		Swapchain_Builder& Swapchain_Builder::Set_Swapchain_Image_Usage(VkImageUsageFlags image_usage_flags)
		{
			LOG_DEBUG << "Graphics (Vulkan): Set swapchain image usage flags to " << string_VkImageUsageFlags(image_usage_flags);
			m_swapchain_image_usage = image_usage_flags;

			return *this;
		}

		Swapchain_Builder& Swapchain_Builder::Select_Present_Mode(std::vector<VkPresentModeKHR> preferred_present_modes)
		{
			LOG_DEBUG << "Graphics (Vulkan): Selecting present mode";

			for (uint32_t preferred_present_mode_index = 0; preferred_present_mode_index < preferred_present_modes.size(); preferred_present_mode_index++)
			{
				for (uint32_t supported_present_mode_index = 0; supported_present_mode_index < m_present_modes.size(); supported_present_mode_index++)
				{
					if (preferred_present_modes[preferred_present_mode_index] == m_present_modes[supported_present_mode_index])
					{
						m_present_mode = m_present_modes[supported_present_mode_index];
						LOG_TRACE << "Graphics (Vulkan): Selected present mode '" << string_VkPresentModeKHR(m_present_mode) << "'";

						return *this;
					}
				}
			}

			assert(m_present_modes.size() != 0 && "Graphics (Vulkan): No present modes supported");

			m_present_mode = m_present_modes[0];
			LOG_TRACE << "Graphics (Vulkan): Selected present mode '" << string_VkPresentModeKHR(m_present_mode) << "'";

			return *this;
		}

		Swapchain* Swapchain_Builder::Build(Device* device_ptr)
		{
			LOG_DEBUG << "Graphics (Vulkan): Building swapchain";

			m_swapchain_ptr = new Swapchain;
			m_swapchain_ptr->m_device_ptr = device_ptr;

			// Ensure required properties have been selected
			assert(m_surface_format.format != VK_FORMAT_UNDEFINED && "Graphics (Vulkan): Surface format selector not called");
			assert(m_swapchain_extent.width != 0 && m_swapchain_extent.height != 0 && "Graphics (Vulkan): Swapchain extent selector not called");
			assert(m_swapchain_image_usage != 0 && "Graphics (Vulkan): Swapchain image usage flags have not been set");

			// Get queues which require access to the swapchain
			std::vector<uint32_t> swapchain_access_queue_families;
			for (uint32_t queue_requirement_index = 0; queue_requirement_index < device_ptr->Get_Device_Queues()->device_queue_requirements.size(); queue_requirement_index++)
			{
				for (uint32_t queue_index = 0; queue_index < device_ptr->Get_Device_Queues()->device_queue_requirements[queue_requirement_index].device_queues.size(); queue_index++)
				{
					swapchain_access_queue_families.push_back(device_ptr->Get_Device_Queues()->device_queue_requirements[queue_requirement_index].device_queues[queue_index].queue_family_index);
				}
			}

			std::sort(swapchain_access_queue_families.begin(), swapchain_access_queue_families.end());
			swapchain_access_queue_families.erase(std::unique(swapchain_access_queue_families.begin(), swapchain_access_queue_families.end()), swapchain_access_queue_families.end());

			// Select image count
			m_image_count = m_surface_capabilities.minImageCount;
			m_image_count = (m_surface_capabilities.maxImageCount != 0 && m_image_count > m_surface_capabilities.maxImageCount) ? m_surface_capabilities.maxImageCount : m_image_count;

			// Create swapchain
			VkSwapchainCreateInfoKHR swapchain_create_info = {};
			swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			swapchain_create_info.pNext = nullptr;
			swapchain_create_info.flags = 0;
			swapchain_create_info.surface = *m_surface_ptr->Get();
			swapchain_create_info.minImageCount = m_image_count;
			swapchain_create_info.imageFormat = m_surface_format.format;
			swapchain_create_info.imageColorSpace = m_surface_format.colorSpace;
			swapchain_create_info.imageExtent = m_swapchain_extent;
			swapchain_create_info.imageArrayLayers = 1;
			swapchain_create_info.imageUsage = m_swapchain_image_usage;
			swapchain_create_info.imageSharingMode = (swapchain_access_queue_families.size() == 1) ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
			swapchain_create_info.queueFamilyIndexCount = swapchain_access_queue_families.size();
			swapchain_create_info.pQueueFamilyIndices = swapchain_access_queue_families.data();
			swapchain_create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
			swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			swapchain_create_info.presentMode = m_present_mode;
			swapchain_create_info.clipped = VK_TRUE;
			swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

			VkResult create_swapchain_result = vkCreateSwapchainKHR(*device_ptr->Get(), &swapchain_create_info, NULL, &m_swapchain_ptr->m_swapchain);
			if (create_swapchain_result != VK_SUCCESS)
			{
				LOG_FATAL << "Graphics (Vulkan): Failed to create swapchain with VkResult " << create_swapchain_result << " (" << string_VkResult(create_swapchain_result) << ")";
				exit(EXIT_FAILURE);
			}

			return m_swapchain_ptr;
		}
	}	 // namespace Vulkan
}	 // namespace Cascade_Graphics