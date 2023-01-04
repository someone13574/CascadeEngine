#include "swapchain.hpp"

namespace Cascade_Graphics
{
	namespace Vulkan
	{
		VkSwapchainKHR* Swapchain::Get()
		{
			return &m_swapchain;
		}
	}	 // namespace Vulkan
}	 // namespace Cascade_Graphics