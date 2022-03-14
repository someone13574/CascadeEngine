#include "queue_wrapper.hpp"

#include "cascade_logging.hpp"

#include <vector>

namespace CascadeGraphics
{
    namespace Vulkan
    {
        Queue_Manager::Queue_Manager(bool graphics_required, bool compute_required, bool transfer_required, bool sparse_binding_required, bool protected_required)
        {
            LOG_INFO << "Vulkan: creating queue manager";

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
                queue_requirement_staisfied[4] |= family_has_sparse_binding;
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
    } // namespace Vulkan
} // namespace CascadeGraphics
