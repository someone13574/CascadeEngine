#include "cascade_logging.hpp"

#include <iomanip>
#include <string.h>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace CascadeGraphicsDebugging
{
    namespace Vulkan
    {
        unsigned int Get_Supported_Extension_Count()
        {
            unsigned int extension_count = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

            LOG_DEBUG << "This device supports " << extension_count << " vulkan extensions.";

            return extension_count;
        }

        bool Is_Vulkan_Supported()
        {
            unsigned int extension_count = Get_Supported_Extension_Count();
            std::vector<VkExtensionProperties> extensions(extension_count);

            vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

            bool vulkan_supported = false;

            for (unsigned int i = 0; i < extensions.size(); i++)
            {
                LOG_INFO << "Extension supported:     Spec Version: " << std::setw(2) << extensions[i].specVersion << "     Extension: " << extensions[i].extensionName;

#if defined __linux__
                vulkan_supported |= strcmp(extensions[i].extensionName, "VK_KHR_xcb_surface") == 0;
#endif
            }

            if (vulkan_supported)
            {
                LOG_DEBUG << "Vulkan is supported on this device.";
            }
            else
            {
                LOG_ERROR << "Vulkan is not supported on this device.";
            }

            return vulkan_supported;
        }
    } // namespace Vulkan
} // namespace CascadeGraphicsDebugging