#include "descriptor_set.hpp"

#include <acorn_logging.hpp>
#include <vulkan/vk_enum_string_helper.h>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Descriptor_Set::Descriptor_Set(Device* device_ptr, std::vector<Descriptor> descriptors) :
            m_device_ptr(device_ptr)
        {
            LOG_DEBUG << "Graphics (Vulkan): Creating descriptor set with " << descriptors.size() << " descriptors";

            // Create descriptor set layout
            std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_bindings(descriptors.size());
            for (uint32_t binding_index = 0; binding_index < descriptor_set_layout_bindings.size(); binding_index++)
            {
                descriptor_set_layout_bindings[binding_index] = {};
                descriptor_set_layout_bindings[binding_index].binding = binding_index;
                descriptor_set_layout_bindings[binding_index].descriptorType = descriptors[binding_index].Get_Descriptor_Type();
                descriptor_set_layout_bindings[binding_index].descriptorCount = 1;
                descriptor_set_layout_bindings[binding_index].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
                descriptor_set_layout_bindings[binding_index].pImmutableSamplers = NULL;
            }

            VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {};
            descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            descriptor_set_layout_create_info.pNext = NULL;
            descriptor_set_layout_create_info.flags = 0;
            descriptor_set_layout_create_info.bindingCount = descriptor_set_layout_bindings.size();
            descriptor_set_layout_create_info.pBindings = descriptor_set_layout_bindings.data();

            VkResult create_descriptor_set_layout_result = vkCreateDescriptorSetLayout(m_device_ptr->Get(), &descriptor_set_layout_create_info, NULL, &m_descriptor_set_layout);
            if (create_descriptor_set_layout_result != VK_SUCCESS)
            {
                LOG_FATAL << "Graphics (Vulkan): Failed to create descriptor set layout with VkResult " << string_VkResult(create_descriptor_set_layout_result);
                exit(EXIT_FAILURE);
            }

            // Create descriptor pool
            std::vector<VkDescriptorPoolSize> descriptor_pool_sizes;
            for (uint32_t binding_index = 0; binding_index < descriptor_pool_sizes.size(); binding_index++)
            {
                descriptor_pool_sizes[binding_index] = {};
                descriptor_pool_sizes[binding_index].type = descriptors[binding_index].Get_Descriptor_Type();
                descriptor_pool_sizes[binding_index].descriptorCount = 1;
            }

            VkDescriptorPoolCreateInfo descriptor_pool_create_info = {};
            descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            descriptor_pool_create_info.pNext = NULL;
            descriptor_pool_create_info.flags = 0;
            descriptor_pool_create_info.maxSets = 1;
            descriptor_pool_create_info.poolSizeCount = descriptor_pool_sizes.size();
            descriptor_pool_create_info.pPoolSizes = descriptor_pool_sizes.data();

            VkResult create_descriptor_pool_result = vkCreateDescriptorPool(m_device_ptr->Get(), &descriptor_pool_create_info, NULL, &m_descriptor_pool);
            if (create_descriptor_pool_result != VK_SUCCESS)
            {
                LOG_FATAL << "Graphics (Vulkan): Failed to create descriptor pool with VkResult " << string_VkResult(create_descriptor_pool_result);
                exit(EXIT_FAILURE);
            }

            // Allocate descriptor set
            VkDescriptorSetAllocateInfo descriptor_set_allocation_info = {};
            descriptor_set_allocation_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            descriptor_set_allocation_info.pNext = NULL;
            descriptor_set_allocation_info.descriptorPool = m_descriptor_pool;
            descriptor_set_allocation_info.descriptorSetCount = 1;
            descriptor_set_allocation_info.pSetLayouts = &m_descriptor_set_layout;

            VkResult allocate_descriptor_set_result = vkAllocateDescriptorSets(m_device_ptr->Get(), &descriptor_set_allocation_info, &m_descriptor_set);
            if (allocate_descriptor_set_result != VK_SUCCESS)
            {
                LOG_FATAL << "Graphics (Vulkan): Failed to allocate descriptor set with VkResult " << string_VkResult(allocate_descriptor_set_result);
                exit(EXIT_FAILURE);
            }

            // Update descriptor sets with information
            std::vector<VkWriteDescriptorSet> write_descriptor_sets(descriptors.size());
            for (uint32_t binding_index = 0; binding_index < descriptors.size(); binding_index++)
            {
                write_descriptor_sets[binding_index] = {};
                write_descriptor_sets[binding_index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write_descriptor_sets[binding_index].pNext = NULL;
                write_descriptor_sets[binding_index].dstSet = m_descriptor_set;
                write_descriptor_sets[binding_index].dstBinding = binding_index;
                write_descriptor_sets[binding_index].dstArrayElement = 0;
                write_descriptor_sets[binding_index].descriptorCount = 1;
                write_descriptor_sets[binding_index].descriptorType = descriptors[binding_index].Get_Descriptor_Type();
                write_descriptor_sets[binding_index].pImageInfo = descriptors[binding_index].Get_Image_Descriptor_Info();
                write_descriptor_sets[binding_index].pBufferInfo = descriptors[binding_index].Get_Buffer_Descriptor_Info();
                write_descriptor_sets[binding_index].pTexelBufferView = nullptr;
            }

            vkUpdateDescriptorSets(m_device_ptr->Get(), write_descriptor_sets.size(), write_descriptor_sets.data(), 0, NULL);
        }

        Descriptor_Set::~Descriptor_Set()
        {
            LOG_TRACE << "Graphics (Vulkan): Destroying descriptor set";

            vkDestroyDescriptorSetLayout(m_device_ptr->Get(), m_descriptor_set_layout, nullptr);
            vkDestroyDescriptorPool(m_device_ptr->Get(), m_descriptor_pool, nullptr);
        }

        VkDescriptorSet* Descriptor_Set::Get()
        {
            return &m_descriptor_set;
        }

        VkDescriptorSetLayout Descriptor_Set::Get_Layout()
        {
            return m_descriptor_set_layout;
        }

    }    // namespace Vulkan
}    // namespace Cascade_Graphics