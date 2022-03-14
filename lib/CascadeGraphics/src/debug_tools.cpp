#include "cascade_logging.hpp"

#include "VulkanWrapper/instance_wrapper.hpp"
#include "vulkan_header.hpp"

#include <cstring>
#include <iomanip>
#include <vector>

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
            LOG_INFO << "Checking Vulkan support";

            std::vector<const char*> required_extensions = CascadeGraphics::Vulkan::Instance::Get_Required_Instance_Extensions();
            std::vector<bool> extensions_satisfied(required_extensions.size());

            unsigned int extension_count = Get_Supported_Extension_Count();
            std::vector<VkExtensionProperties> supported_extensions(extension_count);
            vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, supported_extensions.data());

            for (unsigned int i = 0; i < supported_extensions.size(); i++)
            {
                LOG_TRACE << "Vulkan extension supported: " << supported_extensions[i].extensionName;
            }

            bool vulkan_supported = true;
            for (unsigned int i = 0; i < required_extensions.size(); i++)
            {
                bool found_extension = false;
                for (unsigned int j = 0; j < supported_extensions.size(); j++)
                {
                    found_extension |= strcmp(required_extensions[i], supported_extensions[j].extensionName) == 0;
                }

                if (!found_extension)
                {
                    LOG_ERROR << "Missing support for Vulkan extension: " << required_extensions[i];
                    vulkan_supported = false;
                }
            }

            if (vulkan_supported)
            {
                LOG_DEBUG << "This device has all the required Vulkan extensions";
                return true;
            }
            else
            {
                LOG_FATAL << "This device is missing a required Vulkan extension!";
                exit(EXIT_FAILURE);
            }
        }
    } // namespace Vulkan
} // namespace CascadeGraphicsDebugging