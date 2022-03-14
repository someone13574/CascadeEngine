#include "renderer.hpp"

#include "cascade_graphics.hpp"
#include "cascade_logging.hpp"

#include <memory>

namespace CascadeCore
{
    Renderer::Renderer()
    {
        LOG_INFO << "Initializing renderer";

        std::shared_ptr<CascadeGraphics::Vulkan::Instance> instance_ptr = std::make_shared<CascadeGraphics::Vulkan::Instance>("Application name", 0);
        CascadeGraphicsDebugging::Vulkan::Validation_Layer validation_layer(instance_ptr);

        LOG_TRACE << "Renderer initialized";
    }

    Renderer::~Renderer()
    {
        LOG_TRACE << "Renderer cleanup started";

        LOG_TRACE << "Renderer cleanup finished";
    }
} // namespace CascadeCore
