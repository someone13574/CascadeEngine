#include "vulkan_graphics.hpp"

#include <set>

namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        Vulkan_Graphics::Vulkan_Graphics()
        {
            std::lock_guard<std::mutex> vulkan_objects_acess_lock(m_vulkan_objects_access_mutex);

            LOG_DEBUG << "Vulkan Backend: Initializing Vulkan objects";

            std::set<const char*> required_instance_extensions = {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME};
            std::set<const char*> required_device_extensions = {VK_EXT_MEMORY_BUDGET_EXTENSION_NAME};

            m_instance_wrapper_ptr = std::make_shared<Instance_Wrapper>("Application name", 0, required_instance_extensions);
            m_validation_layer_manager_ptr = std::make_shared<Validation_Layer_Wrapper>(m_instance_wrapper_ptr);
            m_queue_manager_ptr = std::make_shared<Queue_Manager>(Queue_Manager::COMPUTE_QUEUE | Queue_Manager::TRANSFER_QUEUE | Queue_Manager::PRESENT_QUEUE);
            m_physical_device_wrapper_ptr = std::make_shared<Physical_Device_Wrapper>(m_instance_wrapper_ptr, m_queue_manager_ptr, required_device_extensions);
            m_logical_device_wrapper_ptr = std::make_shared<Logical_Device_Wrapper>(m_physical_device_wrapper_ptr, m_queue_manager_ptr, m_validation_layer_manager_ptr);
            m_storage_manager_ptr = std::make_shared<Storage_Manager>(m_logical_device_wrapper_ptr, m_physical_device_wrapper_ptr, m_queue_manager_ptr);
            m_shader_manager_ptr = std::make_shared<Shader_Manager>(m_logical_device_wrapper_ptr);
            m_descriptor_set_manager_ptr = std::make_shared<Descriptor_Set_Manager>(m_logical_device_wrapper_ptr, m_storage_manager_ptr);
            m_pipeline_manager_ptr = std::make_shared<Pipeline_Manager>(m_descriptor_set_manager_ptr, m_logical_device_wrapper_ptr, m_shader_manager_ptr, m_storage_manager_ptr);
            m_command_buffer_manager_ptr = std::make_shared<Command_Buffer_Manager>(m_descriptor_set_manager_ptr, m_logical_device_wrapper_ptr, m_pipeline_manager_ptr, m_storage_manager_ptr);
            m_synchronization_manager_ptr = std::make_shared<Synchronization_Manager>(m_logical_device_wrapper_ptr);

            m_is_vulkan_initialized = true;
            m_vulkan_object_access_notify.notify_all();

            LOG_DEBUG << "Vulkan Backend: Finished initializing Vulkan objects";
        }

        Vulkan_Graphics::~Vulkan_Graphics()
        {
            std::lock_guard<std::mutex> vulkan_objects_acess_lock(m_vulkan_objects_access_mutex);

            LOG_DEBUG << "Graphics: Destroying Vulkan backend";

            if (m_is_vulkan_initialized)
            {
                vkDeviceWaitIdle(*m_logical_device_wrapper_ptr->Get_Device());

                m_synchronization_manager_ptr.reset();
                m_command_buffer_manager_ptr.reset();
                m_pipeline_manager_ptr.reset();
                m_descriptor_set_manager_ptr.reset();
                m_shader_manager_ptr.reset();
                m_storage_manager_ptr.reset();
                m_logical_device_wrapper_ptr.reset();
                m_physical_device_wrapper_ptr.reset();
                m_queue_manager_ptr.reset();
                m_validation_layer_manager_ptr.reset();
                m_instance_wrapper_ptr.reset();
            }
        }

        bool Vulkan_Graphics::Is_Vulkan_Initialized()
        {
            return m_is_vulkan_initialized;
        }
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics