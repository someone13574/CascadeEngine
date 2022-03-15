#pragma once

#include "../vulkan_header.hpp"
#include "physical_device_wrapper.hpp"
#include "queue_wrapper.hpp"
#include "validation_layer_wrapper.hpp"

#include <memory>

namespace CascadeGraphics
{
    namespace Vulkan
    {
        class Physical_Device;

        class Device
        {
        private:
            VkDevice m_device;

        public:
            Device(std::shared_ptr<Queue_Manager> queue_manager_ptr,
                   std::shared_ptr<CascadeGraphicsDebugging::Vulkan::Validation_Layer> validation_layer_ptr,
                   std::shared_ptr<Physical_Device> physical_device_ptr);
            ~Device();

        public:
            VkDevice* Get_Device();
        };
    } // namespace Vulkan
} // namespace CascadeGraphics
