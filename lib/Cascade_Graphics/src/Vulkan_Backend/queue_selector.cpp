#include "queue_selector.hpp"

#include "vkresult_translator.hpp"
#include <acorn_logging.hpp>
#include <algorithm>
#include <cmath>
#include <functional>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Queue_Selector::Queue_Selector(Physical_Device* physical_device_ptr)
        {
            // LOG_DEBUG << "Graphics (Vulkan): Getting available queues for physical device '" << m_physical_device_ptr->Get_Properties()->deviceName << "'";

            // uint32_t queue_family_count;
            // vkGetPhysicalDeviceQueueFamilyProperties(*m_physical_device_ptr->Get(), &queue_family_count, nullptr);

            // std::vector<VkQueueFamilyProperties> queue_family_properties(queue_family_count);
            // vkGetPhysicalDeviceQueueFamilyProperties(*m_physical_device_ptr->Get(), &queue_family_count, queue_family_properties.data());

            // for (uint32_t queue_family_index = 0; queue_family_index < queue_family_count; queue_family_index++)
            // {
            //     Queue_Family_Filter_Info filter_info = {};
            //     filter_info.queue_family_index = queue_family_index;
            //     filter_info.queue_family_properties = queue_family_properties[queue_family_index];

            //     m_queue_filter_infos.push_back(filter_info);
            // }

            LOG_DEBUG << "Graphics (Vulkan): Getting available queue families for physical device '" << physical_device_ptr->Get_Properties()->deviceName << "'";

            uint32_t queue_family_count;
            vkGetPhysicalDeviceQueueFamilyProperties(*physical_device_ptr->Get(), &queue_family_count, nullptr);

            m_queue_families.resize(queue_family_count);
            vkGetPhysicalDeviceQueueFamilyProperties(*physical_device_ptr->Get(), &queue_family_count, m_queue_families.data());
        }

        void Queue_Selector::Generate_Sets()
        {
            LOG_TRACE << "Graphics (Vulkan): Generating all possible queue sets";

            // Create all per requirement queue sets
            std::vector<std::vector<std::vector<uint32_t>>> per_requirement_sets(m_queue_requirements.size()); // [requirement_index][possible_set_index][provider] = queue_family_index
            for (uint32_t requirement_index = 0; requirement_index < m_queue_requirements.size(); requirement_index++)
            {
                uint32_t set_count = std::pow(2, m_queue_families.size());

                // Loop through all possible combinations of queue families (start at 1 to avoid set with no elements)
                for (uint32_t set_index = 1; set_index < set_count; set_index++)
                {
                    per_requirement_sets[requirement_index].resize(per_requirement_sets[requirement_index].size() + 1);

                    // Evalute each bit in the set_index to determine whether a queue family is included
                    for (uint32_t queue_family_index = 0; queue_family_index < m_queue_families.size(); queue_family_index++)
                    {
                        bool include_queue_family = (set_index >> queue_family_index & 1) == 1;

                        if (include_queue_family)
                        {
                            per_requirement_sets[requirement_index].back().push_back(queue_family_index);

                            // Check if set contains an unnessessary queue family
                            if ((m_queue_families[queue_family_index].queueFlags & m_queue_requirements[requirement_index].queue_type) != m_queue_requirements[requirement_index].queue_type)
                            {
                                // Delete current set
                                per_requirement_sets[requirement_index].erase(per_requirement_sets[requirement_index].end());
                                break;
                            }
                        }
                    }
                }
            }

            // Create all usage sets
            std::vector<std::vector<std::vector<std::pair<uint32_t, uint32_t>>>> per_requirement_usage_sets(
                m_queue_requirements.size()); // [requirement_index][possible_set_index][possible_combination_index][provider] = pair(queue_family_index, queue_usage)
            for (uint32_t requirement_index = 0; requirement_index < m_queue_requirements.size(); requirement_index++)
            {
                for (uint32_t set_index = 0; set_index < per_requirement_sets[requirement_index].size(); set_index++)
                {
                    // Get total number of possible combinations
                    uint32_t number_of_possible_combinations = 1;
                    for (uint32_t provider_index = 0; provider_index < per_requirement_sets[requirement_index][set_index].size(); provider_index++)
                    {
                        number_of_possible_combinations *= std::min(m_queue_requirements[requirement_index].required_queue_count + 1, m_queue_families[per_requirement_sets[requirement_index][set_index][provider_index]].queueCount + 1);
                    }

                    // Generate possible combinations
                    for (uint32_t combination_index = 0; combination_index < number_of_possible_combinations; combination_index++)
                    {
                        double period = 1.0;
                        per_requirement_usage_sets[requirement_index].resize(per_requirement_usage_sets[requirement_index].size() + 1);

                        for (uint32_t provider_index = 0; provider_index < per_requirement_sets[requirement_index][set_index].size(); provider_index++)
                        {
                            uint32_t provided_queue_count = std::fmod((uint32_t)((combination_index + 0.5) / period), (uint32_t)std::min(m_queue_requirements[requirement_index].required_queue_count + 1,
                                                                                                                                         m_queue_families[per_requirement_sets[requirement_index][set_index][provider_index]].queueCount + 1));

                            if (provided_queue_count == 0)
                            {
                                // Useless provider
                                per_requirement_usage_sets[requirement_index].pop_back();
                                break;
                            }

                            period *= std::min(m_queue_requirements[requirement_index].required_queue_count + 1, m_queue_families[per_requirement_sets[requirement_index][set_index][provider_index]].queueCount + 1);

                            per_requirement_usage_sets[requirement_index].back().push_back(std::make_pair(per_requirement_sets[requirement_index][set_index][provider_index], provided_queue_count));
                        }
                    }
                }
            }

            // Filter usage sets
            for (uint32_t requirement_index = 0; requirement_index < m_queue_requirements.size(); requirement_index++)
            {
                for (int32_t set_index = per_requirement_usage_sets[requirement_index].size() - 1; set_index >= 0; set_index--)
                {
                    uint32_t provided_queues = 0;
                    std::vector<uint32_t> queue_family_usage(m_queue_families.size()); // [queue_family_index] = total_queue_usage

                    // Determine queue family usage for this combination
                    for (int32_t provider_index = per_requirement_usage_sets[requirement_index][set_index].size() - 1; provider_index >= 0; provider_index--)
                    {
                        uint32_t usage = per_requirement_usage_sets[requirement_index][set_index][provider_index].second;

                        provided_queues += usage;

                        uint32_t queue_family = per_requirement_usage_sets[requirement_index][set_index][provider_index].first;
                        queue_family_usage[queue_family] += usage;
                    }

                    // Detect overallocation
                    for (uint32_t queue_family_index = 0; queue_family_index < m_queue_families.size(); queue_family_index++)
                    {
                        if (queue_family_usage[queue_family_index] > m_queue_families[queue_family_index].queueCount)
                        {
                            // A queue family is overallocated
                            per_requirement_usage_sets[requirement_index].erase(per_requirement_usage_sets[requirement_index].begin() + set_index);
                            break;
                        }
                    }
                    if (provided_queues != m_queue_requirements[requirement_index].required_queue_count)
                    {
                        per_requirement_usage_sets[requirement_index].erase(per_requirement_usage_sets[requirement_index].begin() + set_index);
                    }
                }
            }

            // Create global combinations
            struct Queue_Combination
            {
                std::vector<uint32_t> queue_family_usage;                          // [queue_family_index] = usage_by_this_combination
                std::vector<std::vector<std::pair<uint32_t, uint32_t>>> providers; // [requirement_index][provider_index] = pair(queue_family_index, usage)
            };
            m_valid_queue_sets.clear();

            uint32_t combination_count = 1;
            for (uint32_t requirement_index = 0; requirement_index < m_queue_requirements.size(); requirement_index++)
            {
                combination_count *= per_requirement_usage_sets[requirement_index].size();
            }

            for (int32_t combination_index = combination_count - 1; combination_index >= 0; combination_index--)
            {
                m_valid_queue_sets.resize(m_valid_queue_sets.size() + 1);
                m_valid_queue_sets.back().queue_family_usage.resize(m_queue_families.size());

                double period = 1.0;
                for (uint32_t requirement_index = 0; requirement_index < m_queue_requirements.size(); requirement_index++)
                {
                    uint32_t selected_set = std::fmod((uint32_t)((combination_index + 0.5) / period), (uint32_t)per_requirement_usage_sets[requirement_index].size());
                    period *= per_requirement_usage_sets[requirement_index].size();

                    m_valid_queue_sets.back().queue_providers.push_back(per_requirement_usage_sets[requirement_index][selected_set]);
                    for (uint32_t provider_index = 0; provider_index < per_requirement_usage_sets[requirement_index][selected_set].size(); provider_index++)
                    {
                        m_valid_queue_sets.back().queue_family_usage[per_requirement_usage_sets[requirement_index][selected_set][provider_index].first] += per_requirement_usage_sets[requirement_index][selected_set][provider_index].second;
                    }
                }

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

        Queue_Selector& Queue_Selector::Add_Queue_Requirement(std::string label, VkQueueFlagBits queue_type, uint32_t required_queue_count)
        {
            m_queue_requirements.resize(m_queue_requirements.size() + 1);

            m_queue_requirements.back() = {};
            m_queue_requirements.back().requirement_label = label;
            m_queue_requirements.back().queue_type = queue_type;
            m_queue_requirements.back().required_queue_count = required_queue_count;

            return *this;
        }

        bool Queue_Selector::Meets_Requirements()
        {
            Generate_Sets();

            return !m_valid_queue_sets.empty();
        }
    } // namespace Vulkan
} // namespace Cascade_Graphics