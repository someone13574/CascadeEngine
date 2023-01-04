#include "instance.hpp"

#include <acorn_logging.hpp>

namespace Cascade_Graphics
{
	namespace Vulkan
	{
		Instance::Instance()
		{
		}

		Instance::~Instance()
		{
			LOG_INFO << "Graphics (Vulkan): Destroying instance";

			vkDestroyInstance(m_instance, nullptr);

			LOG_TRACE << "Graphics (Vulkan): Finished destroying instance";
		}

		VkInstance* Instance::Get()
		{
			return &m_instance;
		}
	}	 // namespace Vulkan
}	 // namespace Cascade_Graphics