#pragma once

#include "../vulkan_header.hpp"

#include "instance_wrapper.hpp"

#include <memory>
#include <xcb/xcb.h>

namespace CascadeGraphics
{
    namespace Vulkan
    {
        class Surface
        {
        private:
            VkSurfaceKHR m_surface;

            std::shared_ptr<Instance> m_instance_ptr;

        public:
            Surface(xcb_connection_t* connection_ptr, xcb_window_t* window_ptr, std::shared_ptr<Instance> instance_ptr);
            ~Surface();
        };
    } // namespace Vulkan
} // namespace CascadeGraphics
