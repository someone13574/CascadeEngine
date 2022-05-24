#include "instance_wrapper.hpp"

#include "cascade_logging.hpp"

#include "validation_layer_wrapper.hpp"

#include <cstring>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        unsigned int Get_Supported_Extension_Count()
        {
            unsigned int extension_count = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

            LOG_DEBUG << "Vulkan: this device supports " << extension_count << " extensions";

            return extension_count;
        }

        bool Is_Vulkan_Supported()
        {
            LOG_INFO << "Vulkan: checking Vulkan support";

            std::vector<const char*> required_extensions = Cascade_Graphics::Vulkan::Instance::Get_Required_Instance_Extensions();
            std::vector<bool> extensions_satisfied(required_extensions.size());

            unsigned int extension_count = Get_Supported_Extension_Count();
            std::vector<VkExtensionProperties> supported_extensions(extension_count);
            vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, supported_extensions.data());

            for (unsigned int i = 0; i < supported_extensions.size(); i++)
            {
                LOG_TRACE << "Vulkan: extension supported " << supported_extensions[i].extensionName;
            }

            bool vulkan_supported = true;
            for (unsigned int i = 0; i < required_extensions.size(); i++)
            {
                bool found_extension = false;
                for (unsigned int j = 0; j < supported_extensions.size(); j++)
                {
                    found_extension |= strcmp(required_extensions[i], supported_extensions[j].extensionName) == 0;
                }

                if (!found_extension)
                {
                    LOG_ERROR << "Vulkan: missing support for extension " << required_extensions[i];
                    vulkan_supported = false;
                }
            }

            if (vulkan_supported)
            {
                LOG_DEBUG << "Vulkan: this device has all the required extensions";
                return true;
            }
            else
            {
                LOG_FATAL << "Vulkan: this device is missing a required extension";
                exit(EXIT_FAILURE);
            }
        }

        Instance::Instance(const char* application_name, unsigned int application_version)
        {
            LOG_INFO << "Vulkan: creating instance";

            Is_Vulkan_Supported();

            m_application_info = {};
            m_application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            m_application_info.pNext = nullptr;
            m_application_info.pApplicationName = application_name;
            m_application_info.applicationVersion = application_version;
            m_application_info.pEngineName = "Cascade";
            m_application_info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
            m_application_info.apiVersion = VK_API_VERSION_1_3;

            m_required_instance_extensions = Get_Required_Instance_Extensions();

            VkInstanceCreateInfo instance_create_info = {};
            instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instance_create_info.flags = 0;
            instance_create_info.pApplicationInfo = &m_application_info;
            instance_create_info.enabledExtensionCount = static_cast<uint32_t>(m_required_instance_extensions.size());
            instance_create_info.ppEnabledExtensionNames = m_required_instance_extensions.data();

#if defined CASCADE_ENABLE_DEBUG_LAYERS
            Cascade_GraphicsDebugging::Vulkan::Validation_Layer::Check_Validation_Layer_Support(Cascade_GraphicsDebugging::Vulkan::Validation_Layer::Get_Enabled_Validation_Layers());

            std::vector<const char*> enabled_validation_layers = Cascade_GraphicsDebugging::Vulkan::Validation_Layer::Get_Enabled_Validation_Layers();
            instance_create_info.enabledLayerCount = static_cast<uint32_t>(enabled_validation_layers.size());
            instance_create_info.ppEnabledLayerNames = enabled_validation_layers.data();

            VkDebugUtilsMessengerCreateInfoEXT messenger_create_info = Cascade_GraphicsDebugging::Vulkan::Validation_Layer::Generate_Messenger_Create_Info();
            instance_create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&messenger_create_info;
#else
            instance_create_info.enabledLayerCount = 0;
            instance_create_info.ppEnabledLayerNames = nullptr;

            instance_create_info.pNext = nullptr;
#endif

            VkResult instance_creation_result = vkCreateInstance(&instance_create_info, nullptr, &m_instance);
            if (instance_creation_result != VK_SUCCESS)
            {
                LOG_FATAL << "Vulkan: instance creation failed with VkResult " << instance_creation_result;
                exit(EXIT_FAILURE);
            }

            LOG_TRACE << "Vulkan: finished creating instance";
        }

        Instance::~Instance()
        {
            LOG_INFO << "Vulkan: destroying instance";

            vkDestroyInstance(m_instance, nullptr);

            LOG_TRACE << "Vulkan: finished destroying instance";
        }

        std::vector<const char*> Instance::Get_Required_Instance_Extensions()
        {
            LOG_TRACE << "Vulkan: getting required instance extensions";

            std::vector<const char*> required_instance_extensions;
            required_instance_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

#if defined CSD_ENABLE_DEBUG_LAYERS
            required_instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

#if defined __linux__
            required_instance_extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined _WIN32 || defined WIN32
            required_instance_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
            return required_instance_extensions;
        }

        VkInstance* Instance::Get_Instance()
        {
            return &m_instance;
        }
    } // namespace Vulkan
} // namespace Cascade_Graphics