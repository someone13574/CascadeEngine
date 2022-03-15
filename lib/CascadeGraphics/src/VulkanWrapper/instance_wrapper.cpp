#include "instance_wrapper.hpp"

#include "../debug_tools.cpp"
#include "cascade_logging.hpp"
#include "validation_layer_wrapper.hpp"

namespace CascadeGraphics
{
    namespace Vulkan
    {
        Instance::Instance(const char* application_name, unsigned int application_version)
        {
            LOG_INFO << "Vulkan: creating instance";

            CascadeGraphicsDebugging::Vulkan::Is_Vulkan_Supported();

            m_application_info = {};
            m_application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            m_application_info.pNext = NULL;
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
            CascadeGraphicsDebugging::Vulkan::Validation_Layer::Check_Validation_Layer_Support(
                CascadeGraphicsDebugging::Vulkan::Validation_Layer::Get_Enabled_Validation_Layers());

            std::vector<const char*> enabled_validation_layers = CascadeGraphicsDebugging::Vulkan::Validation_Layer::Get_Enabled_Validation_Layers();
            instance_create_info.enabledLayerCount = static_cast<uint32_t>(enabled_validation_layers.size());
            instance_create_info.ppEnabledLayerNames = enabled_validation_layers.data();

            VkDebugUtilsMessengerCreateInfoEXT messenger_create_info = CascadeGraphicsDebugging::Vulkan::Validation_Layer::Generate_Messenger_Create_Info();
            instance_create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&messenger_create_info;
#else
            instance_create_info.enabledLayerCount = 0;
            instance_create_info.ppEnabledLayerNames = NULL;

            instance_create_info.pNext = NULL;
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

#if defined CASCADE_ENABLE_DEBUG_LAYERS
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
} // namespace CascadeGraphics