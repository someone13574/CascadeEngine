#pragma once

#include "device.hpp"
#include "queue_data.hpp"
#include "vulkan_header.hpp"
#include <vector>

namespace Cascade_Graphics
{
	namespace Vulkan
	{
		class Buffer
		{
		private:
			Device* m_device_ptr;

			VkBuffer m_buffer;
			VkDeviceMemory m_device_memory;

		public:
			Buffer(Device* device_ptr, VkBufferUsageFlags buffer_usage_flags, VkDeviceSize buffer_size, std::vector<Device_Queue_Requirement*> queue_requirements_with_access, VkMemoryPropertyFlags required_memory_properties, VkMemoryPropertyFlags preferred_memory_properties);
			~Buffer();
		};
	}	 // namespace Vulkan
}	 // namespace Cascade_Graphics