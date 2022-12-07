#include "vulkan_graphics.hpp"

#include "Vulkan_Backend/instance_builder.hpp"
#include "Vulkan_Backend/physical_device_selector.hpp"
#include <acorn_logging.hpp>

namespace Cascade_Graphics
{
    Vulkan_Graphics::Vulkan_Graphics()
    {
        LOG_INFO << "Graphics: Initializing graphics with Vulkan Backend";

#ifdef __linux__
        const char* platform_surface_extension = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
#elif defined _WIN32
        const char* platform_surface_extension = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#endif

        m_instance_ptr = Vulkan::Instance_Builder()
                             .Set_Application_Details("test-application", 0)
                             .Set_Engine_Details("Cascade", 0)
                             .Set_Minimum_Vulkan_Version(VK_API_VERSION_1_3)
                             .Add_Layer("VK_LAYER_KHRONOS_validation")
                             .Add_Extension(VK_KHR_SURFACE_EXTENSION_NAME)
                             .Add_Extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)
                             .Add_Extension(platform_surface_extension)
                             .Build();

        m_physical_device_ptr = Vulkan::Physical_Device_Selector(m_instance_ptr)
                                    .Require_Queue_Type("render_queue", VK_QUEUE_COMPUTE_BIT, 1, 1.0)
                                    .Require_Extension(VK_KHR_SWAPCHAIN_EXTENSION_NAME)
                                    .Require_Extension(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME)
                                    .Prefer_Dedicated(1000.0)
                                    .Best();

        m_device_ptr = new Vulkan::Device(m_physical_device_ptr);

        LOG_INFO << "Graphics: Finished initializing graphics with Vulkan backend";
    }

    Vulkan_Graphics::~Vulkan_Graphics()
    {
        LOG_INFO << "Graphics: Destroying Vulkan objects";

        delete m_device_ptr;
        delete m_physical_device_ptr;
        delete m_instance_ptr;

        LOG_INFO << "Graphics: Finished destroying Vulkan objects";
    }
} // namespace Cascade_Graphics