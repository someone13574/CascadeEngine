#pragma once

#include "identifier.hpp"
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
                Identifier identifier;

                std::string file_path;
                std::string shader_source;
                VkShaderModule shader_module;
            };

        private:
            std::vector<Shader_Data> m_shaders;

            std::shared_ptr<Logical_Device_Wrapper> m_logical_device_wrapper_ptr;

        private:
            void Load_Shader_Source(Identifier identifier);
            void Compile_Shader(Identifier identifier);

        public:
            Shader_Manager(std::shared_ptr<Logical_Device_Wrapper> logical_device_wrapper_ptr);
            ~Shader_Manager();

        public:
            Identifier Add_Shader(std::string label, std::string path);

            Shader_Data* Get_Shader_Data(Identifier identifier);
        };
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics