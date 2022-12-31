#include "queue_selector.hpp"

#include "vkresult_translator.hpp"
#include <acorn_logging.hpp>
#include <algorithm>
#include <cmath>
#include <functional>

#ifdef __linux__
#include "../xcb_platform_info.hpp"
#include <xcb/xcb.h>
#endif

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Queue_Selector::Queue_Selector(Physical_Device* physical_device_ptr, Platform_Info* platform_info_ptr) : m_platform_info_ptr(platform_info_ptr), m_pyhsical_device_ptr(physical_device_ptr)
        {
            LOG_DEBUG << "Graphics (Vulkan): Getting available queue families for physical device '" << physical_device_ptr->Get_Properties()->deviceName << "'";

            uint32_t queue_family_count;
            vkGetPhysicalDeviceQueueFamilyProperties(*physical_device_ptr->Get(), &queue_family_count, nullptr);

            m_queue_families.resize(queue_family_count);
            m_queue_family_present_support.resize(queue_family_count);
            vkGetPhysicalDeviceQueueFamilyProperties(*physical_device_ptr->Get(), &queue_family_count, m_queue_families.data());

            Get_Queue_Family_Present_Support();
        }

        void Queue_Selector::Get_Queue_Family_Present_Support()
        {
            LOG_TRACE << "Graphics (Vulkan): Getting queue family present support";

#ifdef __linux__
            XCB_Platform_Info* xcb_platform_info_ptr = dynamic_cast<XCB_Platform_Info*>(m_platform_info_ptr);

            for (uint32_t queue_family_index = 0; queue_family_index < m_queue_families.size(); queue_family_index++)
            {
                m_queue_family_present_support[queue_family_index] = vkGetPhysicalDeviceXcbPresentationSupportKHR(*m_pyhsical_device_ptr->Get(), queue_family_index, *reinterpret_cast<xcb_connection_t**>(xcb_platform_info_ptr->Get_Connection()),
                                                                                                                  *reinterpret_cast<xcb_visualid_t*>(xcb_platform_info_ptr->Get_Visual_Id()));
            }
#endif
        }

        void Queue_Selector::Generate_Sets()
        {
            LOG_TRACE << "Graphics (Vulkan): Generating all possible queue sets";

            // Create all per requirement queue sets
            std::vector<std::vector<Queue_Set>> queue_sets_by_requirement;
            for (uint32_t requirement_index = 0; requirement_index < m_queue_requirements.size(); requirement_index++)
            {
                // Generate combinations of possible providers
                uint32_t provider_set_count = static_cast<uint32_t>(std::pow(2, m_queue_families.size()));
                std::vector<std::vector<uint32_t>> provider_sets; // Array of possible provider queue family sets

                for (uint32_t set_index = 1; set_index < provider_set_count; set_index++)
                {
                    provider_sets.resize(provider_sets.size() + 1);

                    // Evalute each bit in the set_index to determine whether a queue family is included
                    for (uint32_t queue_family_index = 0; queue_family_index < m_queue_families.size(); queue_family_index++)
                    {
                        bool include_queue_family = (set_index >> queue_family_index & 1) == 1;

                        if (include_queue_family)
                        {
                            provider_sets.back().push_back(queue_family_index);

                            // Check if set contains an unnessessary queue family
                            if ((m_queue_families[queue_family_index].queueFlags & m_queue_requirements[requirement_index].required_queue_type) != static_cast<VkQueueFlags>(m_queue_requirements[requirement_index].required_queue_type))
                            {
                                // Delete current set
                                provider_sets.pop_back();
                                break;
                            }
                        }
                    }
                }

                // Create combinations of queue usages
                std::vector<Queue_Set> provider_usage_sets; // Array of possible provider usages
                for (uint32_t provider_set_index = 0; provider_set_index < provider_sets.size(); provider_set_index++)
                {
                    // Get total number of possible combinations
                    uint32_t number_of_possible_combinations = 1;
                    for (uint32_t provider_index = 0; provider_index < provider_sets[provider_set_index].size(); provider_index++)
                    {
                        number_of_possible_combinations *= (std::min)(m_queue_requirements[requirement_index].required_queue_count + 1, m_queue_families[provider_sets[provider_set_index][provider_index]].queueCount + 1);
                    }

                    // Generate possible combinations
                    for (uint32_t combination_index = 0; combination_index < number_of_possible_combinations; combination_index++)
                    {
                        double period = 1.0;

                        provider_usage_sets.resize(provider_usage_sets.size() + 1);
                        provider_usage_sets.back().provided_queues = 0;
                        provider_usage_sets.back().queue_family_usage.resize(m_queue_families.size(), 0);
                        provider_usage_sets.back().queue_providers.clear();

                        for (uint32_t provider_index = 0; provider_index < provider_sets[provider_set_index].size(); provider_index++)
                        {
                            uint32_t provided_queue_count = static_cast<uint32_t>(std::fmod((uint32_t)((combination_index + 0.5) / period), (uint32_t)(std::min)(m_queue_requirements[requirement_index].required_queue_count + 1,
                                                                                                                                                                 m_queue_families[provider_sets[provider_set_index][provider_index]].queueCount + 1)));
                            period *= (std::min)(m_queue_requirements[requirement_index].required_queue_count + 1, m_queue_families[provider_sets[provider_set_index][provider_index]].queueCount + 1);

                            if (provided_queue_count == 0)
                            {
                                // This combination contains redundent queues
                                provider_usage_sets.pop_back();
                                period = -1.0; // Set to -1 to signal that is set is redundent
                                break;
                            }

                            Queue_Usage usage_data = {};
                            usage_data.queue_family_index = provider_sets[provider_set_index][provider_index];
                            usage_data.queue_count = provided_queue_count;

                            Queue_Provider provider_data = {};
                            provider_data.present_support = m_queue_family_present_support[usage_data.queue_family_index];
                            provider_data.requirement = m_queue_requirements[requirement_index];
                            provider_data.usage = usage_data;

                            provider_usage_sets.back().present_support = provider_usage_sets.back().present_support || provider_data.present_support;
                            provider_usage_sets.back().provided_queues += provided_queue_count;
                            provider_usage_sets.back().queue_family_usage[provider_sets[provider_set_index][provider_index]] += provided_queue_count;
                            provider_usage_sets.back().queue_providers.push_back(provider_data);
                        }

                        // Check if this set is valid
                        if (period == -1.0)
                        {
                            // This set has queue usages of 0
                            continue;
                        }

                        if (provider_usage_sets.back().provided_queues != m_queue_requirements[requirement_index].required_queue_count)
                        {
                            // This set doesn't provide the correct number of queues
                            provider_usage_sets.pop_back();
                            continue;
                        }

                        for (uint32_t queue_family_index = 0; queue_family_index < m_queue_families.size(); queue_family_index++)
                        {
                            if (provider_usage_sets.back().queue_family_usage[queue_family_index] > m_queue_families[queue_family_index].queueCount)
                            {
                                // This set uses more queue from a queue family then exist
                                provider_usage_sets.pop_back();
                                break;
                            }
                        }
                    }
                }

                queue_sets_by_requirement.push_back(provider_usage_sets);
            }

            // Create global combinations
            m_valid_queue_sets.clear();

            uint32_t combination_count = 1;
            for (uint32_t requirement_index = 0; requirement_index < m_queue_requirements.size(); requirement_index++)
            {
                combination_count *= static_cast<uint32_t>(queue_sets_by_requirement[requirement_index].size());
            }

            for (int32_t combination_index = combination_count - 1; combination_index >= 0; combination_index--)
            {
                m_valid_queue_sets.resize(m_valid_queue_sets.size() + 1);
                m_valid_queue_sets.back().provided_queues = 0;
                m_valid_queue_sets.back().queue_family_usage.resize(m_queue_families.size());
                m_valid_queue_sets.back().queue_providers.clear();

                double period = 1.0;
                for (uint32_t requirement_index = 0; requirement_index < m_queue_requirements.size(); requirement_index++)
                {
                    uint32_t selected_set = static_cast<uint32_t>(std::fmod((uint32_t)((combination_index + 0.5) / period), (uint32_t)queue_sets_by_requirement[requirement_index].size()));
                    period *= queue_sets_by_requirement[requirement_index].size();

                    m_valid_queue_sets.back() += queue_sets_by_requirement[requirement_index][selected_set];
                }

                // Ensure present support if required
                if (m_present_queue_required && !m_valid_queue_sets.back().present_support)
                {
                    m_valid_queue_sets.erase(m_valid_queue_sets.begin() + combination_index);
                    continue;
                }

                // Ensure there is no overallocation of queue families
                for (uint32_t queue_family_index = 0; queue_family_index < m_queue_families.size(); queue_family_index++)
                {
                    if (m_queue_families[queue_family_index].queueCount < m_valid_queue_sets.back().queue_family_usage[queue_family_index])
                    {
                        m_valid_queue_sets.erase(m_valid_queue_sets.begin() + combination_index);
                        break;
                    }
                }
            }
        }

        Queue_Selector& Queue_Selector::Add_Queue_Requirement(std::string label, VkQueueFlagBits queue_type, uint32_t required_queue_count, float queue_priority)
        {
            m_sets_up_to_date = false;

            m_queue_requirements.resize(m_queue_requirements.size() + 1);

            m_queue_requirements.back() = {};
            m_queue_requirements.back().requirement_label = label;
            m_queue_requirements.back().required_queue_type = queue_type;
            m_queue_requirements.back().required_queue_count = required_queue_count;
            m_queue_requirements.back().queue_priority = queue_priority;

            return *this;
        }

        Queue_Selector& Queue_Selector::Require_Present_Queue()
        {
            m_present_queue_required = true;
            m_sets_up_to_date = false;

            return *this;
        }

        bool Queue_Selector::Meets_Requirements()
        {
            if (!m_sets_up_to_date)
            {
                Generate_Sets();
                m_sets_up_to_date = true;
            }

            return !m_valid_queue_sets.empty();
        }

        Device_Queues Queue_Selector::Best()
        {
            if (!m_sets_up_to_date)
            {
                Generate_Sets();
                m_sets_up_to_date = true;
            }

            if (!Meets_Requirements())
            {
                LOG_ERROR << "Graphics (Vulkan): No queue combinations meet requirements, check Queue_Selector::Meets_Requirements prior to calling Queue_Selector::Best";
                exit(EXIT_FAILURE);
            }

            Device_Queues device_queues = {};
            device_queues.queue_priorities.resize(m_queue_families.size());
            device_queues.queue_family_usage.resize(m_queue_families.size());
            std::vector<Queue_Requirement> unique_queue_requirements;

            for (uint32_t queue_index = 0; queue_index < m_valid_queue_sets.front().queue_providers.size(); queue_index++)
            {
                std::vector<Queue_Requirement>::iterator requirement_location = std::find(unique_queue_requirements.begin(), unique_queue_requirements.end(), m_valid_queue_sets.front().queue_providers[queue_index].requirement);

                if (requirement_location == unique_queue_requirements.end())
                {
                    unique_queue_requirements.push_back(m_valid_queue_sets.front().queue_providers[queue_index].requirement);

                    device_queues.device_queue_requirements.resize(device_queues.device_queue_requirements.size() + 1);
                    device_queues.device_queue_requirements.back().requirement_name = unique_queue_requirements.back().requirement_label;

                    device_queues.device_queue_requirements.back().device_queues.resize(device_queues.device_queue_requirements.back().device_queues.size() + 1);
                    device_queues.device_queue_requirements.back().device_queues.back().priority = m_valid_queue_sets.front().queue_providers[queue_index].requirement.queue_priority;
                    device_queues.device_queue_requirements.back().device_queues.back().queue_family_index = m_valid_queue_sets.front().queue_providers[queue_index].usage.queue_family_index;
                    device_queues.device_queue_requirements.back().device_queues.back().index_in_queue_family = device_queues.queue_family_usage[device_queues.device_queue_requirements.back().device_queues.back().queue_family_index];
                    device_queues.device_queue_requirements.back().device_queues.back().queue = VK_NULL_HANDLE;
                    device_queues.device_queue_requirements.back().device_queues.back().queue_family_properties = m_queue_families[device_queues.device_queue_requirements.back().device_queues.back().queue_family_index];

                    device_queues.queue_priorities[m_valid_queue_sets.front().queue_providers[queue_index].usage.queue_family_index].push_back(m_valid_queue_sets.front().queue_providers[queue_index].requirement.queue_priority);
                    device_queues.queue_family_usage[device_queues.device_queue_requirements.back().device_queues.back().queue_family_index]++;
                }
                else
                {
                    uint32_t requirement_index = requirement_location - unique_queue_requirements.begin();

                    device_queues.device_queue_requirements[requirement_index].requirement_name = unique_queue_requirements[requirement_index].requirement_label;

                    device_queues.device_queue_requirements[requirement_index].device_queues.resize(device_queues.device_queue_requirements[requirement_index].device_queues.size() + 1);
                    device_queues.device_queue_requirements[requirement_index].device_queues.back().priority = m_valid_queue_sets.front().queue_providers[queue_index].requirement.queue_priority;
                    device_queues.device_queue_requirements[requirement_index].device_queues.back().queue_family_index = m_valid_queue_sets.front().queue_providers[queue_index].usage.queue_family_index;
                    device_queues.device_queue_requirements[requirement_index].device_queues.back().index_in_queue_family
                        = device_queues.queue_family_usage[device_queues.device_queue_requirements[requirement_index].device_queues.back().queue_family_index];
                    device_queues.device_queue_requirements[requirement_index].device_queues.back().queue = VK_NULL_HANDLE;
                    device_queues.device_queue_requirements[requirement_index].device_queues.back().queue_family_properties = m_queue_families[device_queues.device_queue_requirements[requirement_index].device_queues.back().queue_family_index];

                    device_queues.queue_priorities[m_valid_queue_sets.front().queue_providers[queue_index].usage.queue_family_index].push_back(m_valid_queue_sets.front().queue_providers[queue_index].requirement.queue_priority);
                    device_queues.queue_family_usage[device_queues.device_queue_requirements[requirement_index].device_queues.back().queue_family_index]++;
                }
            }

            return device_queues;
        }
    } // namespace Vulkan
} // namespace Cascade_Graphics