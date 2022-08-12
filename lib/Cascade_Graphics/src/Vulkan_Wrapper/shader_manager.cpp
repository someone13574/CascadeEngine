#include "shader_manager.hpp"

#include "debug_tools.hpp"

#include <fstream>

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

        Identifier Shader_Manager::Add_Shader(std::string label, std::string path)
        {
            Identifier identifier = {};
            identifier.label = label;
            identifier.index = 0;

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

            LOG_INFO << "Vulkan Backend: Adding shader at " << path << " as " << identifier.Get_Identifier_String();

            for (uint32_t i = 0; i < m_shaders.size(); i++)
            {
                if (m_shaders[i].file_path == path)
                {
                    LOG_WARN << "Vulkan Backend: Shader with path already added";
                }
            }

            std::ifstream file_stream(path.c_str(), std::ios::ate | std::ios::binary);

            if (!file_stream.is_open())
            {
                LOG_ERROR << "Vulkan Backend: Failed to open shader at path " << path;
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
                LOG_ERROR << "Vulkan Backend: SPIR-V doesn't have a file extension. Ensure the name of shader binaries are formatted like this 'shader_name.shader_type.spv' with shader_type being comp, frag, "
                             "vert, etc...";
                exit(EXIT_FAILURE);
            }

            m_shaders.resize(m_shaders.size() + 1);
            m_shaders.back() = {};
            m_shaders.back().identifier = identifier;
            m_shaders.back().file_path = path;
            VALIDATE_VKRESULT(vkCreateShaderModule(*m_logical_device_wrapper_ptr->Get_Device(), &shader_module_create_info, nullptr, &m_shaders.back().shader_module), "Vulkan Backend: Failed to create shader module");

            LOG_TRACE << "Vulkan Backend: Finished adding shader";
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