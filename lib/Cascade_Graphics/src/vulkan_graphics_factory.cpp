#include "vulkan_graphics_factory.hpp"

namespace Cascade_Graphics
{
    Graphics* Vulkan_Graphics_Factory::Create_Graphics() const
    {
    }

    Renderer* Vulkan_Graphics_Factory::Create_Renderer(Graphics* graphics_ptr) const
    {
        return new Vulkan_Renderer(graphics_ptr);
    }
} // namespace Cascade_Graphics