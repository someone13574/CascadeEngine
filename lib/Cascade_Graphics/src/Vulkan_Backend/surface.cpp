#include "surface.hpp"

#include <acorn_logging.hpp>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Surface::Surface(Instance* instance_ptr) :
            m_instance_ptr(instance_ptr)
        {
        }

        Surface::~Surface()
        {
            LOG_TRACE << "Graphics (Vulkan): Destroying surface";

            vkDestroySurfaceKHR(m_instance_ptr->Get(), m_surface, NULL);
        }

        VkSurfaceKHR Surface::Get()
        {
            return m_surface;
        }
    }    // namespace Vulkan
}    // namespace Cascade_Graphics