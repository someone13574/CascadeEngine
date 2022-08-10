#pragma once

#include "vulkan_header.hpp"

#include "device_wrapper.hpp"
#include "physical_device_wrapper.hpp"
#include "queue_manager.hpp"
#include "storage_manager.hpp"
#include "surface_wrapper.hpp"

#include <memory>
#include <vector>

namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        class Swapchain
        {
        private:
            std::shared_ptr<Device> m_logical_device_ptr;
            std::shared_ptr<Physical_Device_Wrapper> m_physical_device_ptr;
            std::shared_ptr<Surface_Wrapper> m_surface_ptr;
            std::shared_ptr<Queue_Manager> m_queue_manager_ptr;

            VkSurfaceCapabilitiesKHR m_surface_capabilities;
            std::vector<VkSurfaceFormatKHR> m_supported_surface_formats;
            std::vector<VkPresentModeKHR> m_supported_present_modes;

            VkSurfaceFormatKHR m_surface_format;
            VkPresentModeKHR m_present_mode;
            VkExtent2D m_swapchain_extent;
            uint32_t m_swapchain_image_count;

            VkSwapchainKHR m_swapchain;
            std::vector<VkImage> m_swapchain_images;
            std::vector<VkImageView> m_swapchain_image_views;

            static const VkImageUsageFlags SWAPCHAIN_USAGE = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

        private:
            void Get_Swapchain_Support();
            void Select_Swapchain_Format();
            void Select_Present_Mode();
            void Select_Swapchain_Extent(uint32_t width, uint32_t height);
            void Select_Swapchain_Image_Count();
            void Create_Swapchain();
            void Get_Swapchain_Images();
            void Create_Swapchain_Image_Views();

        public:
            Swapchain(std::shared_ptr<Device> logical_device_ptr,
                      std::shared_ptr<Physical_Device_Wrapper> physical_device_ptr,
                      std::shared_ptr<Surface_Wrapper> surface_ptr,
                      std::shared_ptr<Queue_Manager> queue_manager_ptr,
                      uint32_t width,
                      uint32_t height);
            ~Swapchain();

        public:
            static bool Is_Swapchain_Adequate(VkPhysicalDevice* physical_device_ptr, std::shared_ptr<Surface_Wrapper> surface_ptr);

            std::vector<Storage_Manager::Image_Resource> Get_Swapchain_Image_Resources();
            VkSwapchainKHR* Get_Swapchain();
            VkImage* Get_Swapchain_Image(uint32_t index);
            uint32_t Get_Swapchain_Image_Count();
            VkExtent2D Get_Swapchain_Extent();
        };
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics