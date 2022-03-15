#include "queue_wrapper.hpp"

#include "cascade_logging.hpp"

#include <set>

namespace CascadeGraphics
{
    namespace Vulkan
    {
        Queue_Manager::Queue_Manager(bool graphics_required, bool compute_required, bool transfer_required, bool sparse_binding_required, bool protected_required)
        {
            LOG_INFO << "Vulkan: creating queue manager";

            if (!graphics_required && !compute_required && !transfer_required && !sparse_binding_required && !protected_required)
            {
                LOG_WARN << "Vulkan: no features set to required for queue manager";
            }

            m_queue_types_required[0] = graphics_required;
            m_queue_types_required[1] = compute_required;
            m_queue_types_required[2] = transfer_required;
            m_queue_types_required[3] = sparse_binding_required;
            m_queue_types_required[4] = protected_required;

            LOG_TRACE << "Vulkan: finished creating queue manager";
        }

        Queue_Manager::~Queue_Manager()
        {
        }

        void Queue_Manager::Set_Queue_Family_Indices(std::shared_ptr<Physical_Device> physical_device_ptr)
        {
            LOG_INFO << "Vulkan: setting queue family indices";

            unsigned int queue_families_count = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(*(physical_device_ptr->Get_Physical_Device()), &queue_families_count, nullptr);
            LOG_TRACE << "Vulkan: found " << queue_families_count << " queue families";

            std::vector<VkQueueFamilyProperties> queue_families(queue_families_count);
            vkGetPhysicalDeviceQueueFamilyProperties(*(physical_device_ptr->Get_Physical_Device()), &queue_families_count, queue_families.data());

            for (unsigned int i = 0; i < queue_families_count; i++)
            {
                bool family_has_graphics = queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;
                bool family_has_compute = queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT;
                bool family_has_transfer = queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT;
                bool family_has_sparse_binding = queue_families[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT;
                bool family_has_protected = queue_families[i].queueFlags & VK_QUEUE_PROTECTED_BIT;

                LOG_TRACE << "Vulkan: found queue family (Count: " << queue_families[i].queueCount << ")" << ((family_has_graphics) ? " (Graphics)" : "")
                          << ((family_has_compute) ? " (Compute)" : "") << ((family_has_transfer) ? " (Transfer)" : "")
                          << ((family_has_sparse_binding) ? " (Sparse binding)" : "") << ((family_has_protected) ? " (Protected)" : "");

                if (m_queue_types_required[0] && family_has_graphics && !m_queue_family_indices.m_graphics_index.has_value())
                {
                    m_queue_family_indices.m_graphics_index = i;
                }
                if (m_queue_types_required[1] && family_has_compute && !m_queue_family_indices.m_compute_index.has_value())
                {
                    m_queue_family_indices.m_compute_index = i;
                }
                if (m_queue_types_required[2] && family_has_transfer && !m_queue_family_indices.m_transfer_index.has_value())
                {
                    m_queue_family_indices.m_transfer_index = i;
                }
                if (m_queue_types_required[3] && family_has_sparse_binding && !m_queue_family_indices.m_sparse_binding_index.has_value())
                {
                    m_queue_family_indices.m_sparse_binding_index = i;
                }
                if (m_queue_types_required[4] && family_has_protected && !m_queue_family_indices.m_protected_index.has_value())
                {
                    m_queue_family_indices.m_protected_index = i;
                }
            }

            if (m_queue_types_required[0])
            {
                if (m_queue_family_indices.m_graphics_index.has_value())
                {
                    LOG_DEBUG << "Vulkan: graphics queue family index set to " << m_queue_family_indices.m_graphics_index.value();
                }
                else
                {
                    LOG_FATAL << "Vulkan: this physical device cannot be used to set queue family indices because it is missing a queue with graphics support";
                    exit(EXIT_FAILURE);
                }
            }
            if (m_queue_types_required[1])
            {
                if (m_queue_family_indices.m_compute_index.has_value())
                {
                    LOG_DEBUG << "Vulkan: compute queue family index set to " << m_queue_family_indices.m_compute_index.value();
                }
                else
                {
                    LOG_FATAL << "Vulkan: this physical device cannot be used to set queue family indices because it is missing a queue with compute support";
                    exit(EXIT_FAILURE);
                }
            }
            if (m_queue_types_required[2])
            {
                if (m_queue_family_indices.m_transfer_index.has_value())
                {
                    LOG_DEBUG << "Vulkan: transfer queue family index set to " << m_queue_family_indices.m_transfer_index.value();
                }
                else
                {
                    LOG_FATAL << "Vulkan: this physical device cannot be used to set queue family indices because it is missing a queue with transfer support";
                    exit(EXIT_FAILURE);
                }
            }
            if (m_queue_types_required[3])
            {
                if (m_queue_family_indices.m_sparse_binding_index.has_value())
                {
                    LOG_DEBUG << "Vulkan: sparse binding queue family index set to " << m_queue_family_indices.m_sparse_binding_index.value();
                }
                else
                {
                    LOG_FATAL << "Vulkan: this physical device cannot be used to set queue family indices because it is missing a queue with sparse binding support";
                    exit(EXIT_FAILURE);
                }
            }
            if (m_queue_types_required[4])
            {
                if (m_queue_family_indices.m_protected_index.has_value())
                {
                    LOG_DEBUG << "Vulkan: protected queue family index set to " << m_queue_family_indices.m_protected_index.value();
                }
                else
                {
                    LOG_FATAL << "Vulkan: this physical device cannot be used to set queue family indices because it is missing a queue with protected support";
                    exit(EXIT_FAILURE);
                }
            }

            m_queue_family_indices_set = true;

            LOG_TRACE << "Vulkan: finished setting queue family indices";
        }

        void Queue_Manager::Get_Device_Queue_Handles(VkDevice* device_ptr)
        {
            LOG_INFO << "Vulkan: getting device queue handles";

            std::set<unsigned int> unique_queue_family_indices;

            if (m_queue_types_required[0])
            {
                unique_queue_family_indices.insert(m_queue_family_indices.m_graphics_index.value());
            }
            if (m_queue_types_required[1])
            {
                unique_queue_family_indices.insert(m_queue_family_indices.m_compute_index.value());
            }
            if (m_queue_types_required[2])
            {
                unique_queue_family_indices.insert(m_queue_family_indices.m_transfer_index.value());
            }
            if (m_queue_types_required[3])
            {
                unique_queue_family_indices.insert(m_queue_family_indices.m_sparse_binding_index.value());
            }
            if (m_queue_types_required[4])
            {
                unique_queue_family_indices.insert(m_queue_family_indices.m_protected_index.value());
            }

            m_queues.resize(unique_queue_family_indices.size());

            unsigned int queue_index = 0;
            for (unsigned int unique_queue_family_index : unique_queue_family_indices)
            {
                vkGetDeviceQueue(*device_ptr, unique_queue_family_index, 0, &m_queues[queue_index]);
                queue_index++;
            }

            LOG_TRACE << "Vulkan: got device queue handles";
        }

        bool Queue_Manager::Physical_Device_Has_Required_Queues(VkPhysicalDevice physical_device)
        {
            LOG_TRACE << "Vulkan: checking if physical device has required queues";

            unsigned int queue_families_count = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, nullptr);
            LOG_TRACE << "Vulkan: found " << queue_families_count << " queue families";

            std::vector<VkQueueFamilyProperties> queue_families(queue_families_count);
            vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, queue_families.data());

            bool queue_requirement_staisfied[5]
                = {!m_queue_types_required[0], !m_queue_types_required[1], !m_queue_types_required[2], !m_queue_types_required[3], !m_queue_types_required[4]};
            for (unsigned int i = 0; i < queue_families_count; i++)
            {
                bool family_has_graphics = queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;
                bool family_has_compute = queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT;
                bool family_has_transfer = queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT;
                bool family_has_sparse_binding = queue_families[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT;
                bool family_has_protected = queue_families[i].queueFlags & VK_QUEUE_PROTECTED_BIT;

                LOG_TRACE << "Vulkan: found queue family (Count: " << queue_families[i].queueCount << ")" << ((family_has_graphics) ? " (Graphics)" : "")
                          << ((family_has_compute) ? " (Compute)" : "") << ((family_has_transfer) ? " (Transfer)" : "")
                          << ((family_has_sparse_binding) ? " (Sparse binding)" : "") << ((family_has_protected) ? " (Protected)" : "");

                queue_requirement_staisfied[0] |= family_has_graphics;
                queue_requirement_staisfied[1] |= family_has_compute;
                queue_requirement_staisfied[2] |= family_has_transfer;
                queue_requirement_staisfied[3] |= family_has_sparse_binding;
                queue_requirement_staisfied[4] |= family_has_protected;
            }

            for (unsigned int i = 0; i < 5; i++)
            {
                if (!queue_requirement_staisfied[i])
                {
                    LOG_TRACE << "Vulkan: physical device doesn't have required queues";
                    return false;
                }
            }

            LOG_TRACE << "Vulkan: physical device has required queues";
            return true;
        }

        Queue_Manager::Queue_Family_Indices Queue_Manager::Get_Queue_Family_Indices()
        {
            return m_queue_family_indices;
        }

        bool Queue_Manager::Is_Feature_Enabled(unsigned int feature_index)
        {
            if (feature_index > 4)
            {
                LOG_WARN << "Vulkan: reading out of bounds queue feature status. Returning false";
                return false;
            }

            return m_queue_types_required[feature_index];
        }

        std::vector<VkDeviceQueueCreateInfo> Queue_Manager::Generate_Queue_Create_Infos()
        {
            LOG_INFO << "Vulkan: genereating device queue create infos";

            if (!m_queue_family_indices_set)
            {
                LOG_FATAL << "Vulkan: queue family indices have not been set";
                exit(EXIT_FAILURE);
            }

            int protected_index = -1;
            std::set<unsigned int> unique_queue_family_indices;

            if (m_queue_types_required[0])
            {
                unique_queue_family_indices.insert(m_queue_family_indices.m_graphics_index.value());
            }
            if (m_queue_types_required[1])
            {
                unique_queue_family_indices.insert(m_queue_family_indices.m_compute_index.value());
            }
            if (m_queue_types_required[2])
            {
                unique_queue_family_indices.insert(m_queue_family_indices.m_transfer_index.value());
            }
            if (m_queue_types_required[3])
            {
                unique_queue_family_indices.insert(m_queue_family_indices.m_sparse_binding_index.value());
            }
            if (m_queue_types_required[4])
            {
                unique_queue_family_indices.insert(m_queue_family_indices.m_protected_index.value());
                protected_index = m_queue_family_indices.m_protected_index.value();
            }

            float queue_priority = 1.0;
            std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
            for (unsigned int unique_queue_family_index : unique_queue_family_indices)
            {
                VkDeviceQueueCreateInfo device_queue_create_info = {};
                device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                device_queue_create_info.pNext = NULL;
                device_queue_create_info.flags = (unique_queue_family_index == protected_index) ? VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT : 0;
                device_queue_create_info.queueFamilyIndex = unique_queue_family_index;
                device_queue_create_info.queueCount = 1;
                device_queue_create_info.pQueuePriorities = &queue_priority;

                queue_create_infos.push_back(device_queue_create_info);
            }

            return queue_create_infos;
        }
    } // namespace Vulkan
} // namespace CascadeGraphics