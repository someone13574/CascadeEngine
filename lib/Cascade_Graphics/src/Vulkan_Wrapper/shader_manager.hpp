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
        class Shader_Manager
        {
        public:
            struct Shader_Data
            {
                std::string label;

                std::string file_path;
                VkShaderModule shader_module;
            };

        private:
            std::vector<Shader_Data> m_shaders;

            std::shared_ptr<Logical_Device_Wrapper> m_logical_device_wrapper_ptr;

        public:
            Shader_Manager(std::shared_ptr<Logical_Device_Wrapper> logical_device_wrapper_ptr);
            ~Shader_Manager();

        public:
            void Add_Shader(std::string label, std::string path);

            Shader_Data* Get_Shader_Data(std::string label);
        };
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics