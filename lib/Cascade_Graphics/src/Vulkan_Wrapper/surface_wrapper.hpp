#pragma once

#include "../window_information.hpp"
#include "instance_wrapper.hpp"
#include "vulkan_header.hpp"
#include <memory>


namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        class Surface_Wrapper
        {
        private:
            VkSurfaceKHR m_surface;

            std::shared_ptr<Instance_Wrapper> m_instance_wrapper_ptr;

        public:
            Surface_Wrapper(std::shared_ptr<Instance_Wrapper> instance_wrapper_ptr, Window_Information window_data);
            ~Surface_Wrapper();

        public:
            VkSurfaceKHR* Get_Surface();
        };
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics