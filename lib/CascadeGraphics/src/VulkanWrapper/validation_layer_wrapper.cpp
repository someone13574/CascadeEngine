#include "validation_layer_wrapper.hpp"

#include "cascade_logging.hpp"

#include <string.h>

namespace CascadeGraphicsDebugging
{
    namespace Vulkan
    {
        Validation_Layer::Validation_Layer(std::shared_ptr<CascadeGraphics::Vulkan::Instance> instance_ptr) : m_instance_ptr(instance_ptr)
        {
#if defined CASCADE_ENABLE_DEBUG_LAYERS
            LOG_INFO << "Creating Vulkan validation layer";

            Check_Validation_Layer_Support(ENABLED_VALIDATION_LAYERS);

            VkDebugUtilsMessengerCreateInfoEXT messenger_create_info = Generate_Messenger_Create_Info();

            VkResult messeneger_creation_result;
            auto create_messenger_function = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(*(m_instance_ptr->Get_Instance()), "vkCreateDebugUtilsMessengerEXT");
            if (create_messenger_function != nullptr)
            {
                messeneger_creation_result = create_messenger_function(*(m_instance_ptr->Get_Instance()), &messenger_create_info, nullptr, &m_debug_messenger);
            }
            else
            {
                messeneger_creation_result = VK_ERROR_EXTENSION_NOT_PRESENT;
            }

            if (messeneger_creation_result != VK_SUCCESS)
            {
                LOG_FATAL << "Vulkan debug messeneger creation failed with VkResult: " << messeneger_creation_result;
                exit(EXIT_FAILURE);
            }

            LOG_TRACE << "Finished creating vulkan validation layer";
#endif
        }

        Validation_Layer::~Validation_Layer()
        {
            LOG_INFO << "Destroying Vulkan validation layer";

            auto destroy_messenger_function = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(*(m_instance_ptr->Get_Instance()), "vkDestroyDebugUtilsMessengerEXT");
            if (destroy_messenger_function != nullptr)
            {
                destroy_messenger_function(*(m_instance_ptr->Get_Instance()), m_debug_messenger, nullptr);
            }
            else
            {
                LOG_FATAL << "Couldn't get vkDestroyDebugUtilsMessengerEXT proc addr";
                exit(EXIT_FAILURE);
            }

            LOG_TRACE << "Finished destroying Vulkan validation layer";
        }

        static VKAPI_ATTR VkBool32 VKAPI_CALL Validation_Layer_Callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                                        VkDebugUtilsMessageTypeFlagsEXT message_type,
                                                                        const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
                                                                        void* p_user_data)
        {
            switch (message_severity)
            {
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                {
                    switch (message_type)
                    {
                        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
                        {
                            LOG_TRACE << "Vulkan validation layer general message: " << p_callback_data->pMessage;
                            break;
                        }
                        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
                        {
                            LOG_TRACE << "Vulkan validation layer specification message: " << p_callback_data->pMessage;
                            break;
                        }
                        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
                        {
                            LOG_TRACE << "Vulkan validation layer performance message: " << p_callback_data->pMessage;
                            break;
                        }
                    }
                    break;
                }
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                {
                    switch (message_type)
                    {
                        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
                        {
                            LOG_INFO << "Vulkan validation layer general message: " << p_callback_data->pMessage;
                            break;
                        }
                        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
                        {
                            LOG_INFO << "Vulkan validation layer specification message: " << p_callback_data->pMessage;
                            break;
                        }
                        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
                        {
                            LOG_INFO << "Vulkan validation layer performance message: " << p_callback_data->pMessage;
                            break;
                        }
                    }
                    break;
                }
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                {
                    switch (message_type)
                    {
                        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
                        {
                            LOG_WARN << "Vulkan validation layer general message: " << p_callback_data->pMessage;
                            break;
                        }
                        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
                        {
                            LOG_WARN << "Vulkan validation layer specification message: " << p_callback_data->pMessage;
                            break;
                        }
                        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
                        {
                            LOG_WARN << "Vulkan validation layer performance message: " << p_callback_data->pMessage;
                            break;
                        }
                    }
                    break;
                }
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                {
                    switch (message_type)
                    {
                        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
                        {
                            LOG_ERROR << "Vulkan validation layer general message: " << p_callback_data->pMessage;
                            break;
                        }
                        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
                        {
                            LOG_ERROR << "Vulkan validation layer specification message: " << p_callback_data->pMessage;
                            break;
                        }
                        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
                        {
                            LOG_ERROR << "Vulkan validation layer performance message: " << p_callback_data->pMessage;
                            break;
                        }
                    }
                    exit(EXIT_FAILURE);
                    break;
                }
                default:
                {
                    break;
                }
            }

            return VK_FALSE;
        }

        bool Validation_Layer::Check_Validation_Layer_Support(std::vector<const char*> layers_to_check)
        {
            unsigned int layer_count;
            vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

            std::vector<VkLayerProperties> supported_layers(layer_count);
            vkEnumerateInstanceLayerProperties(&layer_count, supported_layers.data());

            for (unsigned int i = 0; i < supported_layers.size(); i++)
            {
                LOG_TRACE << "Validation layer supported: " << supported_layers[i].layerName;
            }

            bool requested_layers_satisfied = true;
            for (unsigned int i = 0; i < layers_to_check.size(); i++)
            {
                bool layer_found = false;
                for (unsigned int j = 0; j < supported_layers.size(); j++)
                {
                    layer_found = strcmp(layers_to_check[i], supported_layers[j].layerName) == 0;
                }

                if (!layer_found)
                {
                    LOG_ERROR << "Missing support for requested validation layer: " << layers_to_check[i];
                    requested_layers_satisfied = false;
                }
            }

            if (requested_layers_satisfied)
            {
                LOG_DEBUG << "All requested validation layers satisfied";
                return true;
            }
            else
            {
                LOG_FATAL << "A requested validation layer is not present";
                exit(EXIT_FAILURE);
            }
        }

        VkDebugUtilsMessengerCreateInfoEXT Validation_Layer::Generate_Messenger_Create_Info()
        {
            VkDebugUtilsMessengerCreateInfoEXT messenger_create_info = {};
            messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            messenger_create_info.pNext = NULL;
            messenger_create_info.flags = 0;
            messenger_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                                                    | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            messenger_create_info.messageType
                = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            messenger_create_info.pfnUserCallback = Validation_Layer_Callback;
            messenger_create_info.pUserData = NULL;

            return messenger_create_info;
        }

        std::vector<const char*> Validation_Layer::Get_Enabled_Validation_Layers()
        {
            return {"VK_LAYER_KHRONOS_validation"};
        }
    } // namespace Vulkan

} // namespace CascadeGraphicsDebugging
