#include "instance_wrapper.hpp"

#include "cascade_logging.hpp"

namespace CascadeGraphics
{
    namespace Vulkan
    {
        Instance::Instance(const char* application_name, unsigned int application_version)
        {
            LOG_INFO << "Creating Vulkan instance.";

            m_application_info = {};
            m_application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            m_application_info.pNext = NULL;
            m_application_info.pApplicationName = application_name;
            m_application_info.applicationVersion = application_version;
            m_application_info.pEngineName = "Cascade";
            m_application_info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
            m_application_info.apiVersion = VK_API_VERSION_1_0;

            Get_Required_Instance_Extensions();

            VkInstanceCreateInfo instance_create_info = {};
            instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instance_create_info.pNext = 0; //
            instance_create_info.flags = 0;
            instance_create_info.pApplicationInfo = &m_application_info;
            instance_create_info.enabledLayerCount = 0;                                           //
            instance_create_info.ppEnabledLayerNames = 0;                                         //
            instance_create_info.enabledExtensionCount = m_required_instance_extensions.size();   //
            instance_create_info.ppEnabledExtensionNames = m_required_instance_extensions.data(); //

            VkResult instance_creation_result = vkCreateInstance(&instance_create_info, nullptr, &m_instance);
            if (instance_creation_result != VK_SUCCESS)
            {
                LOG_FATAL << "Vulkan Instance creation failed with VkResult: " << instance_creation_result;
                exit(EXIT_FAILURE);
            }

            LOG_TRACE << "Finished creating Vulkan instance.";
        }

        Instance::~Instance()
        {
            LOG_INFO << "Destroying Vulkan instance";

            LOG_TRACE << "Finished destroying Vulkan instance.";
        }

        void Instance::Get_Required_Instance_Extensions()
        {
            LOG_TRACE << "Getting required instance extensions";

            m_required_instance_extensions.clear();
            m_required_instance_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

#if defined CASCADE_ENABLE_DEBUG_LAYERS
            m_required_instance_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

#if defined __linux__
            m_required_instance_extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined _WIN32 || defined WIN32
            m_required_instance_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
        }
    } // namespace Vulkan
} // namespace CascadeGraphics