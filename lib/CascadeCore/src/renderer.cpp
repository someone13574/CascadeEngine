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

        std::shared_ptr<CascadeGraphics::Vulkan::Queue_Manager> queue_manager_ptr = std::make_shared<CascadeGraphics::Vulkan::Queue_Manager>(false, true, true, false, false, true, surface_ptr);

        std::shared_ptr<CascadeGraphics::Vulkan::Physical_Device> physical_device_ptr = std::make_shared<CascadeGraphics::Vulkan::Physical_Device>(instance_ptr, queue_manager_ptr, surface_ptr);

        queue_manager_ptr->Set_Queue_Family_Indices(physical_device_ptr);

        std::shared_ptr<CascadeGraphics::Vulkan::Device> logical_device_ptr = std::make_shared<CascadeGraphics::Vulkan::Device>(queue_manager_ptr, validation_layer_ptr, physical_device_ptr);

        std::shared_ptr<CascadeGraphics::Vulkan::Swapchain> swapchain_ptr
            = std::make_shared<CascadeGraphics::Vulkan::Swapchain>(logical_device_ptr, physical_device_ptr, surface_ptr, queue_manager_ptr, m_width, m_height);

        std::shared_ptr<CascadeGraphics::Vulkan::Storage_Manager> storage_manager_ptr
            = std::make_shared<CascadeGraphics::Vulkan::Storage_Manager>(logical_device_ptr, physical_device_ptr, queue_manager_ptr);

        storage_manager_ptr->Create_Buffer("geometry_buffer", sizeof(unsigned int) * 16, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                           {false, true, false, false, false, false});
        storage_manager_ptr->Create_Buffer("geometry_buffer", sizeof(unsigned int) * 16, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                           {false, true, false, false, false, false});
        storage_manager_ptr->Create_Buffer("geometry_buffer", sizeof(unsigned int) * 16, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                           {false, true, false, false, false, false});

        storage_manager_ptr->Create_Image("render_target", VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, {width, height},
                                          {false, true, true, false, false, false});
        storage_manager_ptr->Create_Image("render_target", VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, {width, height},
                                          {false, true, true, false, false, false});
        storage_manager_ptr->Create_Image("render_target", VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, {width, height},
                                          {false, true, true, false, false, false});

        std::shared_ptr<CascadeGraphics::Vulkan::Shader_Manager> shader_manager_ptr = std::make_shared<CascadeGraphics::Vulkan::Shader_Manager>(logical_device_ptr);

        shader_manager_ptr->Add_Shader("render_shader", "/home/owen/Documents/Code/C++/CascadeEngine/build/build/build/CascadeGraphics/src/Shaders/render.comp.spv");

        storage_manager_ptr->Create_Descriptor_Set();

        std::shared_ptr<CascadeGraphics::Vulkan::Pipeline> pipeline_ptr = std::make_shared<CascadeGraphics::Vulkan::Pipeline>(logical_device_ptr, storage_manager_ptr, shader_manager_ptr);

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

        std::shared_ptr<CascadeGraphics::Vulkan::Queue_Manager> queue_manager_ptr = std::make_shared<CascadeGraphics::Vulkan::Queue_Manager>(false, true, true, false, false, true, surface_ptr);

        std::shared_ptr<CascadeGraphics::Vulkan::Physical_Device> physical_device_ptr = std::make_shared<CascadeGraphics::Vulkan::Physical_Device>(instance_ptr, queue_manager_ptr, surface_ptr);

        queue_manager_ptr->Set_Queue_Family_Indices(physical_device_ptr);

        std::shared_ptr<CascadeGraphics::Vulkan::Device> logical_device_ptr = std::make_shared<CascadeGraphics::Vulkan::Device>(queue_manager_ptr, validation_layer_ptr, physical_device_ptr);

        std::shared_ptr<CascadeGraphics::Vulkan::Swapchain> swapchain_ptr
            = std::make_shared<CascadeGraphics::Vulkan::Swapchain>(logical_device_ptr, physical_device_ptr, surface_ptr, queue_manager_ptr, m_width, m_height);

        std::shared_ptr<CascadeGraphics::Vulkan::Storage_Manager> storage_manager_ptr
            = std::make_shared<CascadeGraphics::Vulkan::Storage_Manager>(logical_device_ptr, physical_device_ptr, queue_manager_ptr);

        storage_manager_ptr->Create_Buffer("geometry_buffer", sizeof(unsigned int) * 16, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, {false, true, false, false, false, false});
        storage_manager_ptr->Create_Buffer("geometry_buffer", sizeof(unsigned int) * 16, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, {false, true, false, false, false, false});
        storage_manager_ptr->Create_Buffer("geometry_buffer", sizeof(unsigned int) * 16, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, {false, true, false, false, false, false});

        storage_manager_ptr->Create_Image("render_target", VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT, {width, height},
                                          {false, true, true, false, false, false});
        storage_manager_ptr->Create_Image("render_target", VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT, {width, height},
                                          {false, true, true, false, false, false});
        storage_manager_ptr->Create_Image("render_target", VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT, {width, height},
                                          {false, true, true, false, false, false});

        std::shared_ptr<CascadeGraphics::Vulkan::Pipeline> pipeline_ptr = std::make_shared<CascadeGraphics::Vulkan::Pipeline>(logical_device_ptr);

        LOG_TRACE << "Renderer initialized";
    }

    Renderer::~Renderer()
    {
        LOG_TRACE << "Renderer cleanup started";

        LOG_TRACE << "Renderer cleanup finished";
    }
} // namespace CascadeCore

#endif
