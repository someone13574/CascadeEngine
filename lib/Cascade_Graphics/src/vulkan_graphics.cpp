#include "vulkan_graphics.hpp"

#include "Vulkan_Backend/instance.hpp"
#include "Vulkan_Backend/instance_builder.hpp"

namespace Cascade_Graphics
{
    Vulkan_Graphics::Vulkan_Graphics()
    {
        m_instance_ptr = Vulkan::Instance_Builder().Set_Application_Details("test-application", 0).Set_Engine_Details("Cascade", 0).Set_Minimum_Vulkan_Version(VK_VERSION_1_0).Build();
    }

    Vulkan_Graphics::~Vulkan_Graphics()
    {
        delete m_instance_ptr;
    }
} // namespace Cascade_Graphics