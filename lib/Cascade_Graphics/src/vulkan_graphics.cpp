#include "vulkan_graphics.hpp"

#include "Vulkan_Backend/instance_builder.hpp"
#include "Vulkan_Backend/physical_device_selector.hpp"
#include "Vulkan_Backend/win32_surface.hpp"
#include "Vulkan_Backend/xcb_surface.hpp"
#include <acorn_logging.hpp>

namespace Cascade_Graphics
{
    Vulkan_Graphics::Vulkan_Graphics(Window_Platform window_platform)
    {
        LOG_INFO << "Graphics: Initializing graphics with Vulkan Backend";

        const char* platform_surface_extension;
        if (window_platform == Window_Platform::LINUX_XCB)
        {
            platform_surface_extension = Vulkan::XCB_Surface::Get_Surface_Extension_Name();
        }
        else if (window_platform == Window_Platform::WINDOWS_WIN32)
        {
            platform_surface_extension = Vulkan::WIN32_Surface::Get_Surface_Extension_Name();
        }
        else
        {
            LOG_FATAL << "Graphics (Vulkan): Unknown window platform";
            exit(EXIT_FAILURE);
        }

        m_instance_ptr = Vulkan::Instance_Builder()
                             .Set_Application_Details("test-application", 0)
                             .Set_Engine_Details("Cascade", 0)
                             .Set_Minimum_Vulkan_Version(VK_API_VERSION_1_0)
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