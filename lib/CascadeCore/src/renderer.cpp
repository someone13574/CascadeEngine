#include "renderer.hpp"

#include "cascade_graphics.hpp"
#include "cascade_logging.hpp"

#include <memory>

#if defined __linux__

namespace CascadeCore
{
    Renderer::Renderer(xcb_connection_t* connection_ptr, xcb_window_t* window_ptr, unsigned int width, unsigned int height) : m_width(width), m_height(height)
    {
        LOG_INFO << "Initializing renderer";

        std::shared_ptr<CascadeGraphics::Vulkan::Instance> instance_ptr = std::make_shared<CascadeGraphics::Vulkan::Instance>("Application name", 0);

        std::shared_ptr<CascadeGraphics::Vulkan::Surface> surface_ptr = std::make_shared<CascadeGraphics::Vulkan::Surface>(connection_ptr, window_ptr, instance_ptr);

        std::shared_ptr<CascadeGraphicsDebugging::Vulkan::Validation_Layer> validation_layer_ptr = std::make_shared<CascadeGraphicsDebugging::Vulkan::Validation_Layer>(instance_ptr);

        std::shared_ptr<CascadeGraphics::Vulkan::Queue_Manager> queue_manager_ptr = std::make_shared<CascadeGraphics::Vulkan::Queue_Manager>(true, true, false, false, false, true, surface_ptr);

        std::shared_ptr<CascadeGraphics::Vulkan::Physical_Device> physical_device_ptr = std::make_shared<CascadeGraphics::Vulkan::Physical_Device>(instance_ptr, queue_manager_ptr, surface_ptr);

        queue_manager_ptr->Set_Queue_Family_Indices(physical_device_ptr);

        std::shared_ptr<CascadeGraphics::Vulkan::Device> logical_device_ptr = std::make_shared<CascadeGraphics::Vulkan::Device>(queue_manager_ptr, validation_layer_ptr, physical_device_ptr);

        std::shared_ptr<CascadeGraphics::Vulkan::Swapchain> swapchain_ptr = std::make_shared<CascadeGraphics::Vulkan::Swapchain>(physical_device_ptr, surface_ptr, m_width, m_height);

        LOG_TRACE << "Renderer initialized";
    }

    Renderer::~Renderer()
    {
        LOG_TRACE << "Renderer cleanup started";

        LOG_TRACE << "Renderer cleanup finished";
    }
} // namespace CascadeCore

#elif defined _WIN32 || defined WIN32

namespace CascadeCore
{
    Renderer::Renderer(HINSTANCE* hinstance_ptr, HWND* hwnd_ptr, unsigned int width, unsigned int height) : m_width(width), m_height(height)
    {
        LOG_INFO << "Initializing renderer";

        std::shared_ptr<CascadeGraphics::Vulkan::Instance> instance_ptr = std::make_shared<CascadeGraphics::Vulkan::Instance>("Application name", 0);

        std::shared_ptr<CascadeGraphics::Vulkan::Surface> surface_ptr = std::make_shared<CascadeGraphics::Vulkan::Surface>(hinstance_ptr, hwnd_ptr, instance_ptr);

        std::shared_ptr<CascadeGraphicsDebugging::Vulkan::Validation_Layer> validation_layer_ptr = std::make_shared<CascadeGraphicsDebugging::Vulkan::Validation_Layer>(instance_ptr);

        std::shared_ptr<CascadeGraphics::Vulkan::Queue_Manager> queue_manager_ptr = std::make_shared<CascadeGraphics::Vulkan::Queue_Manager>(true, true, false, false, false, true, surface_ptr);

        std::shared_ptr<CascadeGraphics::Vulkan::Physical_Device> physical_device_ptr = std::make_shared<CascadeGraphics::Vulkan::Physical_Device>(instance_ptr, queue_manager_ptr, surface_ptr);

        queue_manager_ptr->Set_Queue_Family_Indices(physical_device_ptr);

        std::shared_ptr<CascadeGraphics::Vulkan::Device> logical_device_ptr = std::make_shared<CascadeGraphics::Vulkan::Device>(queue_manager_ptr, validation_layer_ptr, physical_device_ptr);

        std::shared_ptr<CascadeGraphics::Vulkan::Swapchain> swapchain_ptr = std::make_shared<CascadeGraphics::Vulkan::Swapchain>(physical_device_ptr, surface_ptr, m_width, m_height);

        LOG_TRACE << "Renderer initialized";
    }

    Renderer::~Renderer()
    {
        LOG_TRACE << "Renderer cleanup started";

        LOG_TRACE << "Renderer cleanup finished";
    }
} // namespace CascadeCore

#endif
