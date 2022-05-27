#include "physical_device_wrapper.hpp"

#include "cascade_logging.hpp"

#include "swapchain_wrapper.hpp"

#include <cstring>
#include <string>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Physical_Device::Physical_Device(std::shared_ptr<Instance> instance_ptr, std::shared_ptr<Queue_Manager> queue_manager_ptr, std::shared_ptr<Surface> surface_ptr) : m_queue_manager_ptr(queue_manager_ptr), m_surface_ptr(surface_ptr)
        {
            LOG_INFO << "Vulkan: Choosing physical device";

            unsigned int device_count = 0;
            vkEnumeratePhysicalDevices(*(instance_ptr->Get_Instance()), &device_count, nullptr);
            LOG_DEBUG << "Vulkan: " << device_count << " physical device(s) found";

            if (device_count == 0)
            {
                LOG_FATAL << "Vulkan: Could not find a gpu with Vulkan support";
                exit(EXIT_FAILURE);
            }

            std::vector<VkPhysicalDevice> physical_devices(device_count);
            vkEnumeratePhysicalDevices(*(instance_ptr->Get_Instance()), &device_count, physical_devices.data());

            unsigned int best_device_rating = 0;
            VkPhysicalDevice best_rated_device = VK_NULL_HANDLE;
            VkPhysicalDeviceProperties best_rated_device_properties;

            for (unsigned int i = 0; i < physical_devices.size(); i++)
            {
                VkPhysicalDeviceProperties physical_device_properties;
                VkPhysicalDeviceFeatures physical_device_features;
                vkGetPhysicalDeviceProperties(physical_devices[i], &physical_device_properties);
                vkGetPhysicalDeviceFeatures(physical_devices[i], &physical_device_features);

                if (Is_Device_Suitable(physical_devices[i], physical_device_properties, physical_device_features))
                {
                    unsigned int device_rating = Rate_Device(physical_device_properties);

                    if (device_rating > best_device_rating)
                    {
                        best_device_rating = device_rating;
                        best_rated_device = physical_devices[i];
                        best_rated_device_properties = physical_device_properties;
                    }
                }
            }

            if (best_rated_device == VK_NULL_HANDLE)
            {
                LOG_FATAL << "Vulkan: Could not find a suitable gpu";
                exit(EXIT_FAILURE);
            }

            LOG_DEBUG << "Vulkan: Selected device " << best_rated_device_properties.deviceName;
            m_physical_device = best_rated_device;
        }

        bool Physical_Device::Is_Device_Suitable(VkPhysicalDevice physical_device, VkPhysicalDeviceProperties physical_device_properties, VkPhysicalDeviceFeatures physical_device_features)
        {
            LOG_INFO << "Vulkan: Checking physical device " << physical_device_properties.deviceName << " suitablity";

            if (!m_queue_manager_ptr->Physical_Device_Has_Required_Queues(physical_device))
            {
                return false;
            }

            if (!Check_Device_Extension_Support(physical_device))
            {
                return false;
            }

            if (!Swapchain::Is_Swapchain_Adequate(&physical_device, m_surface_ptr))
            {
                return false;
            }
            return true;
        }

        bool Physical_Device::Check_Device_Extension_Support(VkPhysicalDevice physical_device)
        {
            LOG_TRACE << "Vulkan: Checking physical device extension support";

            std::vector<const char*> required_extensions = Get_Required_Extensions();

            if (required_extensions.empty())
            {
                return true;
            }

            unsigned int extension_count;
            vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr);
            LOG_DEBUG << "Vulkan: Found " << extension_count << " physical device extensions";

            if (extension_count == 0)
            {
                return false;
            }

            std::vector<VkExtensionProperties> available_extensions(extension_count);
            vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, available_extensions.data());

            for (unsigned int i = 0; i < available_extensions.size(); i++)
            {
                LOG_TRACE << "Vulkan: Found physical device extension " << available_extensions[i].extensionName;
            }

            bool device_supports_extensions = true;
            for (unsigned int i = 0; i < required_extensions.size(); i++)
            {
                bool extension_found = false;
                for (unsigned int j = 0; j < available_extensions.size(); j++)
                {
                    extension_found |= strcmp(required_extensions[i], available_extensions[j].extensionName) == 0;
                }

                if (!extension_found)
                {
                    LOG_INFO << "Vulkan: Physical device is missing extension " << required_extensions[i];
                    device_supports_extensions = false;
                }
            }

            if (!device_supports_extensions)
            {
                LOG_TRACE << "Vulkan: This physical device is missing one or more required extensions";
            }
            else
            {
                LOG_TRACE << "Vulkan: This physical device has all required extensions";
            }

            return device_supports_extensions;
        }

        unsigned int Physical_Device::Rate_Device(VkPhysicalDeviceProperties physical_device_properties)
        {
            unsigned int rating = 0;

            if (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                rating += 1000;
            }

            rating += physical_device_properties.limits.maxImageDimension2D;

            LOG_TRACE << "Vulkan: " << physical_device_properties.deviceName << " has a rating of " << rating;

            return rating;
        }

        VkPhysicalDevice* Physical_Device::Get_Physical_Device()
        {
            return &m_physical_device;
        }

        std::vector<const char*> Physical_Device::Get_Required_Extensions()
        {
            std::vector<const char*> required_extensions;

            required_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

            return required_extensions;
        }
    } // namespace Vulkan
} // namespace Cascade_Graphics
