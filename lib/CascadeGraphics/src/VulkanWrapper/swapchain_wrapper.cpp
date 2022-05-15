#include "swapchain_wrapper.hpp"

#include "../debug_tools.hpp"

#include <algorithm>

namespace CascadeGraphics
{
    namespace Vulkan
    {
        Swapchain::Swapchain(std::shared_ptr<Device> logical_device_ptr,
                             std::shared_ptr<Physical_Device> physical_device_ptr,
                             std::shared_ptr<Surface> surface_ptr,
                             std::shared_ptr<Queue_Manager> queue_manager_ptr,
                             unsigned int width,
                             unsigned int height)
            : m_logical_device_ptr(logical_device_ptr), m_physical_device_ptr(physical_device_ptr), m_surface_ptr(surface_ptr), m_queue_manager_ptr(queue_manager_ptr)
        {
            LOG_INFO << "Vulkan: creating swapchain";

            Get_Swapchain_Support();
            Select_Swapchain_Format();
            Select_Present_Mode();
            Select_Swapchain_Extent(width, height);
            Select_Swapchain_Image_Count();
            Create_Swapchain();
            Get_Swapchain_Images();
            Create_Swapchain_Image_Views();


            LOG_TRACE << "Vulkan: finished creating swapchain";
        }

        Swapchain::~Swapchain()
        {
            LOG_INFO << "Vulkan: destroying swapchain";

            for (unsigned int i = 0; i < m_swapchain_image_count; i++)
            {
                vkDestroyImageView(*(m_logical_device_ptr->Get_Device()), m_swapchain_image_views[i], nullptr);
            }

            vkDestroySwapchainKHR(*(m_logical_device_ptr->Get_Device()), m_swapchain, nullptr);

            LOG_TRACE << "Vulkan: finished destroying swapchain";
        }

        void Swapchain::Get_Swapchain_Support()
        {
            LOG_INFO << "Vulkan: getting swapchain support";

            VALIDATE_VKRESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*(m_physical_device_ptr->Get_Physical_Device()), *(m_surface_ptr->Get_Surface()), &m_surface_capabilities), "Vulkan: failed to get physical device surface capabilities");

            unsigned int surface_format_count;
            VALIDATE_VKRESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(*(m_physical_device_ptr->Get_Physical_Device()), *(m_surface_ptr->Get_Surface()), &surface_format_count, nullptr), "Vulkan: failed to get supported surface formats");

            LOG_TRACE << "Vulkan: found " << surface_format_count << " surface formats";

            if (surface_format_count != 0)
            {
                m_supported_surface_formats.resize(surface_format_count);

                VALIDATE_VKRESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(*(m_physical_device_ptr->Get_Physical_Device()), *(m_surface_ptr->Get_Surface()), &surface_format_count, m_supported_surface_formats.data()),
                                  "Vulkan: failed to get supported surface formats");

                for (unsigned int i = 0; i < surface_format_count; i++)
                {
                    LOG_TRACE << "Vulkan: found surface format " << m_supported_surface_formats[i].format;
                }
            }
            else
            {
                LOG_ERROR << "Vulkan: physical device doesn't support any surface formats";
                exit(EXIT_FAILURE);
            }

            unsigned int present_mode_count;
            VALIDATE_VKRESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(*(m_physical_device_ptr->Get_Physical_Device()), *(m_surface_ptr->Get_Surface()), &present_mode_count, nullptr), "Vulkan: failed to get supported present modes");
            LOG_TRACE << "Vulkan: found " << present_mode_count << " present modes";

            if (present_mode_count != 0)
            {
                m_supported_present_modes.resize(present_mode_count);
                VALIDATE_VKRESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(*(m_physical_device_ptr->Get_Physical_Device()), *(m_surface_ptr->Get_Surface()), &present_mode_count, m_supported_present_modes.data()),
                                  "Vulkan: failed to get supported present modes");

                for (unsigned int i = 0; i < present_mode_count; i++)
                {
                    LOG_TRACE << "Vulkan: found present mode " << m_supported_present_modes[i];
                }
            }
            else
            {
                LOG_ERROR << "Vulkan: physical device doesn't support any present modes";
            }
        }

        void Swapchain::Select_Swapchain_Format()
        {
            LOG_TRACE << "Vulkan: selecting surface format";

            for (unsigned int i = 0; i < m_supported_surface_formats.size(); i++)
            {
                if (m_supported_surface_formats[i].format == VK_FORMAT_B8G8R8_SRGB && m_supported_surface_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                {
                    m_surface_format = m_supported_surface_formats[i];

                    LOG_DEBUG << "Vulkan: selected swapchain format " << m_surface_format.format;
                    LOG_DEBUG << "Vulkan: selected swapchain color space " << m_surface_format.colorSpace;

                    return;
                }
            }

            m_surface_format = m_supported_surface_formats[0];

            LOG_DEBUG << "Vulkan: selected swapchain format " << m_surface_format.format;
            LOG_DEBUG << "Vulkan: selected swapchain color space " << m_surface_format.colorSpace;
        }

        void Swapchain::Select_Present_Mode()
        {
            LOG_TRACE << "Vulkan: selecting present mode";

            for (unsigned int i = 0; i < m_supported_present_modes.size(); i++)
            {
                if (m_supported_present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
                {
                    m_present_mode = m_supported_present_modes[i];

                    LOG_DEBUG << "Vulkan: selected present mode " << m_present_mode;

                    return;
                }
            }

            m_present_mode = m_supported_present_modes[0];

            LOG_DEBUG << "Vulkan: selected present mode " << m_present_mode;
        }

        void Swapchain::Select_Swapchain_Extent(unsigned int width, unsigned int height)
        {
            LOG_TRACE << "Vulkan: selecting swapchain extent";

            m_swapchain_extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

            m_swapchain_extent.width = std::clamp(m_swapchain_extent.width, m_surface_capabilities.minImageExtent.width, m_surface_capabilities.maxImageExtent.width);
            m_swapchain_extent.height = std::clamp(m_swapchain_extent.height, m_surface_capabilities.minImageExtent.height, m_surface_capabilities.maxImageExtent.height);

            LOG_DEBUG << "Vulkan: set swapchain extent to " << m_swapchain_extent.width << "x" << m_swapchain_extent.height;
        }

        void Swapchain::Select_Swapchain_Image_Count()
        {
            LOG_TRACE << "Vulkan: selecting swapchain image count";

            m_swapchain_image_count = m_surface_capabilities.minImageCount + 1;

            if (m_surface_capabilities.maxImageCount != 0 && m_swapchain_image_count > m_surface_capabilities.maxImageCount)
            {
                m_swapchain_image_count = m_surface_capabilities.maxImageCount;
            }

            LOG_DEBUG << "Vulkan: selected swapchain image count of " << m_swapchain_image_count;
        }

        void Swapchain::Create_Swapchain()
        {
            VkSwapchainCreateInfoKHR swapchain_create_info = {};
            swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            swapchain_create_info.pNext = nullptr;
            swapchain_create_info.flags = 0;
            swapchain_create_info.surface = *(m_surface_ptr->Get_Surface());
            swapchain_create_info.minImageCount = m_swapchain_image_count;
            swapchain_create_info.imageFormat = m_surface_format.format;
            swapchain_create_info.imageColorSpace = m_surface_format.colorSpace;
            swapchain_create_info.imageExtent = m_swapchain_extent;
            swapchain_create_info.imageArrayLayers = 1;
            swapchain_create_info.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            swapchain_create_info.preTransform = m_surface_capabilities.currentTransform;
            swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            swapchain_create_info.presentMode = m_present_mode;
            swapchain_create_info.clipped = VK_TRUE;
            swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

            if (m_queue_manager_ptr->Get_Queue_Family_Indices().m_present_index.value() == m_queue_manager_ptr->Get_Queue_Family_Indices().m_transfer_index.value())
            {
                LOG_TRACE << "Vulkan: swapchain being created with VK_SHARING_MODE_CONCURRENT";

                unsigned int queue_family_indices[] = {m_queue_manager_ptr->Get_Queue_Family_Indices().m_present_index.value(), m_queue_manager_ptr->Get_Queue_Family_Indices().m_transfer_index.value()};

                swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                swapchain_create_info.queueFamilyIndexCount = 2;
                swapchain_create_info.pQueueFamilyIndices = queue_family_indices;
            }
            else
            {
                LOG_TRACE << "Vulkan: swapchain being created with VK_SHARING_MODE_EXCLUSIVE";

                swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                swapchain_create_info.queueFamilyIndexCount = 0;
                swapchain_create_info.pQueueFamilyIndices = nullptr;
            }

            VALIDATE_VKRESULT(vkCreateSwapchainKHR(*(m_logical_device_ptr->Get_Device()), &swapchain_create_info, nullptr, &m_swapchain), "Vulkan: failed to create swapchain");
        }

        void Swapchain::Get_Swapchain_Images()
        {
            LOG_TRACE << "Vulkan: getting swapchain images";

            VALIDATE_VKRESULT(vkGetSwapchainImagesKHR(*(m_logical_device_ptr->Get_Device()), m_swapchain, &m_swapchain_image_count, nullptr), "Vulkan: failed to get swapchain image count");
            m_swapchain_images.resize(m_swapchain_image_count);
            VALIDATE_VKRESULT(vkGetSwapchainImagesKHR(*(m_logical_device_ptr->Get_Device()), m_swapchain, &m_swapchain_image_count, m_swapchain_images.data()), "Vulkan: failed to get swapchain images");
        }

        void Swapchain::Create_Swapchain_Image_Views()
        {
            LOG_TRACE << "Vulkan: creating swapchain image views";

            m_swapchain_image_views.resize(m_swapchain_image_count);

            for (unsigned int i = 0; i < m_swapchain_image_count; i++)
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

                VALIDATE_VKRESULT(vkCreateImageView(*(m_logical_device_ptr->Get_Device()), &swapchain_image_view_create_info, nullptr, &m_swapchain_image_views[i]), "Vulkan: failed to create swapchain image view");
            }
        }

        bool Swapchain::Is_Swapchain_Adequate(VkPhysicalDevice* physical_device_ptr, std::shared_ptr<Surface> surface_ptr)
        {
            LOG_TRACE << "Vulkan: checking surface support";

            std::vector<VkSurfaceFormatKHR> surface_formats;
            std::vector<VkPresentModeKHR> present_modes;

            unsigned int surface_format_count;
            VALIDATE_VKRESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(*physical_device_ptr, *(surface_ptr->Get_Surface()), &surface_format_count, nullptr), "Vulkan: failed to get supported surface formats");
            LOG_TRACE << "Vulkan: physical device has " << surface_format_count << " surface formats";

            if (surface_format_count != 0)
            {
                surface_formats.resize(surface_format_count);
                VALIDATE_VKRESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(*physical_device_ptr, *(surface_ptr->Get_Surface()), &surface_format_count, surface_formats.data()), "Vulkan: failed to get supported surface formats");

                for (unsigned int i = 0; i < surface_format_count; i++)
                {
                    LOG_TRACE << "Vulkan: surface format supported: " << surface_formats[i].format;
                }
            }

            unsigned int present_mode_count;
            VALIDATE_VKRESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(*physical_device_ptr, *(surface_ptr->Get_Surface()), &present_mode_count, nullptr), "Vulkan: failed to get supported present modes");
            LOG_TRACE << "Vulkan: physical device has " << present_mode_count << " present modes";

            if (present_mode_count != 0)
            {
                present_modes.resize(present_mode_count);
                VALIDATE_VKRESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(*physical_device_ptr, *(surface_ptr->Get_Surface()), &present_mode_count, present_modes.data()), "Vulkan: failed to get supported present modes");

                for (unsigned int i = 0; i < present_mode_count; i++)
                {
                    LOG_TRACE << "Vulkan: present mode supported: " << present_modes[i];
                }
            }

            if (surface_format_count == 0 || present_mode_count == 0)
            {
                return false;
            }

            return true;
        }
    } // namespace Vulkan
} // namespace CascadeGraphics