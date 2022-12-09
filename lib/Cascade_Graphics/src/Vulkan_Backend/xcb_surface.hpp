#pragma once

#include "../window_info.hpp"
#include "instance.hpp"
#include "surface.hpp"

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class XCB_Surface : public Surface
        {
        public:
            XCB_Surface(Window_Info* window_info_ptr, Instance* instance_ptr);
            virtual ~XCB_Surface() = default;
        };
    } // namespace Vulkan
} // namespace Cascade_Graphics