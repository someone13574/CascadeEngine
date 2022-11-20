#pragma once

#include "Vulkan_Backend/instance.hpp"
#include "graphics.hpp"

namespace Cascade_Graphics
{
    class Vulkan_Graphics : public Graphics
    {
        friend class Vulkan_Graphics_Factory;

    private:
        Vulkan::Instance* m_instance_ptr;

    private:
        Vulkan_Graphics();

    public:
        ~Vulkan_Graphics();
    };
} // namespace Cascade_Graphics