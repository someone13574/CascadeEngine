#pragma once

#include "instance.hpp"
#include "vulkan_header.hpp"

namespace Cascade_Graphics
{
	namespace Vulkan
	{
		class Surface
		{
		protected:
			Instance* m_instance_ptr;
			VkSurfaceKHR m_surface;

		protected:
			Surface(Instance* instance_ptr);

		public:
			virtual ~Surface();

			VkSurfaceKHR* Get();
		};
	}	 // namespace Vulkan
}	 // namespace Cascade_Graphics