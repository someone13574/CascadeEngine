#pragma once

#include "Vulkan_Backend/device.hpp"
#include "Vulkan_Backend/instance.hpp"
#include "Vulkan_Backend/physical_device.hpp"
#include "graphics.hpp"

namespace Cascade_Graphics
{
    class Vulkan_Graphics : public Graphics
    {
        friend class Vulkan_Graphics_Factory;
        friend class Vulkan_Renderer;

    private:
        Vulkan::Instance* m_instance_ptr;
        Vulkan::Physical_Device* m_physical_device_ptr;
        Vulkan::Device* m_device_ptr;

    private:
        Vulkan_Graphics();

    public:
        ~Vulkan_Graphics();
    };
} // namespace Cascade_Graphics