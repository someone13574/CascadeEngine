#pragma once

#include "../platform_info.hpp"
#include "instance.hpp"
#include "surface.hpp"

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class WIN32_Surface : public Surface
        {
        public:
            WIN32_Surface(Window_Info* window_info_ptr, Instance* instance_ptr);
            virtual ~WIN32_Surface() = default;

            static const char* Get_Surface_Extension_Name();
        };
    }    // namespace Vulkan
}    // namespace Cascade_Graphics