#pragma once

#include "vulkan_header.hpp"

#include "physical_device_wrapper.hpp"
#include "queue_manager.hpp"
#include "validation_layer_wrapper.hpp"

#include <memory>

namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        class Physical_Device_Wrapper;

        class Device
        {
        private:
            VkDevice m_device;

        public:
            Device(std::shared_ptr<Physical_Device_Wrapper> physical_device_ptr, std::shared_ptr<Queue_Manager> queue_manager_ptr, std::shared_ptr<Validation_Layer> validation_layer_ptr);
            ~Device();

        public:
            VkDevice* Get_Device();
        };
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics
