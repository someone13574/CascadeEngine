#pragma once

#include "command_buffer_manager.hpp"
#include "debug_tools.hpp"
#include "descriptor_set_manager.hpp"
#include "identifier.hpp"
#include "instance_wrapper.hpp"
#include "logical_device_wrapper.hpp"
#include "physical_device_wrapper.hpp"
#include "pipeline_manager.hpp"
#include "queue_manager.hpp"
#include "shader_manager.hpp"
#include "storage_manager.hpp"
#include "synchronization_manager.hpp"
#include "validation_layer_wrapper.hpp"
#include <condition_variable>
#include <memory>
#include <mutex>


namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        class Vulkan_Graphics
        {
        private:
            bool m_is_vulkan_initialized = false;

        public:
            Vulkan_Graphics();
            ~Vulkan_Graphics();

        public:
            std::mutex m_vulkan_objects_access_mutex;
            std::condition_variable m_vulkan_object_access_notify;

            std::shared_ptr<Command_Buffer_Manager> m_command_buffer_manager_ptr;
            std::shared_ptr<Descriptor_Set_Manager> m_descriptor_set_manager_ptr;
            std::shared_ptr<Instance_Wrapper> m_instance_wrapper_ptr;
            std::shared_ptr<Logical_Device_Wrapper> m_logical_device_wrapper_ptr;
            std::shared_ptr<Physical_Device_Wrapper> m_physical_device_wrapper_ptr;
            std::shared_ptr<Pipeline_Manager> m_pipeline_manager_ptr;
            std::shared_ptr<Queue_Manager> m_queue_manager_ptr;
            std::shared_ptr<Shader_Manager> m_shader_manager_ptr;
            std::shared_ptr<Storage_Manager> m_storage_manager_ptr;
            std::shared_ptr<Synchronization_Manager> m_synchronization_manager_ptr;
            std::shared_ptr<Validation_Layer_Wrapper> m_validation_layer_manager_ptr;

        public:
            bool Is_Vulkan_Initialized();
        };
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics