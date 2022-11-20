#include "instance_builder.hpp"

#include "vkresult_translator.hpp"
#include <acorn_logging.hpp>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Instance_Builder::Instance_Builder()
        {
            m_instance_ptr = new Instance();

            m_application_info = {};
            m_application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            m_application_info.pNext = nullptr;
            m_application_info.pApplicationName = "";
            m_application_info.applicationVersion = 0;
            m_application_info.pEngineName = "Cascade";
            m_application_info.engineVersion = 0;
            m_application_info.apiVersion = VK_VERSION_1_0;

            m_enabled_layers.clear();
            m_enabled_extensions.clear();
        }

        Instance_Builder& Instance_Builder::Set_Application_Details(std::string application_name, uint32_t application_version)
        {
            m_application_info.pApplicationName = application_name.c_str();
            m_application_info.applicationVersion = application_version;

            return *this;
        }

        Instance_Builder& Instance_Builder::Set_Engine_Details(std::string engine_name, uint32_t engine_version)
        {
            m_application_info.pEngineName = engine_name.c_str();
            m_application_info.engineVersion = engine_version;

            return *this;
        }

        Instance_Builder& Instance_Builder::Set_Minimum_Vulkan_Version(uint32_t api_version)
        {
            m_application_info.apiVersion = api_version;

            return *this;
        }

        Instance_Builder& Instance_Builder::Add_Layer(const char* layer_name)
        {
            m_enabled_layers.push_back(layer_name);

            return *this;
        }

        Instance_Builder& Instance_Builder::Add_Extension(const char* extension_name)
        {
            m_enabled_extensions.push_back(extension_name);

            return *this;
        }

        Instance* Instance_Builder::Build()
        {
            LOG_INFO << "Graphics (Vulkan): Creating instance";

            VkInstanceCreateInfo instance_create_info = {};
            instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instance_create_info.pNext = nullptr;
            instance_create_info.flags = 0;
            instance_create_info.pApplicationInfo = &m_application_info;
            instance_create_info.enabledLayerCount = m_enabled_layers.size();
            instance_create_info.ppEnabledLayerNames = m_enabled_layers.data();
            instance_create_info.enabledExtensionCount = m_enabled_extensions.size();
            instance_create_info.ppEnabledExtensionNames = m_enabled_extensions.data();

            VkResult create_instance_result = vkCreateInstance(&instance_create_info, nullptr, m_instance_ptr->Get());
            if (create_instance_result != VK_SUCCESS)
            {
                LOG_FATAL << "Graphics (Vulkan): Call to vkCreateInstance failed with VkResult " << create_instance_result << " (" << Translate_VkResult(create_instance_result) << ")";
                exit(EXIT_FAILURE);
            }

            LOG_TRACE << "Graphics (Vulkan): Finished creating instance";

            return m_instance_ptr;
        }
    } // namespace Vulkan
} // namespace Cascade_Graphics