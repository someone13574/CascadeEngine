#pragma once

#include "logical_device_wrapper.hpp"
#include "vulkan_header.hpp"

#include <memory>
#include <string>
#include <vector>

namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        enum Shader_Type
        {
            COMPUTE,
            VERTEX,
            FRAGMENT
        };

        struct Shader_Info
        {
            std::string label;
            Shader_Type shader_type;
        };
        struct Shader
        {
            Shader_Info shader_info;
            std::string path;
            VkShaderModule shader_module;
        };

        class Shader_Manager
        {
        private:
            std::shared_ptr<Logical_Device_Wrapper> m_logical_device_ptr;

            std::vector<Shader> m_shaders;

        public:
            Shader_Manager(std::shared_ptr<Logical_Device_Wrapper> logical_device_ptr);
            ~Shader_Manager();

        public:
            void Add_Shader(std::string label, std::string path);

            VkShaderModule* Get_Shader_Module(std::string label);
            std::vector<Shader_Info> Get_Shaders_Details();
        };
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics