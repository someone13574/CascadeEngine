#include "renderer.hpp"
#include "cascade_graphics.hpp"
#include "cascade_logging.hpp"

#include <vulkan/vulkan.hpp>

namespace CascadeCore
{
    Renderer::Renderer()
    {
        LOG_INFO << "Initializing renderer";

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        LOG_DEBUG << extensionCount << " extensions supported";

        Test_Function();

        LOG_TRACE << "Renderer initialized";
    }

    Renderer::~Renderer()
    {
        LOG_TRACE << "Renderer cleanup started";

        LOG_TRACE << "Renderer cleanup finished";
    }
} // namespace CascadeCore
