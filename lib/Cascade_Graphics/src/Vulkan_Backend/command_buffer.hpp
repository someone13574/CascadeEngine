#pragma once

#ifdef __linux__
    #define VK_USE_PLATFORM_XCB_KHR
#elif defined _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "compute_pipeline.hpp"
#include "descriptor_set.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include <tuple>
#include <vector>
#include <vulkan/vulkan.h>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Command_Buffer
        {
        private:
            struct Image_State
            {
                VkAccessFlags access_flags;
                VkImageLayout image_layout;
                VkPipelineStageFlags pipeline_stage_flags;
            };

        private:
            Device* m_device_ptr;
            Compute_Pipeline* m_pipeline_ptr;

            VkCommandPool m_command_pool;
            std::vector<VkCommandBuffer> m_command_buffers;
            std::vector<std::vector<std::tuple<Image*, Image_State, Image_State>>> m_command_buffer_images;

        public:
            Command_Buffer(Device* device_ptr, Compute_Pipeline* compute_pipeline_ptr, uint32_t command_buffer_count, uint32_t queue_family_index);
            ~Command_Buffer();

        public:
            Command_Buffer* Bind_Descriptor_Set(Descriptor_Set* descriptor_set_ptr, int32_t command_buffer_index = -1);
            Command_Buffer* Add_Image(Image* image_ptr, int32_t command_buffer_index = -1);

            Command_Buffer* Image_Memory_Barrier(Image* image_ptr, VkAccessFlags access_flags, VkImageLayout image_layout, VkPipelineStageFlags pipeline_stage_flags);
            Command_Buffer* Dispatch_Compute_Shader(uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z);
            Command_Buffer* Copy_Image(Image* src_image_ptr, Image* dst_image_ptr, uint32_t width, uint32_t height);
            Command_Buffer* Finish_Recording();
        };
    }    // namespace Vulkan
}    // namespace Cascade_Graphics
