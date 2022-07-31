#include "descriptor_set_manager.hpp"

#include "debug_tools.hpp"
#include <list>


namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Descriptor_Set_Manager::Descriptor_Set_Manager(std::shared_ptr<Device> logical_device_ptr, std::shared_ptr<Storage_Manager> storage_manager_ptr) : m_logical_device_ptr(logical_device_ptr), m_storage_manager_ptr(storage_manager_ptr)
        {
        }

        Descriptor_Set_Manager::~Descriptor_Set_Manager()
        {
            LOG_INFO << "Vulkan: Cleaning up descriptor set manager";

            for (unsigned int i = 0; i < m_descriptor_sets.size(); i++)
            {
                vkDestroyDescriptorSetLayout(*m_logical_device_ptr->Get_Device(), m_descriptor_sets[i].descriptor_set_layout, nullptr);
                vkDestroyDescriptorPool(*m_logical_device_ptr->Get_Device(), m_descriptor_sets[i].descriptor_pool, nullptr);
            }

            LOG_TRACE << "Vulkan: Finished cleaning up descriptor set manager";
        }

        void Descriptor_Set_Manager::Create_Descriptor_Set_Layout(std::string label)
        {
            LOG_INFO << "Vulkan: Creating descriptor set layout for '" << label << "'";

            Descriptor_Set_Data* descriptor_set_data_ptr = Get_Descriptor_Set_Data(label);
            Storage_Manager::Resource_Grouping* resource_grouping_ptr = m_storage_manager_ptr->Get_Resource_Grouping(label);

            std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_bindings(resource_grouping_ptr->resource_ids.size());
            for (unsigned int i = 0; i < resource_grouping_ptr->resource_ids.size(); i++)
            {
                descriptor_set_layout_bindings[i] = {};
                descriptor_set_layout_bindings[i].binding = i;
                descriptor_set_layout_bindings[i].descriptorType = (resource_grouping_ptr->resource_ids[i].resource_type == Storage_Manager::Resource_ID::BUFFER_RESOURCE) ?
                                                                       m_storage_manager_ptr->Get_Buffer_Resource(resource_grouping_ptr->resource_ids[i])->descriptor_type :
                                                                       m_storage_manager_ptr->Get_Image_Resource(resource_grouping_ptr->resource_ids[i])->descriptor_type;
                descriptor_set_layout_bindings[i].descriptorCount = 1;
                descriptor_set_layout_bindings[i].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
                descriptor_set_layout_bindings[i].pImmutableSamplers = nullptr;
            }

            VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {};
            descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            descriptor_set_layout_create_info.pNext = nullptr;
            descriptor_set_layout_create_info.flags = 0;
            descriptor_set_layout_create_info.bindingCount = descriptor_set_layout_bindings.size();
            descriptor_set_layout_create_info.pBindings = descriptor_set_layout_bindings.data();

            VALIDATE_VKRESULT(vkCreateDescriptorSetLayout(*m_logical_device_ptr->Get_Device(), &descriptor_set_layout_create_info, nullptr, &descriptor_set_data_ptr->descriptor_set_layout), "Vulkan: Failed to create descriptor set layout");
        }

        void Descriptor_Set_Manager::Create_Descriptor_Pool(std::string label)
        {
            LOG_INFO << "Vulkan: Creating descriptor pool for '" << label << "'";

            Descriptor_Set_Data* descriptor_set_data_ptr = Get_Descriptor_Set_Data(label);
            Storage_Manager::Resource_Grouping* resource_grouping_ptr = m_storage_manager_ptr->Get_Resource_Grouping(label);

            std::vector<VkDescriptorPoolSize> descriptor_pool_sizes;
            for (unsigned int i = 0; i < resource_grouping_ptr->resource_ids.size(); i++)
            {
                descriptor_pool_sizes.resize(descriptor_pool_sizes.size() + 1);

                descriptor_pool_sizes.back() = {};
                descriptor_pool_sizes.back().descriptorCount = 1;
                descriptor_pool_sizes.back().type = (resource_grouping_ptr->resource_ids[i].resource_type == Storage_Manager::Resource_ID::BUFFER_RESOURCE) ?
                                                        m_storage_manager_ptr->Get_Buffer_Resource(resource_grouping_ptr->resource_ids[i])->descriptor_type :
                                                        m_storage_manager_ptr->Get_Image_Resource(resource_grouping_ptr->resource_ids[i])->descriptor_type;
            }

            VkDescriptorPoolCreateInfo descriptor_pool_create_info = {};
            descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            descriptor_pool_create_info.pNext = nullptr;
            descriptor_pool_create_info.flags = 0;
            descriptor_pool_create_info.maxSets = 1;
            descriptor_pool_create_info.poolSizeCount = descriptor_pool_sizes.size();
            descriptor_pool_create_info.pPoolSizes = descriptor_pool_sizes.data();

            VALIDATE_VKRESULT(vkCreateDescriptorPool(*m_logical_device_ptr->Get_Device(), &descriptor_pool_create_info, nullptr, &descriptor_set_data_ptr->descriptor_pool), "Vulkan: Failed to create descriptor pool");
        }

        void Descriptor_Set_Manager::Allocate_Descriptor_Set(std::string label)
        {
            LOG_INFO << "Vulkan: Allocating descriptor set for '" << label << "'";

            Descriptor_Set_Data* descriptor_set_data_ptr = Get_Descriptor_Set_Data(label);

            VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {};
            descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            descriptor_set_allocate_info.pNext = nullptr;
            descriptor_set_allocate_info.descriptorPool = descriptor_set_data_ptr->descriptor_pool;
            descriptor_set_allocate_info.descriptorSetCount = 1;
            descriptor_set_allocate_info.pSetLayouts = &descriptor_set_data_ptr->descriptor_set_layout;

            VALIDATE_VKRESULT(vkAllocateDescriptorSets(*m_logical_device_ptr->Get_Device(), &descriptor_set_allocate_info, &descriptor_set_data_ptr->descriptor_set), "Vulkan: Failed to allocate descriptor set");
        }

        void Descriptor_Set_Manager::Create_Write_Descriptor_Set(std::string label)
        {
            LOG_INFO << "Vulkan: Creating write descriptor set for '" << label << "'";

            Descriptor_Set_Data* descriptor_set_data_ptr = Get_Descriptor_Set_Data(label);
            Storage_Manager::Resource_Grouping* resource_grouping_ptr = m_storage_manager_ptr->Get_Resource_Grouping(label);

            unsigned int set_buffer_descriptor_infos = 0;
            unsigned int set_image_descriptor_infos = 0;
            for (unsigned int i = 0; i < resource_grouping_ptr->resource_ids.size(); i++)
            {
                if (resource_grouping_ptr->resource_ids[i].resource_type == Storage_Manager::Resource_ID::BUFFER_RESOURCE)
                {
                    descriptor_set_data_ptr->buffer_descriptor_infos[set_buffer_descriptor_infos] = {};
                    descriptor_set_data_ptr->buffer_descriptor_infos[set_buffer_descriptor_infos].buffer = m_storage_manager_ptr->Get_Buffer_Resource(resource_grouping_ptr->resource_ids[i])->buffer;
                    descriptor_set_data_ptr->buffer_descriptor_infos[set_buffer_descriptor_infos].offset = 0;
                    descriptor_set_data_ptr->buffer_descriptor_infos[set_buffer_descriptor_infos].range = VK_WHOLE_SIZE;

                    descriptor_set_data_ptr->write_descriptor_sets[i] = {};
                    descriptor_set_data_ptr->write_descriptor_sets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptor_set_data_ptr->write_descriptor_sets[i].pNext = nullptr;
                    descriptor_set_data_ptr->write_descriptor_sets[i].dstSet = descriptor_set_data_ptr->descriptor_set;
                    descriptor_set_data_ptr->write_descriptor_sets[i].dstBinding = i;
                    descriptor_set_data_ptr->write_descriptor_sets[i].dstArrayElement = 0;
                    descriptor_set_data_ptr->write_descriptor_sets[i].descriptorCount = 1;
                    descriptor_set_data_ptr->write_descriptor_sets[i].descriptorType = m_storage_manager_ptr->Get_Buffer_Resource(resource_grouping_ptr->resource_ids[i])->descriptor_type;
                    descriptor_set_data_ptr->write_descriptor_sets[i].pBufferInfo = &descriptor_set_data_ptr->buffer_descriptor_infos[set_buffer_descriptor_infos];
                    descriptor_set_data_ptr->write_descriptor_sets[i].pImageInfo = nullptr;
                    descriptor_set_data_ptr->write_descriptor_sets[i].pTexelBufferView = nullptr;

                    set_buffer_descriptor_infos++;
                }
                else if (resource_grouping_ptr->resource_ids[i].resource_type == Storage_Manager::Resource_ID::IMAGE_RESOURCE)
                {
                    VkSamplerCreateInfo sampler_create_info = {};
                    sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
                    sampler_create_info.pNext = nullptr;
                    sampler_create_info.flags = 0;
                    sampler_create_info.magFilter = VK_FILTER_NEAREST;
                    sampler_create_info.minFilter = VK_FILTER_NEAREST;
                    sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
                    sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
                    sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
                    sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
                    sampler_create_info.mipLodBias = 0;
                    sampler_create_info.anisotropyEnable = VK_FALSE;
                    sampler_create_info.maxAnisotropy = 0;
                    sampler_create_info.compareEnable = VK_FALSE;
                    sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
                    sampler_create_info.minLod = 0;
                    sampler_create_info.maxLod = 0;
                    sampler_create_info.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
                    sampler_create_info.unnormalizedCoordinates = VK_FALSE;

                    VkSampler sampler;
                    VALIDATE_VKRESULT(vkCreateSampler(*m_logical_device_ptr->Get_Device(), &sampler_create_info, nullptr, &sampler), "Vulkan: Failed to create image sampler");

                    descriptor_set_data_ptr->image_descriptor_infos[set_image_descriptor_infos] = {};
                    descriptor_set_data_ptr->image_descriptor_infos[set_image_descriptor_infos].sampler = sampler;
                    descriptor_set_data_ptr->image_descriptor_infos[set_image_descriptor_infos].imageView = m_storage_manager_ptr->Get_Image_Resource(resource_grouping_ptr->resource_ids[i])->image_view;
                    descriptor_set_data_ptr->image_descriptor_infos[set_image_descriptor_infos].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

                    descriptor_set_data_ptr->write_descriptor_sets[i] = {};
                    descriptor_set_data_ptr->write_descriptor_sets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptor_set_data_ptr->write_descriptor_sets[i].pNext = nullptr;
                    descriptor_set_data_ptr->write_descriptor_sets[i].dstSet = descriptor_set_data_ptr->descriptor_set;
                    descriptor_set_data_ptr->write_descriptor_sets[i].dstBinding = i;
                    descriptor_set_data_ptr->write_descriptor_sets[i].dstArrayElement = 0;
                    descriptor_set_data_ptr->write_descriptor_sets[i].descriptorCount = 1;
                    descriptor_set_data_ptr->write_descriptor_sets[i].descriptorType = m_storage_manager_ptr->Get_Image_Resource(resource_grouping_ptr->resource_ids[i])->descriptor_type;
                    descriptor_set_data_ptr->write_descriptor_sets[i].pBufferInfo = nullptr;
                    descriptor_set_data_ptr->write_descriptor_sets[i].pImageInfo = &descriptor_set_data_ptr->image_descriptor_infos[set_image_descriptor_infos];
                    descriptor_set_data_ptr->write_descriptor_sets[i].pTexelBufferView = nullptr;
                }
            }

            vkUpdateDescriptorSets(*m_logical_device_ptr->Get_Device(), descriptor_set_data_ptr->write_descriptor_sets.size(), descriptor_set_data_ptr->write_descriptor_sets.data(), 0, nullptr);
        }

        void Descriptor_Set_Manager::Create_Descriptor_Set(std::string resource_grouping_label)
        {
            LOG_INFO << "Vulkan: Creating descriptor set for resource grouping '" << resource_grouping_label << "'";

            Storage_Manager::Resource_Grouping* resource_grouping_ptr = m_storage_manager_ptr->Get_Resource_Grouping(resource_grouping_label);
            resource_grouping_ptr->has_descriptor_set = true;

            m_descriptor_sets.resize(m_descriptor_sets.size() + 1);
            m_descriptor_sets.back().label = resource_grouping_label;
            m_descriptor_sets.back().descriptor_set = VK_NULL_HANDLE;
            m_descriptor_sets.back().descriptor_pool = VK_NULL_HANDLE;
            m_descriptor_sets.back().descriptor_set_layout = VK_NULL_HANDLE;
            m_descriptor_sets.back().write_descriptor_sets = std::vector<VkWriteDescriptorSet>(resource_grouping_ptr->resource_ids.size());
            m_descriptor_sets.back().buffer_descriptor_infos = std::vector<VkDescriptorBufferInfo>(resource_grouping_ptr->buffer_resource_count);
            m_descriptor_sets.back().image_descriptor_infos = std::vector<VkDescriptorImageInfo>(resource_grouping_ptr->image_resource_count);

            Create_Descriptor_Set_Layout(resource_grouping_label);
            Create_Descriptor_Pool(resource_grouping_label);
            Allocate_Descriptor_Set(resource_grouping_label);
            Create_Write_Descriptor_Set(resource_grouping_label);
        }

        Descriptor_Set_Manager::Descriptor_Set_Data* Descriptor_Set_Manager::Get_Descriptor_Set_Data(std::string label)
        {
            for (unsigned int i = 0; i < m_descriptor_sets.size(); i++)
            {
                if (m_descriptor_sets[i].label == label)
                {
                    return &m_descriptor_sets[i];
                }
            }

            LOG_ERROR << "Vulkan: The descriptor set '" << label << "' does not exist";
            exit(EXIT_FAILURE);
        }
    } // namespace Vulkan
} // namespace Cascade_Graphics