#pragma once

#include "../vulkan_header.hpp"
#include "device_wrapper.hpp"

#include <memory>
#include <string>
#include <vector>

namespace CascadeGraphics
{
    namespace Vulkan
    {
        struct Shader
        {
            std::string label;
            std::string path;
            VkShaderModule shader_module;
        };

        class Shader_Manager
        {
        private:
            std::shared_ptr<Device> m_logical_device_ptr;

            std::vector<Shader> m_shaders;

        public:
            Shader_Manager(std::shared_ptr<Device> logical_device_ptr);
            ~Shader_Manager();

            void Add_Shader(std::string label, std::string path);
            VkShaderModule* Get_Shader_Module(std::string label);
        };
    } // namespace Vulkan
} // namespace CascadeGraphics