#include "renderer.hpp"

#include "cascade_logging.hpp"

#include <cmath>
#include <memory>

namespace CGV = CascadeGraphics::Vulkan;

namespace CascadeCore
{
    Renderer::Renderer(CascadeGraphics::Vulkan::Surface::Window_Data window_data, unsigned int width, unsigned int height) : m_width(width), m_height(height)
    {
        LOG_INFO << "Initializing renderer";

        std::shared_ptr<CGV::Instance> instance_ptr = std::make_shared<CGV::Instance>("Application name", 0);

        std::shared_ptr<CGV::Surface> surface_ptr = std::make_shared<CGV::Surface>(window_data, instance_ptr);

        std::shared_ptr<CascadeGraphicsDebugging::Vulkan::Validation_Layer> validation_layer_ptr = std::make_shared<CascadeGraphicsDebugging::Vulkan::Validation_Layer>(instance_ptr);

        std::shared_ptr<CGV::Queue_Manager> queue_manager_ptr = std::make_shared<CGV::Queue_Manager>(false, true, true, false, false, true, surface_ptr);

        std::shared_ptr<CGV::Physical_Device> physical_device_ptr = std::make_shared<CGV::Physical_Device>(instance_ptr, queue_manager_ptr, surface_ptr);

        queue_manager_ptr->Set_Queue_Family_Indices(physical_device_ptr);

        std::shared_ptr<CGV::Device> logical_device_ptr = std::make_shared<CGV::Device>(queue_manager_ptr, validation_layer_ptr, physical_device_ptr);

        std::shared_ptr<CGV::Swapchain> swapchain_ptr = std::make_shared<CGV::Swapchain>(logical_device_ptr, physical_device_ptr, surface_ptr, queue_manager_ptr, m_width, m_height);

        std::shared_ptr<CGV::Storage_Manager> storage_manager_ptr = std::make_shared<CGV::Storage_Manager>(logical_device_ptr, physical_device_ptr, queue_manager_ptr, swapchain_ptr);
        storage_manager_ptr->Create_Image("render_target", VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, {width, height}, {false, true, true, false, false, false});
        storage_manager_ptr->Create_Image("render_target", VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, {width, height}, {false, true, true, false, false, false});
        storage_manager_ptr->Create_Image("render_target", VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, {width, height}, {false, true, true, false, false, false});
        storage_manager_ptr->Add_Swapchain("swapchain");

        std::shared_ptr<CGV::Shader_Manager> shader_manager_ptr = std::make_shared<CGV::Shader_Manager>(logical_device_ptr);

#if defined __linux__
        shader_manager_ptr->Add_Shader("render_shader", "/home/owen/Documents/Code/C++/CascadeEngine/build/build/build/CascadeGraphics/src/Shaders/render.comp.spv");
#elif defined _WIN32 || defined WIN32
        shader_manager_ptr->Add_Shader("render_shader", "C:/Users/Owen Law/Documents/Code/C++/CascadeEngine/build/build/build/CascadeGraphics/src/Shaders/render.comp.spv");
#endif

        std::shared_ptr<CGV::Resource_Grouping_Manager> resource_grouping_manager = std::make_shared<CGV::Resource_Grouping_Manager>(logical_device_ptr, storage_manager_ptr);
        resource_grouping_manager->Add_Resource_Grouping("per_frame_descriptor_set", {{0, "render_target", CGV::Storage_Manager::IMAGE}, {1, "render_target", CGV::Storage_Manager::IMAGE}, {2, "render_target", CGV::Storage_Manager::IMAGE}}, true);
        resource_grouping_manager->Add_Resource_Grouping("swapchain", {{0, "swapchain", CGV::Storage_Manager::SWAPCHAIN_IMAGE}, {1, "swapchain", CGV::Storage_Manager::SWAPCHAIN_IMAGE}, {2, "swapchain", CGV::Storage_Manager::SWAPCHAIN_IMAGE}}, false);
        resource_grouping_manager->Allocate_Descriptor_Sets();

        std::shared_ptr<CGV::Pipeline_Manager> pipeline_manager_ptr = std::make_shared<CGV::Pipeline_Manager>(resource_grouping_manager, logical_device_ptr, storage_manager_ptr, shader_manager_ptr);
        pipeline_manager_ptr->Add_Compute_Pipeline("rendering_pipeline", "per_frame_descriptor_set", "render_shader");

        std::shared_ptr<CGV::Command_Buffer_Manager> command_buffer_manager_ptr = std::make_shared<CGV::Command_Buffer_Manager>(resource_grouping_manager, logical_device_ptr, pipeline_manager_ptr, storage_manager_ptr);

        command_buffer_manager_ptr->Add_Command_Buffer("render_frame_0", queue_manager_ptr->Get_Queue_Family_Indices().m_compute_index.value(), {"per_frame_descriptor_set", "swapchain"}, "rendering_pipeline");
        command_buffer_manager_ptr->Begin_Recording("render_frame_0", (VkCommandBufferUsageFlagBits)0);
        command_buffer_manager_ptr->Image_Memory_Barrier("render_frame_0", {0, "swapchain", CGV::Storage_Manager::SWAPCHAIN_IMAGE}, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT);
        command_buffer_manager_ptr->Image_Memory_Barrier("render_frame_0", {0, "render_target", CGV::Storage_Manager::IMAGE}, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        command_buffer_manager_ptr->Dispatch_Compute_Shader("render_frame_0", std::ceil(m_width / 32.0), std::ceil(m_width / 32.0), 1);
        command_buffer_manager_ptr->Image_Memory_Barrier("render_frame_0", {0, "render_target", CGV::Storage_Manager::IMAGE}, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT);
        command_buffer_manager_ptr->Copy_Image("render_frame_0", {0, "render_target", CGV::Storage_Manager::IMAGE}, {0, "swapchain", CGV::Storage_Manager::SWAPCHAIN_IMAGE});
        command_buffer_manager_ptr->Image_Memory_Barrier("render_frame_0", {0, "render_target", CGV::Storage_Manager::IMAGE}, VK_ACCESS_NONE, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_TRANSFER_BIT);
        command_buffer_manager_ptr->Image_Memory_Barrier("render_frame_0", {0, "swapchain", CGV::Storage_Manager::SWAPCHAIN_IMAGE}, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_TRANSFER_BIT);
        command_buffer_manager_ptr->End_Recording("render_frame_0");

        command_buffer_manager_ptr->Add_Command_Buffer("render_frame_1", queue_manager_ptr->Get_Queue_Family_Indices().m_compute_index.value(), {"per_frame_descriptor_set", "swapchain"}, "rendering_pipeline");
        command_buffer_manager_ptr->Begin_Recording("render_frame_1", (VkCommandBufferUsageFlagBits)0);
        command_buffer_manager_ptr->Image_Memory_Barrier("render_frame_1", {1, "swapchain", CGV::Storage_Manager::SWAPCHAIN_IMAGE}, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT);
        command_buffer_manager_ptr->Image_Memory_Barrier("render_frame_1", {1, "render_target", CGV::Storage_Manager::IMAGE}, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        command_buffer_manager_ptr->Dispatch_Compute_Shader("render_frame_1", std::ceil(m_width / 32.0), std::ceil(m_width / 32.0), 1);
        command_buffer_manager_ptr->Image_Memory_Barrier("render_frame_1", {1, "render_target", CGV::Storage_Manager::IMAGE}, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT);
        command_buffer_manager_ptr->Copy_Image("render_frame_1", {1, "render_target", CGV::Storage_Manager::IMAGE}, {1, "swapchain", CGV::Storage_Manager::SWAPCHAIN_IMAGE});
        command_buffer_manager_ptr->Image_Memory_Barrier("render_frame_1", {1, "render_target", CGV::Storage_Manager::IMAGE}, VK_ACCESS_NONE, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_TRANSFER_BIT);
        command_buffer_manager_ptr->Image_Memory_Barrier("render_frame_1", {1, "swapchain", CGV::Storage_Manager::SWAPCHAIN_IMAGE}, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_TRANSFER_BIT);
        command_buffer_manager_ptr->End_Recording("render_frame_1");

        command_buffer_manager_ptr->Add_Command_Buffer("render_frame_2", queue_manager_ptr->Get_Queue_Family_Indices().m_compute_index.value(), {"per_frame_descriptor_set", "swapchain"}, "rendering_pipeline");
        command_buffer_manager_ptr->Begin_Recording("render_frame_2", (VkCommandBufferUsageFlagBits)0);
        command_buffer_manager_ptr->Image_Memory_Barrier("render_frame_2", {2, "swapchain", CGV::Storage_Manager::SWAPCHAIN_IMAGE}, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT);
        command_buffer_manager_ptr->Image_Memory_Barrier("render_frame_2", {2, "render_target", CGV::Storage_Manager::IMAGE}, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        command_buffer_manager_ptr->Dispatch_Compute_Shader("render_frame_2", std::ceil(m_width / 32.0), std::ceil(m_width / 32.0), 1);
        command_buffer_manager_ptr->Image_Memory_Barrier("render_frame_2", {2, "render_target", CGV::Storage_Manager::IMAGE}, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT);
        command_buffer_manager_ptr->Copy_Image("render_frame_2", {2, "render_target", CGV::Storage_Manager::IMAGE}, {2, "swapchain", CGV::Storage_Manager::SWAPCHAIN_IMAGE});
        command_buffer_manager_ptr->Image_Memory_Barrier("render_frame_2", {2, "render_target", CGV::Storage_Manager::IMAGE}, VK_ACCESS_NONE, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_TRANSFER_BIT);
        command_buffer_manager_ptr->Image_Memory_Barrier("render_frame_2", {2, "swapchain", CGV::Storage_Manager::SWAPCHAIN_IMAGE}, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_TRANSFER_BIT);
        command_buffer_manager_ptr->End_Recording("render_frame_2");

        LOG_INFO << "Renderer initialized";
    }

    Renderer::~Renderer()
    {
        LOG_TRACE << "Renderer cleanup started";

        LOG_TRACE << "Renderer cleanup finished";
    }

    void Renderer::Render_Frame()
    {
    }
} // namespace CascadeCore