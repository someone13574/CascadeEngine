#include "device_wrapper.hpp"

#include "cascade_logging.hpp"

#include <set>
#include <vector>

namespace CascadeGraphics
{
    namespace Vulkan
    {
        Device::Device(std::shared_ptr<Queue_Manager> queue_manager_ptr,
                       std::shared_ptr<CascadeGraphicsDebugging::Vulkan::Validation_Layer> validation_layer_ptr,
                       std::shared_ptr<Physical_Device> physical_device_ptr)
        {
            LOG_INFO << "Vulkan: creating logical device";

            std::vector<VkDeviceQueueCreateInfo> queue_create_infos = queue_manager_ptr->Generate_Queue_Create_Infos();

            VkPhysicalDeviceFeatures physical_device_features = {};

            VkDeviceCreateInfo device_create_info = {};
            device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            device_create_info.pNext = NULL;
            device_create_info.flags = 0;
            device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
            device_create_info.pQueueCreateInfos = queue_create_infos.data();
#if defined CASCADE_ENABLE_DEBUG_LAYERS
            std::vector<const char*> enabled_validation_layers = CascadeGraphicsDebugging::Vulkan::Validation_Layer::Get_Enabled_Validation_Layers();
            device_create_info.enabledLayerCount = static_cast<uint32_t>(enabled_validation_layers.size());
            device_create_info.ppEnabledLayerNames = enabled_validation_layers.data();
#else
            device_create_info.enabledLayerCount = 0;
            device_create_info.ppEnabledLayerNames = NULL;
#endif
            device_create_info.enabledExtensionCount = 0;
            device_create_info.ppEnabledExtensionNames = NULL;

            VkResult device_creation_result = vkCreateDevice(*(physical_device_ptr->Get_Physical_Device()), &device_create_info, nullptr, &m_device);
            if (device_creation_result != VK_SUCCESS)
            {
                LOG_FATAL << "Vulkan: failed to create logical device with VkResult " << device_creation_result;
                exit(EXIT_FAILURE);
            }

            queue_manager_ptr->Get_Device_Queue_Handles(&m_device);

            LOG_TRACE << "Vulkan: finished creating logical device";
        }

        Device::~Device()
        {
            LOG_INFO << "Vulkan: destroying logical device";

            vkDestroyDevice(m_device, nullptr);

            LOG_TRACE << "Vulkan: finished destroying logical device";
        }

        VkDevice* Device::Get_Device()
        {
            return &m_device;
        }
    } // namespace Vulkan
} // namespace CascadeGraphics
