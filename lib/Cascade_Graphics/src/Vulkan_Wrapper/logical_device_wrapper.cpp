#include "logical_device_wrapper.hpp"

#include "debug_tools.hpp"
#include <set>
#include <vector>

namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        Logical_Device_Wrapper::Logical_Device_Wrapper(std::shared_ptr<Physical_Device_Wrapper> physical_device_wrapper_ptr, std::shared_ptr<Queue_Manager> queue_manager_ptr)
        {
            LOG_INFO << "Vulkan Backend: Creating logical device";

            float queue_priority = 1.0f;

            std::set<const char*> required_extensions_set = physical_device_wrapper_ptr->Get_Required_Extensions();
            std::vector<const char*> required_extensions(required_extensions_set.begin(), required_extensions_set.end());
            std::vector<VkDeviceQueueCreateInfo> device_queue_create_infos = queue_manager_ptr->Generate_Device_Queue_Create_Infos(&queue_priority);

            VkDeviceCreateInfo device_create_info = {};
            device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            device_create_info.pNext = nullptr;
            device_create_info.flags = 0;
            device_create_info.queueCreateInfoCount = static_cast<uint32_t>(device_queue_create_infos.size());
            device_create_info.pQueueCreateInfos = device_queue_create_infos.data();
            device_create_info.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
            device_create_info.ppEnabledExtensionNames = required_extensions.data();

#if defined CSD_VULKAN_ENABLE_DEBUG_LAYERS
            std::vector<const char*> enabled_validation_layers = Validation_Layer_Wrapper::Get_Enabled_Validation_Layers();

            device_create_info.enabledLayerCount = static_cast<uint32_t>(enabled_validation_layers.size());
            device_create_info.ppEnabledLayerNames = enabled_validation_layers.data();
#else
            device_create_info.enabledLayerCount = 0;
            device_create_info.ppEnabledLayerNames = nullptr;
#endif

            VALIDATE_VKRESULT(vkCreateDevice(*physical_device_wrapper_ptr->Get_Physical_Device(), &device_create_info, nullptr, &m_device), "Vulkan Backend: Failed to create logical device");
            queue_manager_ptr->Get_Device_Queue_Handles(&m_device);

            LOG_TRACE << "Vulkan Backend: Finished creating logical device";
        }

        Logical_Device_Wrapper::~Logical_Device_Wrapper()
        {
            LOG_INFO << "Vulkan Backend: Destroying logical device";

            vkDestroyDevice(m_device, nullptr);

            LOG_TRACE << "Vulkan Backend: Finished destroying logical device";
        }

        VkDevice* Logical_Device_Wrapper::Get_Device()
        {
            return &m_device;
        }
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics
