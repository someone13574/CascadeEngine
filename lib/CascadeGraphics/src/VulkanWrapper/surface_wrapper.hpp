#pragma once

#include "../vulkan_header.hpp"

#include "instance_wrapper.hpp"

#if defined __linux__

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

#elif defined _WIN32 || defined WIN32

#include <memory>
#include <windows.h>

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
            Surface(HINSTANCE* hinstance_ptr, HWND* hwnd_ptr, std::shared_ptr<Instance> instance_ptr);
            ~Surface();
        };
    } // namespace Vulkan
} // namespace CascadeGraphics

#endif