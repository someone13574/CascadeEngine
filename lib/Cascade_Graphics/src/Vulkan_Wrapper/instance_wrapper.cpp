#include "instance_wrapper.hpp"

#include "cascade_logging.hpp"
#include "validation_layer_wrapper.hpp"
#include <cstring>

namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        Instance_Wrapper::Instance_Wrapper(const char* application_name, uint32_t application_version, std::set<const char*> required_instance_extensions) : m_required_instance_extensions(required_instance_extensions)
        {
            LOG_INFO << "Vulkan Backend: Creating instance";

            m_required_instance_extensions.insert(VK_KHR_SURFACE_EXTENSION_NAME);

#if defined CSD_VULKAN_ENABLE_DEBUG_LAYERS
            m_required_instance_extensions.insert(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

#if defined __linux__
            m_required_instance_extensions.insert(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined _WIN32 || defined WIN32
            m_required_instance_extensions.insert(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

            Is_Vulkan_Supported();

            m_application_info = {};
            m_application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            m_application_info.pNext = nullptr;
            m_application_info.pApplicationName = application_name;
            m_application_info.applicationVersion = application_version;
            m_application_info.pEngineName = "Cascade";
            m_application_info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
            m_application_info.apiVersion = VK_API_VERSION_1_3;

            std::vector<const char*> required_extensions(m_required_instance_extensions.begin(), m_required_instance_extensions.end());

            VkInstanceCreateInfo instance_create_info = {};
            instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instance_create_info.flags = 0;
            instance_create_info.pApplicationInfo = &m_application_info;
            instance_create_info.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
            instance_create_info.ppEnabledExtensionNames = required_extensions.data();

#if defined CSD_VULKAN_ENABLE_DEBUG_LAYERS
            Validation_Layer::Check_Validation_Layer_Support(Validation_Layer::Get_Enabled_Validation_Layers());

            std::vector<const char*> enabled_validation_layers = Validation_Layer::Get_Enabled_Validation_Layers();
            instance_create_info.enabledLayerCount = static_cast<uint32_t>(enabled_validation_layers.size());
            instance_create_info.ppEnabledLayerNames = enabled_validation_layers.data();

            VkDebugUtilsMessengerCreateInfoEXT messenger_create_info = Validation_Layer::Generate_Messenger_Create_Info();
            instance_create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&messenger_create_info;
#else
            instance_create_info.enabledLayerCount = 0;
            instance_create_info.ppEnabledLayerNames = nullptr;

            instance_create_info.pNext = nullptr;
#endif

            VkResult instance_creation_result = vkCreateInstance(&instance_create_info, nullptr, &m_instance);
            if (instance_creation_result != VK_SUCCESS)
            {
                LOG_FATAL << "Vulkan Backend: Instance creation failed with VkResult " << instance_creation_result;
                exit(EXIT_FAILURE);
            }

            LOG_TRACE << "Vulkan Backend: Finished creating instance";
        }

        Instance_Wrapper::~Instance_Wrapper()
        {
            LOG_INFO << "Vulkan Backend: Destroying instance";

            vkDestroyInstance(m_instance, nullptr);

            LOG_TRACE << "Vulkan Backend: Finished destroying instance";
        }

        uint32_t Instance_Wrapper::Get_Supported_Extension_Count()
        {
            uint32_t extension_count = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

            LOG_DEBUG << "Vulkan Backend: This device supports " << extension_count << " instance extensions";

            return extension_count;
        }

        bool Instance_Wrapper::Is_Vulkan_Supported()
        {
            LOG_INFO << "Vulkan Backend: Checking this device's Vulkan support";

            std::vector<bool> extensions_satisfied(m_required_instance_extensions.size());

            uint32_t extension_count = Get_Supported_Extension_Count();
            std::vector<VkExtensionProperties> supported_extensions(extension_count);
            vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, supported_extensions.data());

            for (uint32_t i = 0; i < supported_extensions.size(); i++)
            {
                LOG_TRACE << "Vulkan Backend: Instance extension supported " << supported_extensions[i].extensionName;
            }

            bool vulkan_supported = true;
            for (const char* extension : m_required_instance_extensions)
            {
                bool found_extension = false;
                for (uint32_t j = 0; j < supported_extensions.size(); j++)
                {
                    found_extension |= strcmp(extension, supported_extensions[j].extensionName) == 0;
                }

                if (!found_extension)
                {
                    LOG_ERROR << "Vulkan Backend: Missing support for instance extension " << extension;
                    vulkan_supported = false;
                }
            }

            if (vulkan_supported)
            {
                LOG_DEBUG << "Vulkan Backend: This device has all the required instance extensions";
                return true;
            }
            else
            {
                LOG_FATAL << "Vulkan Backend: This device is missing a required instance extension";
                exit(EXIT_FAILURE);
            }
        }

        VkInstance* Instance_Wrapper::Get_Instance()
        {
            return &m_instance;
        }
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics