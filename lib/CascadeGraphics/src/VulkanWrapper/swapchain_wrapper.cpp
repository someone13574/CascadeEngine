#include "swapchain_wrapper.hpp"

#include "../debug_tools.hpp"

namespace CascadeGraphics
{
    namespace Vulkan
    {
        Swapchain::Swapchain(std::shared_ptr<Physical_Device> physical_device_ptr, std::shared_ptr<Surface> surface_ptr) : m_physical_device_ptr(physical_device_ptr), m_surface_ptr(surface_ptr)
        {
            LOG_INFO << "Vulkan: creating swapchain";

            Get_Swapchain_Support();
            Select_Swapchain_Format();
            Select_Present_Mode();

            LOG_TRACE << "Vulkan: finished creating swapchain";
        }

        Swapchain::~Swapchain()
        {
            LOG_INFO << "Vulkan: destroying swapchain";

            LOG_TRACE << "Vulkan: finished destroying swapchain";
        }

        void Swapchain::Get_Swapchain_Support()
        {
            LOG_INFO << "Vulkan: getting swapchain support";

            VALIDATE_VKRESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*(m_physical_device_ptr->Get_Physical_Device()), *(m_surface_ptr->Get_Surface()), &m_surface_capabilities),
                              "Vulkan: failed to get physical device surface capabilities");

            unsigned int surface_format_count;
            VALIDATE_VKRESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(*(m_physical_device_ptr->Get_Physical_Device()), *(m_surface_ptr->Get_Surface()), &surface_format_count, nullptr),
                              "Vulkan: failed to get supported surface formats");

            LOG_TRACE << "Vulkan: found " << surface_format_count << " surface formats";

            if (surface_format_count != 0)
            {
                m_supported_surface_formats.resize(surface_format_count);

                VALIDATE_VKRESULT(
                    vkGetPhysicalDeviceSurfaceFormatsKHR(*(m_physical_device_ptr->Get_Physical_Device()), *(m_surface_ptr->Get_Surface()), &surface_format_count, m_supported_surface_formats.data()),
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
            VALIDATE_VKRESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(*(m_physical_device_ptr->Get_Physical_Device()), *(m_surface_ptr->Get_Surface()), &present_mode_count, nullptr),
                              "Vulkan: failed to get supported present modes");
            LOG_TRACE << "Vulkan: found " << present_mode_count << " present modes";

            if (present_mode_count != 0)
            {
                m_supported_present_modes.resize(present_mode_count);
                VALIDATE_VKRESULT(
                    vkGetPhysicalDeviceSurfacePresentModesKHR(*(m_physical_device_ptr->Get_Physical_Device()), *(m_surface_ptr->Get_Surface()), &present_mode_count, m_supported_present_modes.data()),
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

        bool Swapchain::Is_Swapchain_Adequate(VkPhysicalDevice* physical_device_ptr, std::shared_ptr<Surface> surface_ptr)
        {
            LOG_TRACE << "Vulkan: checking surface support";

            std::vector<VkSurfaceFormatKHR> surface_formats;
            std::vector<VkPresentModeKHR> present_modes;

            unsigned int surface_format_count;
            VALIDATE_VKRESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(*physical_device_ptr, *(surface_ptr->Get_Surface()), &surface_format_count, nullptr),
                              "Vulkan: failed to get supported surface formats");
            LOG_TRACE << "Vulkan: physical device has " << surface_format_count << " surface formats";

            if (surface_format_count != 0)
            {
                surface_formats.resize(surface_format_count);
                VALIDATE_VKRESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(*physical_device_ptr, *(surface_ptr->Get_Surface()), &surface_format_count, surface_formats.data()),
                                  "Vulkan: failed to get supported surface formats");

                for (unsigned int i = 0; i < surface_format_count; i++)
                {
                    LOG_TRACE << "Vulkan: surface format supported: " << surface_formats[i].format;
                }
            }

            unsigned int present_mode_count;
            VALIDATE_VKRESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(*physical_device_ptr, *(surface_ptr->Get_Surface()), &present_mode_count, nullptr),
                              "Vulkan: failed to get supported present modes");
            LOG_TRACE << "Vulkan: physical device has " << present_mode_count << " present modes";

            if (present_mode_count != 0)
            {
                present_modes.resize(present_mode_count);
                VALIDATE_VKRESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(*physical_device_ptr, *(surface_ptr->Get_Surface()), &present_mode_count, present_modes.data()),
                                  "Vulkan: failed to get supported present modes");

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