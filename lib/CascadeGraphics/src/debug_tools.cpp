#include "cascade_logging.hpp"

#include "VulkanWrapper/instance_wrapper.hpp"

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
            LOG_INFO << "Checking vulkan support";

            std::vector<const char*> required_extensions = CascadeGraphics::Vulkan::Instance::Get_Required_Instance_Extensions();
            std::vector<bool> extensions_satisfied(required_extensions.size());

            unsigned int extension_count = Get_Supported_Extension_Count();
            std::vector<VkExtensionProperties> supported_extensions(extension_count);
            vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, supported_extensions.data());

            for (unsigned int i = 0; i < supported_extensions.size(); i++)
            {
                LOG_TRACE << "Extension supported: " << supported_extensions[i].extensionName;

                for (unsigned int j = 0; j < required_extensions.size(); j++)
                {
                    if (strcmp(required_extensions[j], supported_extensions[i].extensionName) == 0)
                    {
                        extensions_satisfied[j] = true;
                    }
                }
            }

            bool vulkan_supported = true;
            for (unsigned int i = 0; i < required_extensions.size(); i++)
            {
                if (!extensions_satisfied[i])
                {
                    vulkan_supported = false;
                    LOG_ERROR << "Missing support for vulkan extension: " << required_extensions[i];
                }
            }

            if (vulkan_supported)
            {
                LOG_DEBUG << "This device has all the required vulkan extensions";
            }
            else
            {
                LOG_FATAL << "This device is missing a required vulkan extension!";
                exit(EXIT_FAILURE);
            }

            return vulkan_supported;
        }
    } // namespace Vulkan
} // namespace CascadeGraphicsDebugging