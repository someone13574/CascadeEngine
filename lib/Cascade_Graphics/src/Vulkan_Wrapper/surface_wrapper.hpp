#pragma once

#include "../window_information.hpp"
#include "instance_wrapper.hpp"
#include "vulkan_header.hpp"

#include <memory>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Surface
        {
        private:
            VkSurfaceKHR m_surface;

            std::shared_ptr<Instance> m_instance_ptr;

        public:
            Surface(Window_Information window_data, std::shared_ptr<Instance> instance_ptr);
            ~Surface();

        public:
            VkSurfaceKHR* Get_Surface();
        };
    } // namespace Vulkan
} // namespace Cascade_Graphics