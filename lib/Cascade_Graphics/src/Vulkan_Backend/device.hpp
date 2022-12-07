#pragma once

#include "physical_device.hpp"
#include "queue_data.hpp"
#include "vulkan_header.hpp"
#include <string>
#include <vector>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Device
        {
        private:
            VkDevice m_device;
            Queue_Set m_queues;

        public:
            Device(Physical_Device* physical_device_ptr);
            ~Device();

        private:
            static std::vector<VkDeviceQueueCreateInfo> Get_Queue_Create_Information(Queue_Set* queue_set_ptr);

        public:
            VkDevice* Get();
        };
    } // namespace Vulkan
} // namespace Cascade_Graphics