#include "physical_device_wrapper.hpp"

#include "cascade_logging.hpp"

#include <string>

namespace CascadeGraphics
{
    namespace Vulkan
    {
        Physical_Device::Physical_Device(std::shared_ptr<Instance> instance_ptr, std::shared_ptr<Queue_Manager> queue_manager_ptr) : m_queue_manager_ptr(queue_manager_ptr)
        {
            LOG_INFO << "Vulkan: choosing physical device";

            unsigned int device_count = 0;
            vkEnumeratePhysicalDevices(*(instance_ptr->Get_Instance()), &device_count, nullptr);
            LOG_DEBUG << "Vulkan: " << device_count << " physical device(s) found";

            if (device_count == 0)
            {
                LOG_FATAL << "Vulkan: could not find a gpu with Vulkan support";
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
                LOG_FATAL << "Vulkan: could not find a suitable gpu";
                exit(EXIT_FAILURE);
            }
            else
            {
                LOG_DEBUG << "Vulkan: selected device " << best_rated_device_properties.deviceName;

                m_physical_device = best_rated_device;
            }
        }

        bool Physical_Device::Is_Device_Suitable(VkPhysicalDevice physical_device,
                                                 VkPhysicalDeviceProperties physical_device_properties,
                                                 VkPhysicalDeviceFeatures physical_device_features)
        {
            LOG_TRACE << "Vulkan: checking physical device " << physical_device_properties.deviceName << " suitablity";

            if (!m_queue_manager_ptr->Physical_Device_Has_Required_Queues(physical_device))
            {
                return false;
            }

            return true;
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
    } // namespace Vulkan
} // namespace CascadeGraphics
