#pragma once

#include "physical_device_wrapper.hpp"
#include "queue_manager.hpp"
#include "validation_layer_wrapper.hpp"
#include "vulkan_header.hpp"
#include <memory>


namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        class Physical_Device_Wrapper;

        class Logical_Device_Wrapper
        {
        private:
            VkDevice m_device;

        public:
            Logical_Device_Wrapper(std::shared_ptr<Physical_Device_Wrapper> physical_device_wrapper_ptr, std::shared_ptr<Queue_Manager> queue_manager_ptr);
            ~Logical_Device_Wrapper();

        public:
            VkDevice* Get_Device();
        };
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics
