#pragma once

#include "graphics_factory.hpp"

namespace Cascade_Graphics
{
    class Vulkan_Graphics_Factory : public Graphics_Factory
    {
    public:
        Vulkan_Graphics_Factory(Platform platform);

        Graphics* Create_Graphics(Platform_Info* platform_info) const override;
        Renderer* Create_Renderer(Graphics* graphics_ptr, Window_Info* window_info_ptr) const override;
    };
}    // namespace Cascade_Graphics