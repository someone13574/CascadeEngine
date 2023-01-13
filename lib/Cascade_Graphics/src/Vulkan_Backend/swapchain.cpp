#include "swapchain.hpp"
#include <acorn_logging.hpp>

namespace Cascade_Graphics
{
	namespace Vulkan
	{
		Swapchain::~Swapchain()
		{
			LOG_TRACE << "Graphics (Vulkan): Destroying swapchain";

			vkDestroySwapchainKHR(*m_device_ptr->Get(), m_swapchain, NULL);
		}

		VkSwapchainKHR* Swapchain::Get()
		{
			return &m_swapchain;
		}
	}	 // namespace Vulkan
}	 // namespace Cascade_Graphics