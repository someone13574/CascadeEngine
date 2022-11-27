#include "physical_device.hpp"

#include <acorn_logging.hpp>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Physical_Device::Physical_Device(VkPhysicalDevice physical_device) : m_physical_device(physical_device)
        {
            LOG_TRACE << "Graphics (Vulkan): Getting physical device properties";
            vkGetPhysicalDeviceProperties(m_physical_device, &m_physical_device_properties);

            LOG_TRACE << "Graphics (Vulkan): Getting physical device features";
            vkGetPhysicalDeviceFeatures(m_physical_device, &m_physical_device_features);
        }

        VkPhysicalDevice* Physical_Device::Get()
        {
            return &m_physical_device;
        }

        VkPhysicalDeviceProperties* Physical_Device::Get_Properties()
        {
            return &m_physical_device_properties;
        }

        VkPhysicalDeviceFeatures* Physical_Device::Get_Features()
        {
            return &m_physical_device_features;
        }
    } // namespace Vulkan
} // namespace Cascade_Graphics