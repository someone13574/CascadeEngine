#include "surface.hpp"

namespace Cascade_Graphics
{
	namespace Vulkan
	{
		Surface::Surface(Instance* instance_ptr) :
			m_instance_ptr(instance_ptr)
		{
		}

		Surface::~Surface()
		{
			vkDestroySurfaceKHR(*m_instance_ptr->Get(), m_surface, nullptr);
		}

		VkSurfaceKHR* Surface::Get()
		{
			return &m_surface;
		}
	}	 // namespace Vulkan
}	 // namespace Cascade_Graphics