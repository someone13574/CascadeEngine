#pragma once

#include "graphics_factory.hpp"

namespace Cascade_Graphics
{
    class Vulkan_Graphics_Factory : public Graphics_Factory
    {
    public:
        Vulkan_Graphics_Factory(Window_Platform window_platform);

        Graphics* Create_Graphics() const override;
        Renderer* Create_Renderer(Graphics* graphics_ptr, Window_Info* window_info_ptr) const override;
    };
} // namespace Cascade_Graphics