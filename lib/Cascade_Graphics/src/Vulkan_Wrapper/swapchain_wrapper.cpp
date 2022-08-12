#include "swapchain_wrapper.hpp"

#include "debug_tools.hpp"
#include <algorithm>


namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        Swapchain_Wrapper::Swapchain_Wrapper(std::shared_ptr<Logical_Device_Wrapper> logical_device_wrapper_ptr,
                                             std::shared_ptr<Physical_Device_Wrapper> physical_device_wrapper_ptr,
                                             std::shared_ptr<Surface_Wrapper> surface_wrapper_ptr,
                                             std::shared_ptr<Queue_Manager> queue_manager_ptr,
                                             uint32_t width,
                                             uint32_t height)
            : m_logical_device_wrapper_ptr(logical_device_wrapper_ptr), m_physical_device_wrapper_ptr(physical_device_wrapper_ptr), m_surface_wrapper_ptr(surface_wrapper_ptr), m_queue_manager_ptr(queue_manager_ptr)
        {
            LOG_INFO << "Vulkan Backend: Creating swapchain";

            Get_Swapchain_Support_Details();

            Select_Swapchain_Image_Format();
            Select_Present_Mode();
            Select_Swapchain_Image_Extent(width, height);
            Select_Swapchain_Image_Count();

            Create_Swapchain();
            Create_Swapchain_Images();
            Create_Swapchain_Image_Views();

            LOG_TRACE << "Vulkan Backend: Finished creating swapchain";
        }

        Swapchain_Wrapper::~Swapchain_Wrapper()
        {
            LOG_INFO << "Vulkan Backend: Destroying swapchain";

            for (uint32_t i = 0; i < m_swapchain_image_count; i++)
            {
                vkDestroyImageView(*m_logical_device_wrapper_ptr->Get_Device(), m_swapchain_image_views[i], nullptr);
            }

            vkDestroySwapchainKHR(*m_logical_device_wrapper_ptr->Get_Device(), m_swapchain, nullptr);

            LOG_TRACE << "Vulkan Backend: Finished destroying swapchain";
        }

        void Swapchain_Wrapper::Get_Swapchain_Support_Details()
        {
            LOG_INFO << "Vulkan Backend: Getting swapchain support details";

            uint32_t surface_format_count;
            uint32_t present_mode_count;

            VALIDATE_VKRESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*m_physical_device_wrapper_ptr->Get_Physical_Device(), *m_surface_wrapper_ptr->Get_Surface(), &m_surface_capabilities),
                              "Vulkan Backend: Failed to get physical device surface capabilities");
            VALIDATE_VKRESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(*m_physical_device_wrapper_ptr->Get_Physical_Device(), *m_surface_wrapper_ptr->Get_Surface(), &surface_format_count, nullptr),
                              "Vulkan Backend: Failed to get supported surface formats");
            VALIDATE_VKRESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(*m_physical_device_wrapper_ptr->Get_Physical_Device(), *m_surface_wrapper_ptr->Get_Surface(), &present_mode_count, nullptr),
                              "Vulkan Backend: Failed to get supported present modes");

            LOG_TRACE << "Vulkan Backend: Found " << surface_format_count << " surface formats";
            LOG_TRACE << "Vulkan Backend: Found " << present_mode_count << " present modes";

            if (surface_format_count == 0)
            {
                LOG_ERROR << "Vulkan Backend: Physical device doesn't support any surface formats";
                exit(EXIT_FAILURE);
            }
            else if (present_mode_count == 0)
            {
                LOG_ERROR << "Vulkan Backend: Physical device doesn't support any present modes";
                exit(EXIT_FAILURE);
            }

            m_supported_surface_formats.resize(surface_format_count);
            m_supported_present_modes.resize(present_mode_count);

            VALIDATE_VKRESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(*m_physical_device_wrapper_ptr->Get_Physical_Device(), *m_surface_wrapper_ptr->Get_Surface(), &surface_format_count, m_supported_surface_formats.data()),
                              "Vulkan Backend: Failed to get supported surface formats");
            VALIDATE_VKRESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(*m_physical_device_wrapper_ptr->Get_Physical_Device(), *m_surface_wrapper_ptr->Get_Surface(), &present_mode_count, m_supported_present_modes.data()),
                              "Vulkan Backend: Failed to get supported present modes");

            for (uint32_t i = 0; i < surface_format_count; i++)
            {
                LOG_TRACE << "Vulkan Backend: Found surface format " << m_supported_surface_formats[i].format;
            }
            for (uint32_t i = 0; i < present_mode_count; i++)
            {
                LOG_TRACE << "Vulkan Backend: Found present mode " << m_supported_present_modes[i];
            }
        }

        void Swapchain_Wrapper::Select_Swapchain_Image_Format()
        {
            LOG_TRACE << "Vulkan Backend: Selecting swapchain image format";

            for (uint32_t i = 0; i < m_supported_surface_formats.size(); i++)
            {
                if (m_supported_surface_formats[i].format == VK_FORMAT_B8G8R8_SRGB && m_supported_surface_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                {
                    m_surface_format = m_supported_surface_formats[i];

                    LOG_DEBUG << "Vulkan Backend: Selected swapchain image format " << m_surface_format.format;
                    LOG_DEBUG << "Vulkan Backend: Selected swapchain color space " << m_surface_format.colorSpace;

                    return;
                }
            }

            m_surface_format = m_supported_surface_formats[0];

            LOG_DEBUG << "Vulkan Backend: Selected swapchain image format " << m_surface_format.format;
            LOG_DEBUG << "Vulkan Backend: Selected swapchain color space " << m_surface_format.colorSpace;
        }

        void Swapchain_Wrapper::Select_Present_Mode()
        {
            LOG_TRACE << "Vulkan Backend: Selecting present mode";

            for (uint32_t i = 0; i < m_supported_present_modes.size(); i++)
            {
                if (m_supported_present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
                {
                    m_present_mode = m_supported_present_modes[i];

                    LOG_DEBUG << "Vulkan Backend: Selected present mode " << m_present_mode;

                    return;
                }
            }

            m_present_mode = m_supported_present_modes[0];

            LOG_DEBUG << "Vulkan Backend: Selected present mode " << m_present_mode;
        }

        void Swapchain_Wrapper::Select_Swapchain_Image_Extent(uint32_t width, uint32_t height)
        {
            LOG_TRACE << "Vulkan Backend: Selecting swapchain image extent";

            m_swapchain_extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

            m_swapchain_extent.width = std::clamp(m_swapchain_extent.width, m_surface_capabilities.minImageExtent.width, m_surface_capabilities.maxImageExtent.width);
            m_swapchain_extent.height = std::clamp(m_swapchain_extent.height, m_surface_capabilities.minImageExtent.height, m_surface_capabilities.maxImageExtent.height);

            LOG_DEBUG << "Vulkan Backend: Set swapchain image extent to " << m_swapchain_extent.width << "x" << m_swapchain_extent.height;
        }

        void Swapchain_Wrapper::Select_Swapchain_Image_Count()
        {
            LOG_TRACE << "Vulkan Backend: Selecting swapchain image count";

            m_swapchain_image_count = m_surface_capabilities.minImageCount + 1;

            if (m_surface_capabilities.maxImageCount != 0 && m_swapchain_image_count > m_surface_capabilities.maxImageCount)
            {
                m_swapchain_image_count = m_surface_capabilities.maxImageCount;
            }

            LOG_DEBUG << "Vulkan Backend: Selected swapchain image count of " << m_swapchain_image_count;
        }

        void Swapchain_Wrapper::Create_Swapchain()
        {
            LOG_INFO << "Vulkan Backend: Creating swapchain";

            std::vector<uint32_t> unique_queues = m_queue_manager_ptr->Get_Unique_Queue_Families(Queue_Manager::Queue_Types::TRANSFER_QUEUE | Queue_Manager::Queue_Types::PRESENT_QUEUE);

            VkSwapchainCreateInfoKHR swapchain_create_info = {};
            swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            swapchain_create_info.pNext = nullptr;
            swapchain_create_info.flags = 0;
            swapchain_create_info.surface = *m_surface_wrapper_ptr->Get_Surface();
            swapchain_create_info.minImageCount = m_swapchain_image_count;
            swapchain_create_info.imageFormat = m_surface_format.format;
            swapchain_create_info.imageColorSpace = m_surface_format.colorSpace;
            swapchain_create_info.imageExtent = m_swapchain_extent;
            swapchain_create_info.imageArrayLayers = 1;
            swapchain_create_info.imageUsage = SWAPCHAIN_USAGE;
            swapchain_create_info.imageSharingMode = (unique_queues.size() == 1) ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
            swapchain_create_info.queueFamilyIndexCount = static_cast<uint32_t>(unique_queues.size());
            swapchain_create_info.pQueueFamilyIndices = unique_queues.data();
            swapchain_create_info.preTransform = m_surface_capabilities.currentTransform;
            swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            swapchain_create_info.presentMode = m_present_mode;
            swapchain_create_info.clipped = VK_TRUE;
            swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

            VALIDATE_VKRESULT(vkCreateSwapchainKHR(*m_logical_device_wrapper_ptr->Get_Device(), &swapchain_create_info, nullptr, &m_swapchain), "Vulkan Backend: Failed to create swapchain");
        }

        void Swapchain_Wrapper::Create_Swapchain_Images()
        {
            LOG_TRACE << "Vulkan Backend: Getting image handles from swapchain";

            VALIDATE_VKRESULT(vkGetSwapchainImagesKHR(*m_logical_device_wrapper_ptr->Get_Device(), m_swapchain, &m_swapchain_image_count, nullptr), "Vulkan Backend: Failed to get swapchain image count");

            m_swapchain_images.resize(m_swapchain_image_count);
            VALIDATE_VKRESULT(vkGetSwapchainImagesKHR(*m_logical_device_wrapper_ptr->Get_Device(), m_swapchain, &m_swapchain_image_count, m_swapchain_images.data()), "Vulkan Backend: Failed to get image handles from swapchain");
        }

        void Swapchain_Wrapper::Create_Swapchain_Image_Views()
        {
            LOG_TRACE << "Vulkan Backend: Creating swapchain image views";

            m_swapchain_image_views.resize(m_swapchain_image_count);

            for (uint32_t i = 0; i < m_swapchain_image_count; i++)
            {
                VkImageViewCreateInfo swapchain_image_view_create_info = {};
                swapchain_image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                swapchain_image_view_create_info.pNext = nullptr;
                swapchain_image_view_create_info.flags = 0;
                swapchain_image_view_create_info.image = m_swapchain_images[i];
                swapchain_image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
                swapchain_image_view_create_info.format = m_surface_format.format;
                swapchain_image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
                swapchain_image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
                swapchain_image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
                swapchain_image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
                swapchain_image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                swapchain_image_view_create_info.subresourceRange.baseMipLevel = 0;
                swapchain_image_view_create_info.subresourceRange.levelCount = 1;
                swapchain_image_view_create_info.subresourceRange.baseArrayLayer = 0;
                swapchain_image_view_create_info.subresourceRange.layerCount = 1;

                VALIDATE_VKRESULT(vkCreateImageView(*m_logical_device_wrapper_ptr->Get_Device(), &swapchain_image_view_create_info, nullptr, &m_swapchain_image_views[i]), "Vulkan Backend: Failed to create swapchain image view");
            }
        }

        std::vector<Storage_Manager::Image_Resource> Swapchain_Wrapper::Get_Swapchain_Image_Resources()
        {
            LOG_TRACE << "Vulkan Backend: Making image resources for the swapchain";

            std::vector<Storage_Manager::Image_Resource> swapchain_image_resources(m_swapchain_image_count);

            for (uint32_t i = 0; i < m_swapchain_image_count; i++)
            {
                swapchain_image_resources[i] = {};
                swapchain_image_resources[i].identifier = {"swapchain", 0};
                swapchain_image_resources[i].is_swapchain_image = true;
                swapchain_image_resources[i].image_format = m_surface_format.format;
                swapchain_image_resources[i].image_usage = SWAPCHAIN_USAGE;
                swapchain_image_resources[i].descriptor_type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                swapchain_image_resources[i].image_size = m_swapchain_extent;
                swapchain_image_resources[i].resource_queue_mask = Queue_Manager::Queue_Types::COMPUTE_QUEUE | Queue_Manager::Queue_Types::PRESENT_QUEUE | Queue_Manager::Queue_Types::TRANSFER_QUEUE;
                swapchain_image_resources[i].image = m_swapchain_images[i];
                swapchain_image_resources[i].image_view = m_swapchain_image_views[i];
                swapchain_image_resources[i].device_memory = VK_NULL_HANDLE;
                swapchain_image_resources[i].memory_type_index = 0;
            }

            return swapchain_image_resources;
        }

        VkSwapchainKHR* Swapchain_Wrapper::Get_Swapchain()
        {
            return &m_swapchain;
        }

        VkImage* Swapchain_Wrapper::Get_Swapchain_Image(uint32_t index)
        {
            return &m_swapchain_images[index];
        }

        uint32_t Swapchain_Wrapper::Get_Swapchain_Image_Count()
        {
            return m_swapchain_image_count;
        }

        VkExtent2D Swapchain_Wrapper::Get_Swapchain_Extent()
        {
            return m_swapchain_extent;
        }
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics