#include "fence.hpp"
#include <acorn_logging.hpp>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Fence::Fence(Device* device_ptr, uint32_t fence_count, bool signaled) :
            m_device_ptr(device_ptr)
        {
            LOG_TRACE << "Graphics (Vulkan): Creating " << fence_count << " fences";

            VkFenceCreateInfo fence_create_info = {};
            fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fence_create_info.pNext = NULL;
            fence_create_info.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

            m_fences.resize(fence_count);
            for (uint32_t fence_index = 0; fence_index < fence_count; fence_index++)
            {
                VkResult create_fence_result = vkCreateFence(device_ptr->Get(), &fence_create_info, NULL, &m_fences[fence_index]);
                if (create_fence_result != VK_SUCCESS)
                {
                    LOG_FATAL << "Graphics (Vulkan): Failed to create fence";
                    exit(EXIT_FAILURE);
                }
            }
        }

        Fence::~Fence()
        {
            LOG_TRACE << "Graphics (Vulkan): Destroying " << m_fences.size() << " fences";

            for (uint32_t fence_index = 0; fence_index < m_fences.size(); fence_index++)
            {
                vkDestroyFence(m_device_ptr->Get(), m_fences[fence_index], NULL);
            }
            m_fences.clear();
        }

        VkFence* Fence::Get(uint32_t index)
        {
            return &m_fences[index];
        }
    }    // namespace Vulkan
}    // namespace Cascade_Graphics