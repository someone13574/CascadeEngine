#pragma once

#include "instance_wrapper.hpp"
#include "vulkan_header.hpp"
#include <memory>
#include <vector>


namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        class Validation_Layer_Wrapper
        {
        private:
            VkDebugUtilsMessengerEXT m_debug_messenger;

            std::shared_ptr<Cascade_Graphics::Vulkan_Backend::Instance_Wrapper> m_instance_wrapper_ptr;

        public:
            Validation_Layer_Wrapper(std::shared_ptr<Instance_Wrapper> instance_wrapper_ptr);
            ~Validation_Layer_Wrapper();

        public:
            static bool Check_Validation_Layer_Support(std::vector<const char*> layers_to_check);
            static VkDebugUtilsMessengerCreateInfoEXT Generate_Messenger_Create_Info();

            static std::vector<const char*> Get_Enabled_Validation_Layers();
        };
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics