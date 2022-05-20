#pragma once

#include "../vulkan_header.hpp"
#include "instance_wrapper.hpp"

#include <memory>

#if defined __linux__
#include <xcb/xcb.h>
#elif defined _WIN32 || defined WIN32
#include <windows.h>
#endif

namespace CascadeGraphics
{
    namespace Vulkan
    {
        class Surface
        {
        public:
#if defined __linux__
            struct Window_Data
            {
                xcb_connection_t* connection_ptr;
                xcb_window_t* window_ptr;
            };
#elif defined _WIN32 || defined WIN32
            struct Window_Data
            {
                HINSTANCE* hinstance_ptr;
                HWND* hwindow_ptr;
            };
#endif
        private:
            VkSurfaceKHR m_surface;

            std::shared_ptr<Instance> m_instance_ptr;

        public:
            Surface(Window_Data window_data, std::shared_ptr<Instance> instance_ptr);
            ~Surface();

        public:
            VkSurfaceKHR* Get_Surface();
        };
    } // namespace Vulkan
} // namespace CascadeGraphics