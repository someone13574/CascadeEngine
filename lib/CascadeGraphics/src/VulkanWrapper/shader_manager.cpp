#include "shader_manager.hpp"

#include "../debug_tools.hpp"

#include <fstream>

namespace CascadeGraphics
{
    namespace Vulkan
    {
        Shader_Manager::Shader_Manager(std::shared_ptr<Device> logical_device_ptr) : m_logical_device_ptr(logical_device_ptr)
        {
        }

        Shader_Manager::~Shader_Manager()
        {
            LOG_INFO << "Vulkan: cleaning up shaders";

            for (unsigned int i = 0; i < m_shaders.size(); i++)
            {
                LOG_TRACE << "Vulkan: destroying shader module " << m_shaders[i].label;

                vkDestroyShaderModule(*(m_logical_device_ptr->Get_Device()), m_shaders[i].shader_module, nullptr);
            }

            LOG_TRACE << "Vulkan: finished cleaning up shaders";
        }

        void Shader_Manager::Add_Shader(std::string label, std::string path)
        {
            LOG_INFO << "Vulkan: adding shader at " << path << " as " << label;

            for (unsigned int i = 0; i < m_shaders.size(); i++)
            {
                if (m_shaders[i].label == label)
                {
                    LOG_WARN << "Vulkan: adding shader with label already used, skipping";
                    return;
                }
                if (m_shaders[i].path == path)
                {
                    LOG_WARN << "Vulkan: shader with path already added";
                    break;
                }
            }

            std::ifstream file_stream(path.c_str(), std::ios::ate | std::ios::binary);

            if (!file_stream.is_open())
            {
                LOG_ERROR << "Vulkan: failed to open shader at path " << path;
                exit(EXIT_FAILURE);
            }

            size_t file_size = static_cast<size_t>(file_stream.tellg());
            std::vector<char> shader_buffer(file_size);

            file_stream.seekg(0);
            file_stream.read(shader_buffer.data(), file_size);
            file_stream.close();

            VkShaderModuleCreateInfo shader_module_create_info = {};
            shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            shader_module_create_info.pNext = NULL;
            shader_module_create_info.flags = 0;
            shader_module_create_info.codeSize = shader_buffer.size();
            shader_module_create_info.pCode = reinterpret_cast<const uint32_t*>(shader_buffer.data());

            m_shaders.resize(m_shaders.size() + 1);

            m_shaders.back() = {};
            m_shaders.back().label = label;
            m_shaders.back().path = path;
            VALIDATE_VKRESULT(vkCreateShaderModule(*(m_logical_device_ptr->Get_Device()), &shader_module_create_info, nullptr, &m_shaders.back().shader_module),
                              "Vulkan: failed to create shader module");

            LOG_TRACE << "Vulkan: finished adding shader";
        }
    } // namespace Vulkan
} // namespace CascadeGraphics