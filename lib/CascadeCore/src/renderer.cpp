#include "renderer.hpp"

#include "cascade_graphics.hpp"
#include "cascade_logging.hpp"

#include <cmath>
#include <memory>

#if defined __linux__

namespace CGV = CascadeGraphics::Vulkan;

namespace CascadeCore
{
    Renderer::Renderer(xcb_connection_t* connection_ptr, xcb_window_t* window_ptr, unsigned int width, unsigned int height) : m_width(width), m_height(height)
    {
        LOG_INFO << "Initializing renderer";

        std::shared_ptr<CGV::Instance> instance_ptr = std::make_shared<CGV::Instance>("Application name", 0);

        std::shared_ptr<CGV::Surface> surface_ptr = std::make_shared<CGV::Surface>(connection_ptr, window_ptr, instance_ptr);

        std::shared_ptr<CascadeGraphicsDebugging::Vulkan::Validation_Layer> validation_layer_ptr = std::make_shared<CascadeGraphicsDebugging::Vulkan::Validation_Layer>(instance_ptr);

        std::shared_ptr<CGV::Queue_Manager> queue_manager_ptr = std::make_shared<CGV::Queue_Manager>(false, true, true, false, false, true, surface_ptr);

        std::shared_ptr<CGV::Physical_Device> physical_device_ptr = std::make_shared<CGV::Physical_Device>(instance_ptr, queue_manager_ptr, surface_ptr);

        queue_manager_ptr->Set_Queue_Family_Indices(physical_device_ptr);

        std::shared_ptr<CGV::Device> logical_device_ptr = std::make_shared<CGV::Device>(queue_manager_ptr, validation_layer_ptr, physical_device_ptr);

        std::shared_ptr<CGV::Swapchain> swapchain_ptr = std::make_shared<CGV::Swapchain>(logical_device_ptr, physical_device_ptr, surface_ptr, queue_manager_ptr, m_width, m_height);

        std::shared_ptr<CGV::Storage_Manager> storage_manager_ptr = std::make_shared<CGV::Storage_Manager>(logical_device_ptr, physical_device_ptr, queue_manager_ptr);
        storage_manager_ptr->Create_Image("render_target", VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, {width, height}, {false, true, true, false, false, false});

        std::shared_ptr<CGV::Shader_Manager> shader_manager_ptr = std::make_shared<CGV::Shader_Manager>(logical_device_ptr);
        shader_manager_ptr->Add_Shader("render_shader", "/home/owen/Documents/Code/C++/CascadeEngine/build/build/build/CascadeGraphics/src/Shaders/render.comp.spv");

        std::shared_ptr<CGV::Descriptor_Set_Manager> descriptor_set_manager_ptr = std::make_shared<CGV::Descriptor_Set_Manager>(logical_device_ptr, storage_manager_ptr);
        descriptor_set_manager_ptr->Add_Descriptor_Set("main_descriptor_set", {{0, "render_target", CGV::Storage_Manager::IMAGE}});
        descriptor_set_manager_ptr->Allocate_Descriptor_Sets();

        std::shared_ptr<CGV::Pipeline_Manager> pipeline_manager_ptr = std::make_shared<CGV::Pipeline_Manager>(descriptor_set_manager_ptr, logical_device_ptr, storage_manager_ptr, shader_manager_ptr);
        pipeline_manager_ptr->Add_Compute_Pipeline("main_render_pipeline", "main_descriptor_set", "render_shader");

        std::shared_ptr<CGV::Command_Buffer_Manager> command_buffer_manager_ptr = std::make_shared<CGV::Command_Buffer_Manager>(descriptor_set_manager_ptr, logical_device_ptr, pipeline_manager_ptr, storage_manager_ptr);
        command_buffer_manager_ptr->Add_Command_Buffer("execute_render", queue_manager_ptr->Get_Queue_Family_Indices().m_compute_index.value(), "main_descriptor_set", "main_render_pipeline");
        command_buffer_manager_ptr->Begin_Recording("execute_render", (VkCommandBufferUsageFlagBits)0);
        command_buffer_manager_ptr->Image_Memory_Barrier("execute_render", {0, "render_target", CGV::Storage_Manager::IMAGE}, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        command_buffer_manager_ptr->Dispatch_Compute_Shader("execute_render", std::ceil(m_width / 32.0), std::ceil(m_width / 32.0), 1);
        command_buffer_manager_ptr->End_Recording("execute_render");

        LOG_INFO << "Renderer initialized";
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
    namespace CGV = CascadeGraphics::Vulkan;

    Renderer::Renderer(HINSTANCE* hinstance_ptr, HWND* hwnd_ptr, unsigned int width, unsigned int height) : m_width(width), m_height(height)
    {
        LOG_INFO << "Initializing renderer";

        std::shared_ptr<CGV::Instance> instance_ptr = std::make_shared<CGV::Instance>("Application name", 0);

        std::shared_ptr<CGV::Surface> surface_ptr = std::make_shared<CGV::Surface>(hinstance_ptr, hwnd_ptr, instance_ptr);

        std::shared_ptr<CascadeGraphicsDebugging::Vulkan::Validation_Layer> validation_layer_ptr = std::make_shared<CascadeGraphicsDebugging::Vulkan::Validation_Layer>(instance_ptr);

        std::shared_ptr<CGV::Queue_Manager> queue_manager_ptr = std::make_shared<CGV::Queue_Manager>(false, true, true, false, false, true, surface_ptr);

        std::shared_ptr<CGV::Physical_Device> physical_device_ptr = std::make_shared<CGV::Physical_Device>(instance_ptr, queue_manager_ptr, surface_ptr);

        queue_manager_ptr->Set_Queue_Family_Indices(physical_device_ptr);

        std::shared_ptr<CGV::Device> logical_device_ptr = std::make_shared<CGV::Device>(queue_manager_ptr, validation_layer_ptr, physical_device_ptr);

        std::shared_ptr<CGV::Swapchain> swapchain_ptr = std::make_shared<CGV::Swapchain>(logical_device_ptr, physical_device_ptr, surface_ptr, queue_manager_ptr, m_width, m_height);

        std::shared_ptr<CGV::Storage_Manager> storage_manager_ptr = std::make_shared<CGV::Storage_Manager>(logical_device_ptr, physical_device_ptr, queue_manager_ptr);
        storage_manager_ptr->Create_Image("render_target", VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, {width, height}, {false, true, true, false, false, false});

        std::shared_ptr<CGV::Shader_Manager> shader_manager_ptr = std::make_shared<CGV::Shader_Manager>(logical_device_ptr);
        shader_manager_ptr->Add_Shader("render_shader", "C:/Users/Owen Law/Documents/Code/C++/CascadeEngine/build/build/build/CascadeGraphics/src/Shaders/render.comp.spv");

        std::shared_ptr<CGV::Descriptor_Set_Manager> descriptor_set_manager_ptr = std::make_shared<CGV::Descriptor_Set_Manager>(logical_device_ptr, storage_manager_ptr);
        descriptor_set_manager_ptr->Add_Descriptor_Set("main_descriptor_set", {{0, "render_target", CGV::Storage_Manager::IMAGE}});
        descriptor_set_manager_ptr->Allocate_Descriptor_Sets();

        std::shared_ptr<CGV::Pipeline_Manager> pipeline_manager_ptr = std::make_shared<CGV::Pipeline_Manager>(descriptor_set_manager_ptr, logical_device_ptr, storage_manager_ptr, shader_manager_ptr);
        pipeline_manager_ptr->Add_Compute_Pipeline("main_render_pipeline", "main_descriptor_set", "render_shader");

        std::shared_ptr<CGV::Command_Buffer_Manager> command_buffer_manager_ptr = std::make_shared<CGV::Command_Buffer_Manager>(descriptor_set_manager_ptr, logical_device_ptr, pipeline_manager_ptr, storage_manager_ptr);
        command_buffer_manager_ptr->Add_Command_Buffer("execute_render", queue_manager_ptr->Get_Queue_Family_Indices().m_compute_index.value(), "main_descriptor_set", "main_render_pipeline");
        command_buffer_manager_ptr->Begin_Recording("execute_render", (VkCommandBufferUsageFlagBits)0);
        command_buffer_manager_ptr->Image_Memory_Barrier("execute_render", {0, "render_target", CGV::Storage_Manager::IMAGE}, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        command_buffer_manager_ptr->Dispatch_Compute_Shader("execute_render", std::ceil(m_width / 32.0), std::ceil(m_width / 32.0), 1);
        command_buffer_manager_ptr->End_Recording("execute_render");

        LOG_INFO << "Renderer initialized";
    }

    Renderer::~Renderer()
    {
        LOG_TRACE << "Renderer cleanup started";

        LOG_TRACE << "Renderer cleanup finished";
    }
} // namespace CascadeCore

#endif
