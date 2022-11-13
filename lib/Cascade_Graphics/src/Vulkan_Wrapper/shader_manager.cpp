#include "shader_manager.hpp"

#include "debug_tools.hpp"
#include <cstring>
#include <fstream>
#include <iostream>

namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        Shader_Manager::Shader_Manager(std::shared_ptr<Logical_Device_Wrapper> logical_device_wrapper_ptr) : m_logical_device_wrapper_ptr(logical_device_wrapper_ptr)
        {
        }

        Shader_Manager::~Shader_Manager()
        {
            LOG_INFO << "Vulkan Backend: Cleaning up shaders";

            for (uint32_t i = 0; i < m_shaders.size(); i++)
            {
                LOG_TRACE << "Vulkan Backend: Destroying shader module " << m_shaders[i].identifier.Get_Identifier_String();

                vkDestroyShaderModule(*m_logical_device_wrapper_ptr->Get_Device(), m_shaders[i].shader_module, nullptr);
            }

            LOG_TRACE << "Vulkan Backend: Finished cleaning up shaders";
        }

        void Shader_Manager::Load_Shader_Source(Identifier identifier)
        {
            Shader_Manager::Shader_Data* shader_data_ptr = Get_Shader_Data(identifier);

            LOG_INFO << "Vulkan Backend: Loading shader source from " << shader_data_ptr->file_path;

            unsigned int line_index = 0;
            std::string current_line;
            std::ifstream input_file_stream(shader_data_ptr->file_path, std::ios::in);

            if (input_file_stream.is_open())
            {
                while (!input_file_stream.eof())
                {
                    getline(input_file_stream, current_line);
                    LOG_TRACE << "#" << line_index++ << "  " << current_line;

                    shader_data_ptr->shader_source += current_line + "\n";
                }
            }
            else
            {
                LOG_ERROR << "Vulkan Backend: Shader source failed to open with errno " << errno << " (" << std::strerror(errno) << ")";
                exit(EXIT_FAILURE);
            }
        }

        void Shader_Manager::Compile_Shader(Identifier identifier)
        {
            Shader_Manager::Shader_Data* shader_data_ptr = Get_Shader_Data(identifier);

            LOG_INFO << "Vulkan Backend: Compiling shader " << identifier.Get_Identifier_String();

            shaderc::Compiler compiler;
            shaderc::CompileOptions compile_options;

            shader_data_ptr->compilation_result = compiler.CompileGlslToSpv(shader_data_ptr->shader_source, shaderc_shader_kind::shaderc_compute_shader, shader_data_ptr->file_path.c_str(), compile_options);

            if (shader_data_ptr->compilation_result.GetCompilationStatus() != shaderc_compilation_status_success)
            {
                LOG_ERROR << "Vulkan Backend: Failed to compile shader " << identifier.Get_Identifier_String() << " with error " << shader_data_ptr->compilation_result.GetErrorMessage();
                exit(EXIT_FAILURE);
            }
            else
            {
                LOG_TRACE << "Successfully compiled shader " << identifier.Get_Identifier_String();
            }
        }

        void Shader_Manager::Create_Shader_Module(Identifier identifier)
        {
            Shader_Manager::Shader_Data* shader_data_ptr = Get_Shader_Data(identifier);

            LOG_INFO << "Vulkan Backend: Creating shader module for " << identifier.Get_Identifier_String();

            std::vector<uint32_t> compiled_shader = {shader_data_ptr->compilation_result.cbegin(), shader_data_ptr->compilation_result.cend()};

            // Shader module create info
            VkShaderModuleCreateInfo shader_module_create_info = {};
            shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            shader_module_create_info.pNext = nullptr;
            shader_module_create_info.flags = 0;
            shader_module_create_info.codeSize = compiled_shader.size() * sizeof(uint32_t);
            shader_module_create_info.pCode = compiled_shader.data();

            VALIDATE_VKRESULT(vkCreateShaderModule(*m_logical_device_wrapper_ptr->Get_Device(), &shader_module_create_info, nullptr, &shader_data_ptr->shader_module), "Vulkan Backend: Failed to create shader module");
        }

        Identifier Shader_Manager::Add_Shader(std::string label, std::string path)
        {
            Identifier identifier = {};
            identifier.label = label;
            identifier.index = 0;

            // Find next valid identifier
            while (true)
            {
                bool index_in_use = false;

                for (uint32_t i = 0; i < m_shaders.size(); i++)
                {
                    index_in_use |= m_shaders[i].identifier == identifier;
                }

                if (!index_in_use)
                {
                    break;
                }
                else
                {
                    identifier.index++;
                }
            }

            m_shaders.resize(m_shaders.size() + 1);
            m_shaders.back() = {};
            m_shaders.back().identifier = identifier;
            m_shaders.back().file_path = path;
            m_shaders.back().shader_source = "";
            m_shaders.back().shader_module = VK_NULL_HANDLE;

            Load_Shader_Source(identifier);
            Compile_Shader(identifier);
            Create_Shader_Module(identifier);

            LOG_TRACE << "Vulkan Backend: Finished adding shader " << identifier.Get_Identifier_String();

            return identifier;
        }

        Shader_Manager::Shader_Data* Shader_Manager::Get_Shader_Data(Identifier identifier)
        {
            for (uint32_t i = 0; i < m_shaders.size(); i++)
            {
                if (m_shaders[i].identifier == identifier)
                {
                    return &m_shaders[i];
                }
            }

            LOG_ERROR << "Vulkan Backend: The shader " << identifier.Get_Identifier_String() << " does not exist";
            exit(EXIT_FAILURE);
        }
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics