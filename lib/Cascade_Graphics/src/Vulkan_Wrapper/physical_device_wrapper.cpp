#include "physical_device_wrapper.hpp"

#include "cascade_logging.hpp"
#include "swapchain_wrapper.hpp"
#include <cstring>
#include <string>


namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        Physical_Device_Wrapper::Physical_Device_Wrapper(std::shared_ptr<Instance_Wrapper> instance_wrapper_ptr,
                                                         std::shared_ptr<Queue_Manager> queue_manager_ptr,
                                                         std::shared_ptr<Surface_Wrapper> surface_wrapper_ptr,
                                                         std::set<const char*> required_extensions)
            : m_queue_manager_ptr(queue_manager_ptr), m_surface_wrapper_ptr(surface_wrapper_ptr), m_required_extensions(required_extensions)
        {
            LOG_INFO << "Vulkan Backend: Choosing physical device";

            m_required_extensions.insert(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

            uint32_t device_count = 0;
            vkEnumeratePhysicalDevices(*instance_wrapper_ptr->Get_Instance(), &device_count, nullptr);
            LOG_DEBUG << "Vulkan Backend: " << device_count << " physical device(s) found";

            if (device_count == 0)
            {
                LOG_FATAL << "Vulkan Backend: Could not find a GPU with Vulkan support";
                exit(EXIT_FAILURE);
            }

            std::vector<VkPhysicalDevice> physical_devices(device_count);
            vkEnumeratePhysicalDevices(*instance_wrapper_ptr->Get_Instance(), &device_count, physical_devices.data());

            uint32_t best_device_rating = 0;
            VkPhysicalDevice best_rated_device = VK_NULL_HANDLE;
            VkPhysicalDeviceProperties best_rated_device_properties;

            for (uint32_t i = 0; i < physical_devices.size(); i++)
            {
                VkPhysicalDeviceProperties physical_device_properties;
                VkPhysicalDeviceFeatures physical_device_features;
                vkGetPhysicalDeviceProperties(physical_devices[i], &physical_device_properties);
                vkGetPhysicalDeviceFeatures(physical_devices[i], &physical_device_features);

                if (Does_Device_Meet_Requirements(physical_devices[i], physical_device_properties, physical_device_features))
                {
                    uint32_t device_rating = Rate_Physical_Device(physical_device_properties);

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
                LOG_ERROR << "Vulkan Backend: Could not find a GPU which meets feature requirements";
                exit(EXIT_FAILURE);
            }

            LOG_DEBUG << "Vulkan Backend: Selected physical device '" << best_rated_device_properties.deviceName << "'";
            m_physical_device = best_rated_device;
            m_queue_manager_ptr->Get_Queue_Families(&m_physical_device);
        }

        bool Physical_Device_Wrapper::Does_Device_Meet_Requirements(VkPhysicalDevice physical_device, VkPhysicalDeviceProperties physical_device_properties, VkPhysicalDeviceFeatures physical_device_features)
        {
            LOG_INFO << "Vulkan Backend: Checking whether physical device '" << physical_device_properties.deviceName << "' meets feature requirements";

            if (!m_queue_manager_ptr->Has_Required_Queues(&physical_device))
            {
                LOG_INFO << "Vulkan Backend: Physical device '" << physical_device_properties.deviceName << "' is missing a required feature";
                return false;
            }

            if (!Check_Device_Extension_Support(physical_device))
            {
                LOG_INFO << "Vulkan Backend: Physical device '" << physical_device_properties.deviceName << "' is missing a required feature";
                return false;
            }

            if (!Swapchain_Wrapper::Is_Swapchain_Adequate(&physical_device, m_surface_wrapper_ptr))
            {
                LOG_INFO << "Vulkan Backend: Physical device '" << physical_device_properties.deviceName << "' is missing a required feature";
                return false;
            }
            LOG_INFO << "Vulkan Backend: Physical device '" << physical_device_properties.deviceName << "' has all required features";
            return true;
        }

        bool Physical_Device_Wrapper::Check_Device_Extension_Support(VkPhysicalDevice physical_device)
        {
            LOG_TRACE << "Vulkan Backend: Checking physical device extension support";

            if (m_required_extensions.empty())
            {
                return true;
            }

            uint32_t extension_count;
            vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr);
            LOG_TRACE << "Vulkan Backend: Found " << extension_count << " physical device extensions";

            std::vector<VkExtensionProperties> available_extensions(extension_count);
            vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, available_extensions.data());

            for (uint32_t i = 0; i < available_extensions.size(); i++)
            {
                LOG_TRACE << "Vulkan Backend: Found physical device extension " << available_extensions[i].extensionName;
            }

            bool device_supports_extensions = true;
            for (const char* extension : m_required_extensions)
            {
                bool extension_found = false;
                for (uint32_t i = 0; i < available_extensions.size(); i++)
                {
                    extension_found |= strcmp(extension, available_extensions[i].extensionName) == 0;
                }

                if (!extension_found)
                {
                    LOG_INFO << "Vulkan Backend: Physical device is missing extension " << extension;
                    device_supports_extensions = false;
                }
            }

            if (!device_supports_extensions)
            {
                LOG_TRACE << "Vulkan Backend: This physical device is missing one or more required extensions";
            }
            else
            {
                LOG_TRACE << "Vulkan Backend: This physical device has all required extensions";
            }

            return device_supports_extensions;
        }

        uint32_t Physical_Device_Wrapper::Rate_Physical_Device(VkPhysicalDeviceProperties physical_device_properties)
        {
            uint32_t rating = 0;

            if (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                rating += 1000;
            }

            rating += physical_device_properties.limits.maxImageDimension2D;

            LOG_TRACE << "Vulkan Backend: Physical device '" << physical_device_properties.deviceName << "' has a rating of " << rating;

            return rating;
        }

        VkPhysicalDevice* Physical_Device_Wrapper::Get_Physical_Device()
        {
            return &m_physical_device;
        }

        std::set<const char*> Physical_Device_Wrapper::Get_Required_Extensions()
        {
            return m_required_extensions;
        }
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics
