#pragma once

#include "vulkan_header.hpp"
#include <string>
#include <vector>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        struct Queue_Requirement
        {
            std::string requirement_label;       // Label used to identify this requirement in logs
            uint32_t required_queue_count;       // Number of queues required
            VkQueueFlagBits required_queue_type; // Type of queue required
        };

        struct Queue_Usage
        {
            uint32_t queue_family_index; // Index of the queue family this queue is from
            uint32_t queue_count;        // Number of queues from queue_family_index
        };

        struct Queue_Provider
        {
            Queue_Requirement requirement; // Requirement that is queue fulfills
            Queue_Usage usage;             // Queue usage by this provider
        };

        struct Queue_Set
        {
            uint32_t provided_queues;                    // Total number of queues this set uses
            std::vector<uint32_t> queue_family_usage;    // Usage of queue families by this set
            std::vector<Queue_Provider> queue_providers; // Array of providers for this set

            Queue_Set& operator+=(const Queue_Set& other)
            {
                this->provided_queues += other.provided_queues;

                for (uint32_t i = 0; i < queue_family_usage.size(); i++)
                {
                    this->queue_family_usage[i] += other.queue_family_usage[i];
                }

                this->queue_providers.reserve(this->queue_providers.size() + other.queue_providers.size());
                this->queue_providers.insert(this->queue_providers.end(), other.queue_providers.begin(), other.queue_providers.end());

                return *this;
            }
        };

    } // namespace Vulkan
} // namespace Cascade_Graphics