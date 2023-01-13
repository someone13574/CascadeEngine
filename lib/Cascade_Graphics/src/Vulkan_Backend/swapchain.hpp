#pragma once

#include "device.hpp"
#include "vulkan_header.hpp"

namespace Cascade_Graphics
{
	namespace Vulkan
	{
		class Swapchain_Builder;

		class Swapchain
		{
			friend class Swapchain_Builder;

		private:
			VkSwapchainKHR m_swapchain;

			Device* m_device_ptr;

		public:
			~Swapchain();

			VkSwapchainKHR* Get();
		};
	}	 // namespace Vulkan
}	 // namespace Cascade_Graphics