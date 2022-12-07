#include "physical_device_selector.hpp"

#include "vkresult_translator.hpp"
#include <acorn_logging.hpp>
#include <algorithm>
#include <string>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Physical_Device_Selector::Physical_Device_Selector(Instance* instance_ptr) : m_instance_ptr(instance_ptr)
        {
            LOG_INFO << "Graphics (Vulkan): Selecting physical device";
            LOG_DEBUG << "Graphics (Vulkan): Getting available physical devices";

            uint32_t physical_device_count;
            std::vector<VkPhysicalDevice> physical_devices;

            VkResult get_physical_device_count_result = vkEnumeratePhysicalDevices(*instance_ptr->Get(), &physical_device_count, nullptr);
            if (get_physical_device_count_result != VK_SUCCESS && get_physical_device_count_result != VK_INCOMPLETE)
            {
                LOG_FATAL << "Graphics (Vulkan): Failed to get number of physical devices with VkResult " << get_physical_device_count_result << " (" << Translate_VkResult(get_physical_device_count_result) << ")";
                exit(EXIT_FAILURE);
            }

            physical_devices.resize(physical_device_count);
            VkResult get_physical_devices_result = vkEnumeratePhysicalDevices(*instance_ptr->Get(), &physical_device_count, physical_devices.data());
            if (get_physical_devices_result != VK_SUCCESS && get_physical_devices_result != VK_INCOMPLETE)
            {
                LOG_FATAL << "Graphics (Vulkan): Failed to get available physical devices with VkResult " << get_physical_devices_result << " (" << Translate_VkResult(get_physical_devices_result) << ")";
                exit(EXIT_FAILURE);
            }

            LOG_DEBUG << "Graphics (Vulkan): Found " << physical_device_count << " physical device(s)";
            LOG_DEBUG << "Graphics (Vulkan): Getting information about available physical device(s)";

            for (uint32_t physical_device_index = 0; physical_device_index < physical_device_count; physical_device_index++)
            {
                Physical_Device_Filter_Info physical_device_filter_info = {};
                physical_device_filter_info.physical_device_ptr = new Physical_Device(physical_devices[physical_device_index]);
                physical_device_filter_info.queue_selector_ptr = nullptr;

                LOG_TRACE << "Graphics (Vulkan): Getting supported device extensions";

                uint32_t device_extension_count;
                VkResult get_extension_count_result = vkEnumerateDeviceExtensionProperties(*physical_device_filter_info.physical_device_ptr->Get(), nullptr, &device_extension_count, nullptr);
                if (get_extension_count_result != VK_SUCCESS)
                {
                    LOG_FATAL << "Graphics (Vulkan): Failed to get number of extensions for physical device '" << physical_device_filter_info.physical_device_ptr->Get_Properties()->deviceName << "' with VkResult " << get_extension_count_result
                              << " (" << Translate_VkResult(get_extension_count_result) << ")";
                    exit(EXIT_FAILURE);
                }

                physical_device_filter_info.available_extensions.resize(device_extension_count);
                VkResult get_extensions_result = vkEnumerateDeviceExtensionProperties(*physical_device_filter_info.physical_device_ptr->Get(), nullptr, &device_extension_count, physical_device_filter_info.available_extensions.data());
                if (get_extensions_result != VK_SUCCESS)
                {
                    LOG_FATAL << "Graphics (Vulkan): Failed to get available extensions for physical device '" << physical_device_filter_info.physical_device_ptr->Get_Properties()->deviceName << "' with VkResult " << get_extension_count_result
                              << " (" << Translate_VkResult(get_extension_count_result) << ")";
                    exit(EXIT_FAILURE);
                }

                physical_device_filter_info.score = 0;

                m_physical_device_filter_infos.push_back(physical_device_filter_info);
            }
        }

        Physical_Device_Selector& Physical_Device_Selector::Require_Queue_Type(std::string requirement_label, VkQueueFlagBits queue_type, uint32_t required_queue_count, float queue_priority)
        {
            for (int32_t physical_device_index = m_physical_device_filter_infos.size() - 1; physical_device_index >= 0; physical_device_index--)
            {
                if (m_physical_device_filter_infos[physical_device_index].queue_selector_ptr == nullptr)
                {
                    m_physical_device_filter_infos[physical_device_index].queue_selector_ptr = new Queue_Selector(m_physical_device_filter_infos[physical_device_index].physical_device_ptr);
                }

                if (!m_physical_device_filter_infos[physical_device_index].queue_selector_ptr->Add_Queue_Requirement(requirement_label, queue_type, required_queue_count, queue_priority).Meets_Requirements())
                {
                    LOG_TRACE << "Graphics (Vulkan): Physical device '" << m_physical_device_filter_infos[physical_device_index].physical_device_ptr->Get_Properties()->deviceName << "' cannot meet queue requirements";
                    m_physical_device_filter_infos.erase(m_physical_device_filter_infos.begin() + physical_device_index);
                }
                else
                {
                    m_physical_device_filter_infos[physical_device_index].physical_device_ptr->m_queue_set = m_physical_device_filter_infos[physical_device_index].queue_selector_ptr->Best();
                }
            }

            return *this;
        }

        Physical_Device_Selector& Physical_Device_Selector::Require_Extension(const char* extension_name)
        {
            LOG_DEBUG << "Graphics (Vulkan): Filtering physical devices for support of device extension '" << extension_name << "'";

            for (int32_t physical_device_index = m_physical_device_filter_infos.size() - 1; physical_device_index >= 0; physical_device_index--)
            {
                bool found_extension = false;
                for (uint32_t extension_index = 0; extension_index < m_physical_device_filter_infos[physical_device_index].available_extensions.size(); extension_index++)
                {
                    if (std::string(extension_name) == m_physical_device_filter_infos[physical_device_index].available_extensions[extension_index].extensionName)
                    {
                        found_extension = true;
                    }
                }

                if (!found_extension)
                {
                    LOG_TRACE << "Graphics (Vulkan): Physical device '" << m_physical_device_filter_infos[physical_device_index].physical_device_ptr->Get_Properties()->deviceName << "' doesn't support required extension '" << extension_name << "'";
                    m_physical_device_filter_infos.erase(m_physical_device_filter_infos.begin() + physical_device_index);
                }
                else
                {
                    LOG_TRACE << "Graphics (Vulkan): Physical device '" << m_physical_device_filter_infos[physical_device_index].physical_device_ptr->Get_Properties()->deviceName << "' supports required extension '" << extension_name << "'";
                    m_physical_device_filter_infos[physical_device_index].physical_device_ptr->m_enabled_device_extensions.push_back(extension_name);
                }
            }

            return *this;
        }

        Physical_Device_Selector& Physical_Device_Selector::Prefer_Dedicated(double add_score)
        {
            LOG_DEBUG << "Graphics (Vulkan): Adding score of " << add_score << " to dedicated devices";

            for (uint32_t physical_device_index = 0; physical_device_index < m_physical_device_filter_infos.size(); physical_device_index++)
            {
                if (m_physical_device_filter_infos[physical_device_index].physical_device_ptr->Get_Properties()->deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                {
                    LOG_TRACE << "Graphics (Vulkan): Physical device '" << m_physical_device_filter_infos[physical_device_index].physical_device_ptr->Get_Properties()->deviceName << "' is dedicated, +" << add_score;
                    m_physical_device_filter_infos[physical_device_index].score += add_score;
                }
                else
                {
                    LOG_TRACE << "Graphics (Vulkan): Physical device '" << m_physical_device_filter_infos[physical_device_index].physical_device_ptr->Get_Properties()->deviceName << "' isn't dedicated, +0";
                }
            }

            return *this;
        }

        Physical_Device* Physical_Device_Selector::Best()
        {
            LOG_DEBUG << "Graphics (Vulkan): Selecting best physical device";

            if (m_physical_device_filter_infos.empty())
            {
                LOG_FATAL << "Graphics (Vulkan): No physical devices meet requirements";
                exit(EXIT_FAILURE);
            }

            std::sort(m_physical_device_filter_infos.begin(), m_physical_device_filter_infos.end(),
                      [](const Physical_Device_Filter_Info& left_hand_size, const Physical_Device_Filter_Info& right_hand_side) { return left_hand_size.score > right_hand_side.score; });

            LOG_INFO << "Graphics (Vulkan): Selected physical device '" << m_physical_device_filter_infos.front().physical_device_ptr->Get_Properties()->deviceName << "'";

            return m_physical_device_filter_infos.front().physical_device_ptr;
        }
    } // namespace Vulkan
} // namespace Cascade_Graphics