#pragma once

#include "instance_wrapper.hpp"
#include "vulkan_header.hpp"

#include <memory>
#include <vector>

namespace Cascade_Graphics_Debugging
{
    namespace Vulkan
    {
        class Validation_Layer
        {
        private:
            const std::vector<const char*> ENABLED_VALIDATION_LAYERS = {"VK_LAYER_KHRONOS_validation"};
            VkDebugUtilsMessengerEXT m_debug_messenger;

            std::shared_ptr<Cascade_Graphics::Vulkan::Instance> m_instance_ptr;

        public:
            Validation_Layer(std::shared_ptr<Cascade_Graphics::Vulkan::Instance> instance_ptr);
            ~Validation_Layer();

        public:
            static bool Check_Validation_Layer_Support(std::vector<const char*> layers_to_check);
            static VkDebugUtilsMessengerCreateInfoEXT Generate_Messenger_Create_Info();

            static std::vector<const char*> Get_Enabled_Validation_Layers();
        };
    } // namespace Vulkan
} // namespace Cascade_Graphics_Debugging