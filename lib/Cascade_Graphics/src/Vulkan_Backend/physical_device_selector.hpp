#pragma once

#include "../platform_info.hpp"
#include "instance.hpp"
#include "physical_device.hpp"
#include "queue_selector.hpp"
#include <vector>

namespace Cascade_Graphics
{
	namespace Vulkan
	{
		class Physical_Device_Selector
		{
		private:
			struct Physical_Device_Filter_Info
			{
				Physical_Device* physical_device_ptr;
				Queue_Selector* queue_selector_ptr;
				std::vector<VkExtensionProperties> available_extensions;

				double score;
			};

		private:
			std::vector<Physical_Device_Filter_Info> m_physical_device_filter_infos;

			Instance* m_instance_ptr;
			Platform_Info* m_platform_info_ptr;

		public:
			Physical_Device_Selector(Instance* instance_ptr, Platform_Info* platform_info_ptr);

		public:
			Physical_Device_Selector& Require_Queue_Type(std::string requirement_label, VkQueueFlagBits queue_type, uint32_t required_queue_count, float queue_priority, bool swapchain_access);
			Physical_Device_Selector& Require_Present_Queue();

			Physical_Device_Selector& Require_Extension(const char* extension_name);
			Physical_Device_Selector& Prefer_Dedicated(double add_score);
			Physical_Device* Best();
		};
	}	 // namespace Vulkan
}	 // namespace Cascade_Graphics