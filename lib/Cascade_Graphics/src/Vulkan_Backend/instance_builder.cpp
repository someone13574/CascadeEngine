#include "instance_builder.hpp"

#include <acorn_logging.hpp>
#include <vulkan/vk_enum_string_helper.h>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Instance_Builder::Instance_Builder()
        {
            m_instance_ptr = new Instance();

            m_application_info = {};
            m_application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            m_application_info.pNext = NULL;
            m_application_info.pApplicationName = "";
            m_application_info.applicationVersion = 0;
            m_application_info.pEngineName = "Cascade";
            m_application_info.engineVersion = 0;
            m_application_info.apiVersion = VK_API_VERSION_1_0;

            m_enabled_layers.clear();
            m_enabled_extensions.clear();
        }

        void Instance_Builder::Ensure_All_Required_Layers_Are_Present()
        {
            LOG_DEBUG << "Graphics (Vulkan): Checking that all instance layers required are present";
            LOG_TRACE << "Graphics (Vulkan): Getting all available instance layers";

            bool all_required_layers_present = true;

            uint32_t available_layer_count;
            std::vector<VkLayerProperties> available_layers;

            VkResult get_available_layer_count_result = vkEnumerateInstanceLayerProperties(&available_layer_count, NULL);
            if (get_available_layer_count_result != VK_SUCCESS)
            {
                LOG_FATAL << "Graphics (Vulkan): Failed to get number of available instance layers with VkResult " << get_available_layer_count_result << " (" << string_VkResult(get_available_layer_count_result) << ")";
                exit(EXIT_FAILURE);
            }
            available_layers.resize(available_layer_count);

            VkResult get_available_layers_result = vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers.data());
            if (get_available_layers_result != VK_SUCCESS)
            {
                LOG_FATAL << "Graphics (Vulkan): Failed to get available instance layers with VkResult " << get_available_layers_result << " (" << string_VkResult(get_available_layers_result) << ")";
                exit(EXIT_FAILURE);
            }

            LOG_TRACE << "Graphics (Vulkan): Ensuring required layers are present";

            for (uint32_t required_layer_index = 0; required_layer_index < m_enabled_layers.size(); required_layer_index++)
            {
                const char* current_required_layer = m_enabled_layers[required_layer_index];

                bool found_layer = false;
                for (uint32_t available_layer_index = 0; available_layer_index < available_layers.size(); available_layer_index++)
                {
                    if (std::string(current_required_layer) == available_layers[available_layer_index].layerName)
                    {
                        found_layer = true;
                    }
                }

                if (found_layer)
                {
                    LOG_TRACE << "Graphics (Vulkan): Layer '" << current_required_layer << "' - Found";
                }
                else
                {
                    LOG_ERROR << "Graphics (Vulkan): Layer '" << current_required_layer << "' - Missing";
                    all_required_layers_present = false;
                }
            }

            if (!all_required_layers_present)
            {
                LOG_FATAL << "Graphics (Vulkan): One or more required instance layers are missing";
                exit(EXIT_FAILURE);
            }
            else
            {
                LOG_TRACE << "Graphics (Vulkan): All required instance layers were found";
            }
        }

        void Instance_Builder::Ensure_All_Required_Extensions_Are_Present()
        {
            LOG_DEBUG << "Graphics (Vulkan): Checking that all instance extensions required are present";
            LOG_TRACE << "Graphics (Vulkan): Getting all available instance extensions";

            bool all_required_extensions_present = true;

            std::vector<VkExtensionProperties> available_extensions;

            for (uint32_t layer_index = 0; layer_index <= m_enabled_layers.size(); layer_index++)
            {
                const char* layer_name = (layer_index == m_enabled_layers.size()) ? NULL : m_enabled_layers[layer_index];

                uint32_t available_extensions_in_layer_count;
                VkResult get_extension_count_in_layer_result = vkEnumerateInstanceExtensionProperties(layer_name, &available_extensions_in_layer_count, NULL);
                if (get_extension_count_in_layer_result != VK_SUCCESS)
                {
                    LOG_FATAL << "Graphics (Vulkan): Failed to get number of available instance extensions in layer '" << layer_name << "' with VkResult " << get_extension_count_in_layer_result << " ("
                              << string_VkResult(get_extension_count_in_layer_result) << ")";
                    exit(EXIT_FAILURE);
                }

                std::vector<VkExtensionProperties> available_extensions_in_layer(available_extensions_in_layer_count);
                VkResult get_extensions_in_layer = vkEnumerateInstanceExtensionProperties(layer_name, &available_extensions_in_layer_count, available_extensions_in_layer.data());
                if (get_extensions_in_layer != VK_SUCCESS)
                {
                    LOG_FATAL << "Graphics (Vulkan): Failed to get available instance extensions in layer '" << layer_name << "' with VkResult " << get_extension_count_in_layer_result << " (" << string_VkResult(get_extension_count_in_layer_result)
                              << ")";
                    exit(EXIT_FAILURE);
                }

                available_extensions.insert(available_extensions.begin(), available_extensions_in_layer.begin(), available_extensions_in_layer.end());
            }

            LOG_TRACE << "Graphics (Vulkan): Ensuring required extensions are present";

            for (uint32_t required_extension_index = 0; required_extension_index < m_enabled_extensions.size(); required_extension_index++)
            {
                const char* current_required_extension = m_enabled_extensions[required_extension_index];

                bool found_extension = false;
                for (uint32_t available_extension_index = 0; available_extension_index < available_extensions.size(); available_extension_index++)
                {
                    if (std::string(current_required_extension) == available_extensions[available_extension_index].extensionName)
                    {
                        found_extension = true;
                    }
                }

                if (found_extension)
                {
                    LOG_TRACE << "Graphics (Vulkan): Extension '" << current_required_extension << "' - Found";
                }
                else
                {
                    LOG_ERROR << "Graphics (Vulkan): Extension '" << current_required_extension << "' - Missing";
                    all_required_extensions_present = false;
                }
            }

            if (!all_required_extensions_present)
            {
                LOG_FATAL << "Graphics (Vulkan): One or more required instance extensions are missing";
                exit(EXIT_FAILURE);
            }
            else
            {
                LOG_TRACE << "Graphics (Vulkan): All required instance extensions were found";
            }
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
            if (m_enabled_layers.size() != 0)
            {
                Ensure_All_Required_Layers_Are_Present();
            }
            if (m_enabled_extensions.size() != 0)
            {
                Ensure_All_Required_Extensions_Are_Present();
            }

            LOG_INFO << "Graphics (Vulkan): Creating instance";

            VkInstanceCreateInfo instance_create_info = {};
            instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instance_create_info.pNext = NULL;
            instance_create_info.flags = 0;
            instance_create_info.pApplicationInfo = &m_application_info;
            instance_create_info.enabledLayerCount = static_cast<uint32_t>(m_enabled_layers.size());
            instance_create_info.ppEnabledLayerNames = m_enabled_layers.data();
            instance_create_info.enabledExtensionCount = static_cast<uint32_t>(m_enabled_extensions.size());
            instance_create_info.ppEnabledExtensionNames = m_enabled_extensions.data();

            VkResult create_instance_result = vkCreateInstance(&instance_create_info, NULL, m_instance_ptr->Get());
            if (create_instance_result != VK_SUCCESS)
            {
                LOG_FATAL << "Graphics (Vulkan): Call to vkCreateInstance failed with VkResult " << create_instance_result << " (" << string_VkResult(create_instance_result) << ")";
                exit(EXIT_FAILURE);
            }

            LOG_TRACE << "Graphics (Vulkan): Finished creating instance";

            return m_instance_ptr;
        }
    }    // namespace Vulkan
}    // namespace Cascade_Graphics