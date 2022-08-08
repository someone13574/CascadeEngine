#include "queue_manager.hpp"


#include "cascade_logging.hpp"

#include <set>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Queue_Manager::Queue_Manager(std::shared_ptr<Surface> surface_ptr, uint32_t required_queues) : m_surface_ptr(surface_ptr), m_required_queues(required_queues)
        {
        }

        uint32_t Queue_Manager::Get_Required_Queue_Types()
        {
            return m_required_queues;
        }

        VkQueue* Queue_Manager::Get_Queue(Queue_Types queue_type)
        {
            if (m_required_queues & queue_type)
            {
                return &m_queues.find(queue_type)->second;
            }
            else
            {
                LOG_ERROR << "Vulkan: The requested queue type was not required";
                exit(EXIT_FAILURE);
            }
        }

        uint32_t Queue_Manager::Get_Queue_Family_Index(Queue_Types queue_type)
        {
            if (m_required_queues & queue_type)
            {
                if (queue_type & Queue_Types::GRAPHICS_QUEUE)
                {
                    return m_queue_family_indices.graphics_family_index.value();
                }
                if (queue_type & Queue_Types::COMPUTE_QUEUE)
                {
                    return m_queue_family_indices.compute_family_index.value();
                }
                if (queue_type & Queue_Types::TRANSFER_QUEUE)
                {
                    return m_queue_family_indices.transfer_family_index.value();
                }
                if (queue_type & Queue_Types::SPARSE_BINDING_QUEUE)
                {
                    return m_queue_family_indices.sparse_binding_family_index.value();
                }
                if (queue_type & Queue_Types::PROTECTED_QUEUE)
                {
                    return m_queue_family_indices.protected_family_index.value();
                }
                if (queue_type & Queue_Types::PRESENT_QUEUE)
                {
                    return m_queue_family_indices.present_family_index.value();
                }

                LOG_ERROR << "Vulkan: Queue type does not exist";
                exit(EXIT_FAILURE);
            }
            else
            {
                LOG_ERROR << "Vulkan: The requested queue type was not required";
                exit(EXIT_FAILURE);
            }
        }

        void Queue_Manager::Get_Device_Queue_Handles(VkDevice* device_ptr)
        {
            LOG_INFO << "Vulkan: Getting device queues from logical device";

            if (m_required_queues & Queue_Types::GRAPHICS_QUEUE)
            {
                m_queues.insert({Queue_Types::GRAPHICS_QUEUE, VK_NULL_HANDLE});
                vkGetDeviceQueue(*device_ptr, m_queue_family_indices.graphics_family_index.value(), 0, &m_queues.find(Queue_Types::GRAPHICS_QUEUE)->second);
            }
            if (m_required_queues & Queue_Types::COMPUTE_QUEUE)
            {
                m_queues.insert({Queue_Types::COMPUTE_QUEUE, VK_NULL_HANDLE});
                vkGetDeviceQueue(*device_ptr, m_queue_family_indices.compute_family_index.value(), 0, &m_queues.find(Queue_Types::COMPUTE_QUEUE)->second);
            }
            if (m_required_queues & Queue_Types::TRANSFER_QUEUE)
            {
                m_queues.insert({Queue_Types::TRANSFER_QUEUE, VK_NULL_HANDLE});
                vkGetDeviceQueue(*device_ptr, m_queue_family_indices.transfer_family_index.value(), 0, &m_queues.find(Queue_Types::TRANSFER_QUEUE)->second);
            }
            if (m_required_queues & Queue_Types::SPARSE_BINDING_QUEUE)
            {
                m_queues.insert({Queue_Types::SPARSE_BINDING_QUEUE, VK_NULL_HANDLE});
                vkGetDeviceQueue(*device_ptr, m_queue_family_indices.sparse_binding_family_index.value(), 0, &m_queues.find(Queue_Types::SPARSE_BINDING_QUEUE)->second);
            }
            if (m_required_queues & Queue_Types::PROTECTED_QUEUE)
            {
                m_queues.insert({Queue_Types::PROTECTED_QUEUE, VK_NULL_HANDLE});
                vkGetDeviceQueue(*device_ptr, m_queue_family_indices.protected_family_index.value(), 0, &m_queues.find(Queue_Types::PROTECTED_QUEUE)->second);
            }
            if (m_required_queues & Queue_Types::PRESENT_QUEUE)
            {
                m_queues.insert({Queue_Types::PRESENT_QUEUE, VK_NULL_HANDLE});
                vkGetDeviceQueue(*device_ptr, m_queue_family_indices.present_family_index.value(), 0, &m_queues.find(Queue_Types::PRESENT_QUEUE)->second);
            }
        }

        void Queue_Manager::Get_Queue_Families(VkPhysicalDevice* physical_device_ptr)
        {
            LOG_INFO << "Vulkan: Getting queue families";

            m_queue_family_indices = {};
            m_queue_family_indices.required_queues = m_required_queues;

            uint32_t queue_family_count;
            vkGetPhysicalDeviceQueueFamilyProperties(*physical_device_ptr, &queue_family_count, nullptr);

            std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
            vkGetPhysicalDeviceQueueFamilyProperties(*physical_device_ptr, &queue_family_count, queue_families.data());

            LOG_DEBUG << "Vulkan: Found " << queue_family_count << " queue_families";

            for (uint32_t i = 0; i < queue_family_count; i++)
            {
                bool is_graphics_family = queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;
                bool is_compute_family = queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT;
                bool is_transfer_family = queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT;
                bool is_sparse_binding_family = queue_families[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT;
                bool is_protected_family = queue_families[i].queueFlags & VK_QUEUE_PROTECTED_BIT;

                VkBool32 is_present_family = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(*physical_device_ptr, i, *m_surface_ptr->Get_Surface(), &is_present_family);


                LOG_DEBUG << "Vulkan: Found queue family (Count: " << queue_families[i].queueCount << ")" << (is_graphics_family ? " (Graphics)" : "") << (is_compute_family ? " (Compute)" : "") << (is_transfer_family ? " (Transfer)" : "")
                          << (is_sparse_binding_family ? " (Sparse binding)" : "") << (is_protected_family ? " (Protected)" : "") << (is_present_family ? " (Present)" : "");

                if (m_required_queues & Queue_Types::GRAPHICS_QUEUE && is_graphics_family && !m_queue_family_indices.graphics_family_index.has_value())
                {
                    m_queue_family_indices.graphics_family_index = i;
                }
                if (m_required_queues & Queue_Types::COMPUTE_QUEUE && is_compute_family && !m_queue_family_indices.compute_family_index.has_value())
                {
                    m_queue_family_indices.compute_family_index = i;
                }
                if (m_required_queues & Queue_Types::TRANSFER_QUEUE && is_transfer_family && !m_queue_family_indices.transfer_family_index.has_value())
                {
                    m_queue_family_indices.transfer_family_index = i;
                }
                if (m_required_queues & Queue_Types::SPARSE_BINDING_QUEUE && is_sparse_binding_family && !m_queue_family_indices.sparse_binding_family_index.has_value())
                {
                    m_queue_family_indices.sparse_binding_family_index = i;
                }
                if (m_required_queues & Queue_Types::PROTECTED_QUEUE && is_protected_family && !m_queue_family_indices.protected_family_index.has_value())
                {
                    m_queue_family_indices.protected_family_index = i;
                }
                if (m_required_queues & Queue_Types::PRESENT_QUEUE && is_present_family && !m_queue_family_indices.present_family_index.has_value())
                {
                    m_queue_family_indices.present_family_index = i;
                }
            }

            if (m_required_queues & Queue_Types::GRAPHICS_QUEUE)
            {
                if (m_queue_family_indices.graphics_family_index.has_value())
                {
                    LOG_DEBUG << "Vulkan: Graphics queue family set to index " << m_queue_family_indices.graphics_family_index.value();
                }
                else
                {
                    LOG_ERROR << "Vulkan: Selected physical device does not have a graphics queue";
                    exit(EXIT_FAILURE);
                }
            }
            if (m_required_queues & Queue_Types::COMPUTE_QUEUE)
            {
                if (m_queue_family_indices.compute_family_index.has_value())
                {
                    LOG_DEBUG << "Vulkan: Compute queue family set to index " << m_queue_family_indices.compute_family_index.value();
                }
                else
                {
                    LOG_ERROR << "Vulkan: Selected physical device does not have a compute queue";
                    exit(EXIT_FAILURE);
                }
            }
            if (m_required_queues & Queue_Types::TRANSFER_QUEUE)
            {
                if (m_queue_family_indices.transfer_family_index.has_value())
                {
                    LOG_DEBUG << "Vulkan: Transfer queue family set to index " << m_queue_family_indices.transfer_family_index.value();
                }
                else
                {
                    LOG_ERROR << "Vulkan: Selected physical device does not have a transfer queue";
                    exit(EXIT_FAILURE);
                }
            }
            if (m_required_queues & Queue_Types::SPARSE_BINDING_QUEUE)
            {
                if (m_queue_family_indices.sparse_binding_family_index.has_value())
                {
                    LOG_DEBUG << "Vulkan: Sparse binding queue family set to index " << m_queue_family_indices.sparse_binding_family_index.value();
                }
                else
                {
                    LOG_ERROR << "Vulkan: Selected physical device does not have a sparse binding queue";
                    exit(EXIT_FAILURE);
                }
            }
            if (m_required_queues & Queue_Types::PROTECTED_QUEUE)
            {
                if (m_queue_family_indices.protected_family_index.has_value())
                {
                    LOG_DEBUG << "Vulkan: Protected queue family set to index " << m_queue_family_indices.protected_family_index.value();
                }
                else
                {
                    LOG_ERROR << "Vulkan: Selected physical device does not have a protected queue";
                    exit(EXIT_FAILURE);
                }
            }
            if (m_required_queues & Queue_Types::PRESENT_QUEUE)
            {
                if (m_queue_family_indices.present_family_index.has_value())
                {
                    LOG_DEBUG << "Vulkan: Present queue family set to index " << m_queue_family_indices.present_family_index.value();
                }
                else
                {
                    LOG_ERROR << "Vulkan: Selected physical device does not have a present queue";
                    exit(EXIT_FAILURE);
                }
            }

            LOG_TRACE << "Vulkan: Finished getting queue families";
        }

        bool Queue_Manager::Has_Required_Queues(VkPhysicalDevice* physical_device_ptr)
        {
            LOG_TRACE << "Vulkan: Checking if physical device has required queues";

            uint32_t queue_family_count;
            vkGetPhysicalDeviceQueueFamilyProperties(*physical_device_ptr, &queue_family_count, nullptr);

            std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
            vkGetPhysicalDeviceQueueFamilyProperties(*physical_device_ptr, &queue_family_count, queue_families.data());

            LOG_TRACE << "Vulkan: Found " << queue_family_count << " queue_families";

            uint32_t staisfied_queues = 0;
            for (uint32_t i = 0; i < queue_family_count; i++)
            {
                bool is_graphics_family = queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;
                bool is_compute_family = queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT;
                bool is_transfer_family = queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT;
                bool is_sparse_binding_family = queue_families[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT;
                bool is_protected_family = queue_families[i].queueFlags & VK_QUEUE_PROTECTED_BIT;

                VkBool32 is_present_family = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(*physical_device_ptr, i, *m_surface_ptr->Get_Surface(), &is_present_family);


                LOG_TRACE << "Vulkan: Found queue family (Count: " << queue_families[i].queueCount << ")" << (is_graphics_family ? " (Graphics)" : "") << (is_compute_family ? " (Compute)" : "") << (is_transfer_family ? " (Transfer)" : "")
                          << (is_sparse_binding_family ? " (Sparse binding)" : "") << (is_protected_family ? " (Protected)" : "") << (is_present_family ? " (Present)" : "");

                staisfied_queues |= is_graphics_family ? Queue_Types::GRAPHICS_QUEUE : 0;
                staisfied_queues |= is_compute_family ? Queue_Types::COMPUTE_QUEUE : 0;
                staisfied_queues |= is_transfer_family ? Queue_Types::TRANSFER_QUEUE : 0;
                staisfied_queues |= is_sparse_binding_family ? Queue_Types::SPARSE_BINDING_QUEUE : 0;
                staisfied_queues |= is_protected_family ? Queue_Types::PROTECTED_QUEUE : 0;
                staisfied_queues |= is_present_family ? Queue_Types::PRESENT_QUEUE : 0;
            }

            if ((staisfied_queues & m_required_queues) == m_required_queues)
            {
                LOG_TRACE << "Vulkan: Has all required queues";
                return true;
            }
            else
            {
                LOG_TRACE << "Vulkan: Does not have all required queues";
                return false;
            }
        }

        std::vector<uint32_t> Queue_Manager::Get_Unique_Queue_Families(uint32_t required_queues)
        {
            std::set<uint32_t> unique_family_indices;

            if ((required_queues & Queue_Types::GRAPHICS_QUEUE) && (m_required_queues & Queue_Types::GRAPHICS_QUEUE))
            {
                unique_family_indices.insert(m_queue_family_indices.graphics_family_index.value());
            }
            if ((required_queues & Queue_Types::COMPUTE_QUEUE) && (m_required_queues & Queue_Types::COMPUTE_QUEUE))
            {
                unique_family_indices.insert(m_queue_family_indices.compute_family_index.value());
            }
            if ((required_queues & Queue_Types::TRANSFER_QUEUE) && (m_required_queues & Queue_Types::TRANSFER_QUEUE))
            {
                unique_family_indices.insert(m_queue_family_indices.transfer_family_index.value());
            }
            if ((required_queues & Queue_Types::SPARSE_BINDING_QUEUE) && (m_required_queues & Queue_Types::SPARSE_BINDING_QUEUE))
            {
                unique_family_indices.insert(m_queue_family_indices.sparse_binding_family_index.value());
            }
            if ((required_queues & Queue_Types::PROTECTED_QUEUE) && (m_required_queues & Queue_Types::PROTECTED_QUEUE))
            {
                unique_family_indices.insert(m_queue_family_indices.protected_family_index.value());
            }
            if ((required_queues & Queue_Types::PRESENT_QUEUE) && (m_required_queues & Queue_Types::PRESENT_QUEUE))
            {
                unique_family_indices.insert(m_queue_family_indices.present_family_index.value());
            }

            std::vector<uint32_t> unique_queue_indices_vector(unique_family_indices.begin(), unique_family_indices.end());

            return unique_queue_indices_vector;
        }

        std::vector<VkDeviceQueueCreateInfo> Queue_Manager::Generate_Device_Queue_Create_Infos()
        {
            LOG_INFO << "Vulkan: Generating device queue create infos";

            int32_t protected_queue_family_index = (m_required_queues & Queue_Types::PROTECTED_QUEUE) ? m_queue_family_indices.protected_family_index.value() : -1;
            std::vector<uint32_t> unique_queue_families = Get_Unique_Queue_Families(m_required_queues);

            float queue_priority = 1.0;
            std::vector<VkDeviceQueueCreateInfo> device_queue_create_infos;

            for (uint32_t i = 0; i < unique_queue_families.size(); i++)
            {
                device_queue_create_infos.resize(device_queue_create_infos.size() + 1);

                device_queue_create_infos.back() = {};
                device_queue_create_infos.back().sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                device_queue_create_infos.back().pNext = nullptr;
                device_queue_create_infos.back().flags = (unique_queue_families[i] == protected_queue_family_index) ? VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT : 0;
                device_queue_create_infos.back().queueFamilyIndex = unique_queue_families[i];
                device_queue_create_infos.back().queueCount = 1;
                device_queue_create_infos.back().pQueuePriorities = &queue_priority;
            }

            return device_queue_create_infos;
        }
    } // namespace Vulkan
} // namespace Cascade_Graphics