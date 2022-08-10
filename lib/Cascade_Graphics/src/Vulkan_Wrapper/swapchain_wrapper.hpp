#pragma once

#include "logical_device_wrapper.hpp"
#include "physical_device_wrapper.hpp"
#include "queue_manager.hpp"
#include "storage_manager.hpp"
#include "surface_wrapper.hpp"
#include "vulkan_header.hpp"
#include <memory>
#include <vector>


namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        class Swapchain_Wrapper
        {
        private:
            static const VkImageUsageFlags SWAPCHAIN_USAGE = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

            VkSwapchainKHR m_swapchain;
            std::vector<VkImage> m_swapchain_images;
            std::vector<VkImageView> m_swapchain_image_views;

            VkSurfaceFormatKHR m_surface_format;
            VkPresentModeKHR m_present_mode;
            VkExtent2D m_swapchain_extent;
            uint32_t m_swapchain_image_count;

            VkSurfaceCapabilitiesKHR m_surface_capabilities;
            std::vector<VkSurfaceFormatKHR> m_supported_surface_formats;
            std::vector<VkPresentModeKHR> m_supported_present_modes;

            std::shared_ptr<Logical_Device_Wrapper> m_logical_device_wrapper_ptr;
            std::shared_ptr<Physical_Device_Wrapper> m_physical_device_wrapper_ptr;
            std::shared_ptr<Surface_Wrapper> m_surface_wrapper_ptr;
            std::shared_ptr<Queue_Manager> m_queue_manager_ptr;

        private:
            void Get_Swapchain_Support_Details();

            void Select_Swapchain_Image_Format();
            void Select_Present_Mode();
            void Select_Swapchain_Image_Extent(uint32_t width, uint32_t height);
            void Select_Swapchain_Image_Count();

            void Create_Swapchain();
            void Create_Swapchain_Images();
            void Create_Swapchain_Image_Views();

        public:
            Swapchain_Wrapper(std::shared_ptr<Logical_Device_Wrapper> logical_device_wrapper_ptr,
                              std::shared_ptr<Physical_Device_Wrapper> physical_device_wrapper_ptr,
                              std::shared_ptr<Surface_Wrapper> surface_wrapper_ptr,
                              std::shared_ptr<Queue_Manager> queue_manager_ptr,
                              uint32_t width,
                              uint32_t height);
            ~Swapchain_Wrapper();

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