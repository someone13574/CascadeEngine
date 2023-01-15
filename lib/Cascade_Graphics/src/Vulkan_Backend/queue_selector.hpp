#pragma once

#include "../platform_info.hpp"
#include "physical_device.hpp"
#include "queue_data.hpp"
#include "vulkan_header.hpp"
#include <string>
#include <vector>

namespace Cascade_Graphics
{
	namespace Vulkan
	{
		class Queue_Selector
		{
		private:
			struct Queue_Requirement
			{
				std::string requirement_label;			// Label used to identify this requirement in logs
				uint32_t required_queue_count;			// Number of queues required
				VkQueueFlagBits required_queue_type;	// Type of queue required
				float queue_priority;					// Priority of the queues created to meet this requirement

				bool operator==(const Queue_Requirement& other)
				{
					return requirement_label == other.requirement_label && required_queue_count == other.required_queue_count && required_queue_type == other.required_queue_type && queue_priority == other.queue_priority;
				}
			};

			struct Queue_Usage
			{
				uint32_t queue_family_index;	// Index of the queue family this queue is from
				uint32_t queue_count;			// Number of queues from queue_family_index
			};

			struct Queue_Provider
			{
				bool present_support;			  // Whether or not this queue provides present support
				Queue_Requirement requirement;	  // Requirement that is queue fulfills
				Queue_Usage usage;				  // Queue usage by this provider
			};

			struct Queue_Set
			{
				bool present_support;							// Whether or not this queue set contains a queue which provides present support
				uint32_t provided_queues;						// Total number of queues this set uses
				std::vector<uint32_t> queue_family_usage;		// Usage of queue families by this set
				std::vector<Queue_Provider> queue_providers;	// Array of providers for this set

				Queue_Set& operator+=(const Queue_Set& other)
				{
					this->provided_queues += other.provided_queues;

					for (uint32_t i = 0; i < queue_family_usage.size(); i++)
					{
						this->queue_family_usage[i] += other.queue_family_usage[i];
					}

					this->queue_providers.reserve(this->queue_providers.size() + other.queue_providers.size());
					this->queue_providers.insert(this->queue_providers.end(), other.queue_providers.begin(), other.queue_providers.end());

					this->present_support = this->present_support || other.present_support;

					return *this;
				}
			};

		private:
			Platform_Info* m_platform_info_ptr;
			Physical_Device* m_pyhsical_device_ptr;

			bool m_sets_up_to_date = false;
			bool m_present_queue_required = false;

			std::vector<VkQueueFamilyProperties> m_queue_families;
			std::vector<bool> m_queue_family_present_support;

			std::vector<Queue_Requirement> m_queue_requirements;
			std::vector<Queue_Set> m_valid_queue_sets;

		private:
			void Get_Queue_Family_Present_Support();
			void Generate_Sets();

		public:
			Queue_Selector(Physical_Device* physical_device_ptr, Platform_Info* platform_info_ptr);

		public:
			Queue_Selector& Add_Queue_Requirement(std::string label, VkQueueFlagBits queue_type, uint32_t required_queue_count, float queue_priority);
			Queue_Selector& Require_Present_Queue();

			bool Meets_Requirements();
			Device_Queues Best();
		};
	}	 // namespace Vulkan
}	 // namespace Cascade_Graphics