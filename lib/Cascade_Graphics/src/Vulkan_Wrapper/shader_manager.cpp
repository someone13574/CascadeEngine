#include "shader_manager.hpp"

#include "debug_tools.hpp"

#include <fstream>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Shader_Manager::Shader_Manager(std::shared_ptr<Device> logical_device_ptr) : m_logical_device_ptr(logical_device_ptr)
        {
        }

        Shader_Manager::~Shader_Manager()
        {
            LOG_INFO << "Vulkan: Cleaning up shaders";

            for (unsigned int i = 0; i < m_shaders.size(); i++)
            {
                LOG_TRACE << "Vulkan: Destroying shader module " << m_shaders[i].shader_info.label;

                vkDestroyShaderModule(*m_logical_device_ptr->Get_Device(), m_shaders[i].shader_module, nullptr);
            }

            LOG_TRACE << "Vulkan: Finished cleaning up shaders";
        }

        void Shader_Manager::Add_Shader(std::string label, std::string path)
        {
            LOG_INFO << "Vulkan: Adding shader at " << path << " as " << label;

            for (unsigned int i = 0; i < m_shaders.size(); i++)
            {
                if (m_shaders[i].shader_info.label == label)
                {
                    LOG_WARN << "Vulkan: Adding shader with label already used, skipping";
                    return;
                }
                if (m_shaders[i].path == path)
                {
                    LOG_WARN << "Vulkan: Shader with path already added";
                    break;
                }
            }

            std::ifstream file_stream(path.c_str(), std::ios::ate | std::ios::binary);

            if (!file_stream.is_open())
            {
                LOG_ERROR << "Vulkan: Failed to open shader at path " << path;
                exit(EXIT_FAILURE);
            }

            size_t file_size = static_cast<size_t>(file_stream.tellg());
            std::vector<char> shader_buffer(file_size);

            file_stream.seekg(0);
            file_stream.read(shader_buffer.data(), file_size);
            file_stream.close();

            VkShaderModuleCreateInfo shader_module_create_info = {};
            shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            shader_module_create_info.pNext = nullptr;
            shader_module_create_info.flags = 0;
            shader_module_create_info.codeSize = shader_buffer.size();
            shader_module_create_info.pCode = reinterpret_cast<const uint32_t*>(shader_buffer.data());

            size_t last_index = path.find_last_of(".");
            if (last_index == std::string::npos)
            {
                LOG_ERROR << "Vulkan: SPIR-V doesn't have a file extension. Ensure the name of shader binaries are formatted like this 'shader_name.shader_type.spv' with shader_type being comp, frag, "
                             "vert, etc...";
                exit(EXIT_FAILURE);
            }
            std::string file_extension = path.substr(0, last_index);
            last_index = file_extension.find_last_of(".");
            if (last_index == std::string::npos)
            {
                LOG_ERROR << "Vulkan: SPIR-V file name doesn't contain shader type. Ensure the name of shader binaries are formatted like this 'shader_name.shader_type.spv' with shader_type being "
                             "comp, frag, "
                             "vert, etc...";
                exit(EXIT_FAILURE);
            }
            file_extension = file_extension.substr(last_index + 1);

            Shader_Type shader_type;
            if (file_extension == "comp")
            {
                shader_type = Shader_Type::COMPUTE;
            }
            else if (file_extension == "frag")
            {
                shader_type = Shader_Type::FRAGMENT;
            }
            else if (file_extension == "vert")
            {
                shader_type = Shader_Type::VERTEX;
            }
            else
            {
                LOG_ERROR << "Vulkan: Unknown shader type " << file_extension;
                exit(EXIT_FAILURE);
            }

            m_shaders.resize(m_shaders.size() + 1);
            m_shaders.back() = {};
            m_shaders.back().shader_info.label = label;
            m_shaders.back().shader_info.shader_type = shader_type;
            m_shaders.back().path = path;
            VALIDATE_VKRESULT(vkCreateShaderModule(*m_logical_device_ptr->Get_Device(), &shader_module_create_info, nullptr, &m_shaders.back().shader_module), "Vulkan: Failed to create shader module");

            LOG_TRACE << "Vulkan: Finished adding shader";
        }

        VkShaderModule* Shader_Manager::Get_Shader_Module(std::string label)
        {
            for (unsigned int i = 0; i < m_shaders.size(); i++)
            {
                if (m_shaders[i].shader_info.label == label)
                {
                    return &m_shaders[i].shader_module;
                }
            }

            LOG_ERROR << "Vulkan: Couldn't find requested shader " << label;
            exit(EXIT_FAILURE);
        }

        std::vector<Shader_Info> Shader_Manager::Get_Shaders_Details()
        {
            std::vector<Shader_Info> shaders_details(m_shaders.size());

            for (unsigned int i = 0; i < m_shaders.size(); i++)
            {
                shaders_details[i] = m_shaders[i].shader_info;
            }

            return shaders_details;
        }
    } // namespace Vulkan
} // namespace Cascade_Graphics