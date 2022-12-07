#include "device.hpp"

#include "vkresult_translator.hpp"
#include <acorn_logging.hpp>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Device::Device(Physical_Device* physical_device_ptr)
        {
            LOG_INFO << "Graphics (Vulkan): Creating device from physical device '" << physical_device_ptr->Get_Properties()->deviceName << "'";

            Queue_Set queue_set = physical_device_ptr->Get_Queue_Set();
            std::vector<VkDeviceQueueCreateInfo> device_queue_create_informations = Get_Queue_Create_Information(&queue_set);
            std::vector<const char*> enabled_device_extensions = physical_device_ptr->Get_Device_Extensions();

            VkDeviceCreateInfo device_create_info = {};
            device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            device_create_info.pNext = nullptr;
            device_create_info.flags = 0;
            device_create_info.queueCreateInfoCount = static_cast<uint32_t>(device_queue_create_informations.size());
            device_create_info.pQueueCreateInfos = device_queue_create_informations.data();
            device_create_info.enabledLayerCount = 0;
            device_create_info.ppEnabledLayerNames = nullptr;
            device_create_info.enabledExtensionCount = static_cast<uint32_t>(enabled_device_extensions.size());
            device_create_info.ppEnabledExtensionNames = enabled_device_extensions.data();
            device_create_info.pEnabledFeatures = nullptr;

            VkResult create_device_result = vkCreateDevice(*physical_device_ptr->Get(), &device_create_info, nullptr, &m_device);
            if (create_device_result != VK_SUCCESS)
            {
                LOG_FATAL << "Graphics (Vulkan): Failed to create device with VkResult " << create_device_result << " (" << Translate_VkResult(create_device_result) << ")";
                exit(EXIT_FAILURE);
            }
        }

        Device::~Device()
        {
            LOG_DEBUG << "Graphics (Vulkan): Destroying device";

            vkDestroyDevice(m_device, nullptr);
        }

        std::vector<VkDeviceQueueCreateInfo> Device::Get_Queue_Create_Information(Queue_Set* queue_set_ptr)
        {
            LOG_TRACE << "Graphics (Vulkan): Generating device queue create information";

            std::vector<VkDeviceQueueCreateInfo> device_queue_create_informations;

            for (uint32_t queue_provider_index = 0; queue_provider_index < queue_set_ptr->queue_providers.size(); queue_provider_index++)
            {
                for (uint32_t queue_index = 0; queue_index < queue_set_ptr->queue_providers[queue_provider_index].usage.queue_count; queue_index++)
                {
                    VkDeviceQueueCreateInfo device_queue_create_info = {};
                    device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                    device_queue_create_info.pNext = nullptr;
                    device_queue_create_info.flags = 0;
                    device_queue_create_info.queueFamilyIndex = queue_set_ptr->queue_providers[queue_provider_index].usage.queue_family_index;
                    device_queue_create_info.queueCount = 1;
                    device_queue_create_info.pQueuePriorities = &queue_set_ptr->queue_providers[queue_provider_index].requirement.queue_priority;

                    device_queue_create_informations.push_back(device_queue_create_info);
                }
            }

            return device_queue_create_informations;
        }

        VkDevice* Device::Get()
        {
            return &m_device;
        }
    } // namespace Vulkan
} // namespace Cascade_Graphics