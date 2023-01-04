#include "swapchain_builder.hpp"

#include "vkresult_translator.hpp"
#include <acorn_logging.hpp>

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
				LOG_FATAL << "Graphics (Vulkan): Failed to get surface capabilities with VkResult " << get_surface_capabilities_result << " (" << Translate_VkResult(get_surface_capabilities_result) << ")";
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
				LOG_FATAL << "Graphics (Vulkan): Failed to get number of supported surface formats with VkResult " << get_surface_format_count_result << " (" << Translate_VkResult(get_surface_format_count_result) << ")";
				exit(EXIT_FAILURE);
			}

			m_surface_formats.resize(surface_format_count);

			VkResult get_surface_formats_result = vkGetPhysicalDeviceSurfaceFormatsKHR(*m_physical_device_ptr->Get(), *m_surface_ptr->Get(), &surface_format_count, m_surface_formats.data());
			if (get_surface_formats_result != VK_SUCCESS)
			{
				LOG_FATAL << "Graphics (Vulkan): Failed to get supported surface formats with VkResult " << get_surface_formats_result << " (" << Translate_VkResult(get_surface_formats_result) << ")";
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
				LOG_FATAL << "Graphics (Vulkan): Failed to get number of supported present modes with VkResult " << get_present_mode_count_result << " (" << Translate_VkResult(get_present_mode_count_result) << ")";
				exit(EXIT_FAILURE);
			}

			m_present_modes.resize(present_mode_count);

			VkResult get_present_modes_result = vkGetPhysicalDeviceSurfacePresentModesKHR(*m_physical_device_ptr->Get(), *m_surface_ptr->Get(), &present_mode_count, m_present_modes.data());
			if (get_present_modes_result != VK_SUCCESS)
			{
				LOG_FATAL << "Graphics (Vulkan): Failed to get supported present modes with VkResult " << get_present_modes_result << " (" << Translate_VkResult(get_present_modes_result) << ")";
				exit(EXIT_FAILURE);
			}
		}

		Swapchain* Swapchain_Builder::Build()
		{
			LOG_TRACE << "Graphics (Vulkan): Building swapchain";

			// VkSwapchainCreateInfoKHR swapchain_create_info = {};
			// swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			// swapchain_create_info.pNext = nullptr;
			// swapchain_create_info.flags = 0;
			// swapchain_create_info.surface = *m_surface_ptr->Get();
			// swapchain_create_info.minImageCount;   // Image count selector
			// swapchain_create_info.imageFormat;     // Surface format selector
			// swapchain_create_info.imageColorSpace; // Surface format selector
			// swapchain_create_info.imageExtent;     // Image size selector
			// swapchain_create_info.imageArrayLayers = 1;
			// swapchain_create_info.imageUsage; // Image usage selector
			// swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			// swapchain_create_info.

			// vkQueuePresentKHR()

			return m_swapchain_ptr;
		}
	}	 // namespace Vulkan
}	 // namespace Cascade_Graphics