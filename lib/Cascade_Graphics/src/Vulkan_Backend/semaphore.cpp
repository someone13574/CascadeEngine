#include "semaphore.hpp"
#include <acorn_logging.hpp>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Semaphore::Semaphore(Device* device_ptr, uint32_t semaphore_count) :
            m_device_ptr(device_ptr)
        {
            LOG_TRACE << "Graphics (Vulkan): Creating " << semaphore_count << " semaphores";

            VkSemaphoreCreateInfo semaphore_create_info = {};
            semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            semaphore_create_info.pNext = NULL;
            semaphore_create_info.flags = 0;

            m_semaphores.resize(semaphore_count);
            for (uint32_t semaphore_index = 0; semaphore_index < semaphore_count; semaphore_index++)
            {
                VkResult create_semaphore_result = vkCreateSemaphore(device_ptr->Get(), &semaphore_create_info, NULL, &m_semaphores[semaphore_index]);
                if (create_semaphore_result != VK_SUCCESS)
                {
                    LOG_FATAL << "Graphics (Vulkan): Failed to create semaphore";
                    exit(EXIT_FAILURE);
                }
            }
        }

        Semaphore::~Semaphore()
        {
            LOG_TRACE << "Graphics (Vulkan): Destroying " << m_semaphores.size() << " semaphores";

            for (uint32_t semaphore_index = 0; semaphore_index < m_semaphores.size(); semaphore_index++)
            {
                vkDestroySemaphore(m_device_ptr->Get(), m_semaphores[semaphore_index], NULL);
            }
            m_semaphores.clear();
        }

        VkSemaphore* Semaphore::Get(uint32_t index)
        {
            return &m_semaphores[index];
        }
    }    // namespace Vulkan
}    // namespace Cascade_Graphics