#include "cascade_logging.hpp"

#include <vulkan/vulkan.hpp>

namespace CascadeGraphicsDebugging
{
    unsigned int Get_Supported_Extension_Count()
    {
        unsigned int extension_count = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

        LOG_DEBUG << "This device supports " << extension_count << " vulkan extensions.";

        return extension_count;
    }
}