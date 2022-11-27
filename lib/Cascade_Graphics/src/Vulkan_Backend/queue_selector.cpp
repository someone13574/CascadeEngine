#include "queue_selector.hpp"

#include "vkresult_translator.hpp"
#include <acorn_logging.hpp>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Queue_Selector::Queue_Selector(Physical_Device* physical_device_ptr) : m_physical_device_ptr(physical_device_ptr)
        {
            LOG_DEBUG << "Graphics (Vulkan): Getting available queues for physical device '" << m_physical_device_ptr->Get_Properties()->deviceName << "'";

            uint32_t queue_family_count;
            vkGetPhysicalDeviceQueueFamilyProperties(*m_physical_device_ptr->Get(), &queue_family_count, nullptr);

            std::vector<VkQueueFamilyProperties> queue_family_properties(queue_family_count);
            vkGetPhysicalDeviceQueueFamilyProperties(*m_physical_device_ptr->Get(), &queue_family_count, queue_family_properties.data());

            for (uint32_t queue_family_index = 0; queue_family_index < queue_family_count; queue_family_index++)
            {
                Queue_Family_Filter_Info filter_info = {};
                filter_info.queue_family_index = queue_family_index;
                filter_info.queue_family_properties = queue_family_properties[queue_family_index];

                m_queue_filter_infos.push_back(filter_info);
            }
        }

        Queue_Selector& Queue_Selector::Require_Queue_Type(VkQueueFlagBits queue_type)
        {
            for (int32_t queue_family_index = m_queue_filter_infos.size() - 1; queue_family_index >= 0; queue_family_index--)
            {
                if ((m_queue_filter_infos[queue_family_index].queue_family_properties.queueFlags & queue_type) != queue_type)
                {
                    m_queue_filter_infos.erase(m_queue_filter_infos.begin() + queue_family_index);
                }
            }

            return *this;
        }

        bool Queue_Selector::Meets_Requirements()
        {
            return !m_queue_filter_infos.empty();
        }
    } // namespace Vulkan
} // namespace Cascade_Graphics