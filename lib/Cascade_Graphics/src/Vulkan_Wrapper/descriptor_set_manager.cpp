#include "descriptor_set_manager.hpp"

#include "debug_tools.hpp"
#include <list>


namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        Descriptor_Set_Manager::Descriptor_Set_Manager(std::shared_ptr<Logical_Device_Wrapper> logical_device_wrapper_ptr, std::shared_ptr<Storage_Manager> storage_manager_ptr)
            : m_logical_device_wrapper_ptr(logical_device_wrapper_ptr), m_storage_manager_ptr(storage_manager_ptr)
        {
        }

        Descriptor_Set_Manager::~Descriptor_Set_Manager()
        {
            LOG_INFO << "Vulkan Backend: Cleaning up descriptor set manager";

            for (uint32_t i = 0; i < m_descriptor_sets.size(); i++)
            {
                vkDestroyDescriptorSetLayout(*m_logical_device_wrapper_ptr->Get_Device(), m_descriptor_sets[i].descriptor_set_layout, nullptr);
                vkDestroyDescriptorPool(*m_logical_device_wrapper_ptr->Get_Device(), m_descriptor_sets[i].descriptor_pool, nullptr);
            }

            for (uint32_t i = 0; i < m_samplers.size(); i++)
            {
                vkDestroySampler(*m_logical_device_wrapper_ptr->Get_Device(), m_samplers[i], nullptr);
            }

            LOG_TRACE << "Vulkan Backend: Finished cleaning up descriptor set manager";
        }

        void Descriptor_Set_Manager::Create_Descriptor_Set_Layout(Identifier identifier)
        {
            LOG_INFO << "Vulkan Backend: Creating descriptor set layout for " << identifier.Get_Identifier_String();

            Descriptor_Set_Data* descriptor_set_data_ptr = Get_Descriptor_Set_Data(identifier);
            Storage_Manager::Resource_Grouping* resource_grouping_ptr = m_storage_manager_ptr->Get_Resource_Grouping(descriptor_set_data_ptr->resource_grouping_identifier);

            uint32_t index = 0;
            std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_bindings(resource_grouping_ptr->buffer_identifiers.size() + resource_grouping_ptr->image_identifiers.size());
            for (uint32_t i = 0; i < resource_grouping_ptr->buffer_identifiers.size(); i++, index++)
            {
                descriptor_set_layout_bindings[index] = {};
                descriptor_set_layout_bindings[index].binding = index;
                descriptor_set_layout_bindings[index].descriptorType = m_storage_manager_ptr->Get_Buffer_Resource(resource_grouping_ptr->buffer_identifiers[i])->descriptor_type;
                descriptor_set_layout_bindings[index].descriptorCount = 1;
                descriptor_set_layout_bindings[index].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
                descriptor_set_layout_bindings[index].pImmutableSamplers = nullptr;
            }
            for (uint32_t i = 0; i < resource_grouping_ptr->image_identifiers.size(); i++, index++)
            {
                descriptor_set_layout_bindings[index] = {};
                descriptor_set_layout_bindings[index].binding = index;
                descriptor_set_layout_bindings[index].descriptorType = m_storage_manager_ptr->Get_Image_Resource(resource_grouping_ptr->image_identifiers[i])->descriptor_type;
                descriptor_set_layout_bindings[index].descriptorCount = 1;
                descriptor_set_layout_bindings[index].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
                descriptor_set_layout_bindings[index].pImmutableSamplers = nullptr;
            }

            VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {};
            descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            descriptor_set_layout_create_info.pNext = nullptr;
            descriptor_set_layout_create_info.flags = 0;
            descriptor_set_layout_create_info.bindingCount = static_cast<uint32_t>(descriptor_set_layout_bindings.size());
            descriptor_set_layout_create_info.pBindings = descriptor_set_layout_bindings.data();

            VALIDATE_VKRESULT(vkCreateDescriptorSetLayout(*m_logical_device_wrapper_ptr->Get_Device(), &descriptor_set_layout_create_info, nullptr, &descriptor_set_data_ptr->descriptor_set_layout),
                              "Vulkan Backend: Failed to create descriptor set layout");
        }

        void Descriptor_Set_Manager::Create_Descriptor_Pool(Identifier identifier)
        {
            LOG_INFO << "Vulkan Backend: Creating descriptor pool for " << identifier.Get_Identifier_String();

            Descriptor_Set_Data* descriptor_set_data_ptr = Get_Descriptor_Set_Data(identifier);
            Storage_Manager::Resource_Grouping* resource_grouping_ptr = m_storage_manager_ptr->Get_Resource_Grouping(descriptor_set_data_ptr->resource_grouping_identifier);

            uint32_t index = 0;
            std::vector<VkDescriptorPoolSize> descriptor_pool_sizes(resource_grouping_ptr->buffer_identifiers.size() + resource_grouping_ptr->image_identifiers.size());
            for (uint32_t i = 0; i < resource_grouping_ptr->buffer_identifiers.size(); i++, index++)
            {
                descriptor_pool_sizes[index] = {};
                descriptor_pool_sizes[index].descriptorCount = 1;
                descriptor_pool_sizes[index].type = m_storage_manager_ptr->Get_Buffer_Resource(resource_grouping_ptr->buffer_identifiers[i])->descriptor_type;
            }
            for (uint32_t i = 0; i < resource_grouping_ptr->image_identifiers.size(); i++, index++)
            {
                descriptor_pool_sizes[index] = {};
                descriptor_pool_sizes[index].descriptorCount = 1;
                descriptor_pool_sizes[index].type = m_storage_manager_ptr->Get_Image_Resource(resource_grouping_ptr->image_identifiers[i])->descriptor_type;
            }

            VkDescriptorPoolCreateInfo descriptor_pool_create_info = {};
            descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            descriptor_pool_create_info.pNext = nullptr;
            descriptor_pool_create_info.flags = 0;
            descriptor_pool_create_info.maxSets = 1;
            descriptor_pool_create_info.poolSizeCount = static_cast<uint32_t>(descriptor_pool_sizes.size());
            descriptor_pool_create_info.pPoolSizes = descriptor_pool_sizes.data();

            VALIDATE_VKRESULT(vkCreateDescriptorPool(*m_logical_device_wrapper_ptr->Get_Device(), &descriptor_pool_create_info, nullptr, &descriptor_set_data_ptr->descriptor_pool), "Vulkan Backend: Failed to create descriptor pool");
        }

        void Descriptor_Set_Manager::Allocate_Descriptor_Set(Identifier identifier)
        {
            LOG_INFO << "Vulkan Backend: Allocating descriptor set for " << identifier.Get_Identifier_String();

            Descriptor_Set_Data* descriptor_set_data_ptr = Get_Descriptor_Set_Data(identifier);

            VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {};
            descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            descriptor_set_allocate_info.pNext = nullptr;
            descriptor_set_allocate_info.descriptorPool = descriptor_set_data_ptr->descriptor_pool;
            descriptor_set_allocate_info.descriptorSetCount = 1;
            descriptor_set_allocate_info.pSetLayouts = &descriptor_set_data_ptr->descriptor_set_layout;

            VALIDATE_VKRESULT(vkAllocateDescriptorSets(*m_logical_device_wrapper_ptr->Get_Device(), &descriptor_set_allocate_info, &descriptor_set_data_ptr->descriptor_set), "Vulkan Backend: Failed to allocate descriptor set");
        }

        void Descriptor_Set_Manager::Create_Write_Descriptor_Set(Identifier identifier)
        {
            LOG_INFO << "Vulkan Backend: Creating write descriptor set for " << identifier.Get_Identifier_String();

            Descriptor_Set_Data* descriptor_set_data_ptr = Get_Descriptor_Set_Data(identifier);
            Storage_Manager::Resource_Grouping* resource_grouping_ptr = m_storage_manager_ptr->Get_Resource_Grouping(descriptor_set_data_ptr->resource_grouping_identifier);

            for (uint32_t i = 0; i < resource_grouping_ptr->buffer_identifiers.size(); i++)
            {
                descriptor_set_data_ptr->buffer_descriptor_infos[i] = {};
                descriptor_set_data_ptr->buffer_descriptor_infos[i].buffer = m_storage_manager_ptr->Get_Buffer_Resource(resource_grouping_ptr->buffer_identifiers[i])->buffer;
                descriptor_set_data_ptr->buffer_descriptor_infos[i].offset = 0;
                descriptor_set_data_ptr->buffer_descriptor_infos[i].range = VK_WHOLE_SIZE;

                descriptor_set_data_ptr->write_descriptor_sets[i] = {};
                descriptor_set_data_ptr->write_descriptor_sets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptor_set_data_ptr->write_descriptor_sets[i].pNext = nullptr;
                descriptor_set_data_ptr->write_descriptor_sets[i].dstSet = descriptor_set_data_ptr->descriptor_set;
                descriptor_set_data_ptr->write_descriptor_sets[i].dstBinding = i;
                descriptor_set_data_ptr->write_descriptor_sets[i].dstArrayElement = 0;
                descriptor_set_data_ptr->write_descriptor_sets[i].descriptorCount = 1;
                descriptor_set_data_ptr->write_descriptor_sets[i].descriptorType = m_storage_manager_ptr->Get_Buffer_Resource(resource_grouping_ptr->buffer_identifiers[i])->descriptor_type;
                descriptor_set_data_ptr->write_descriptor_sets[i].pBufferInfo = &descriptor_set_data_ptr->buffer_descriptor_infos[i];
                descriptor_set_data_ptr->write_descriptor_sets[i].pImageInfo = nullptr;
                descriptor_set_data_ptr->write_descriptor_sets[i].pTexelBufferView = nullptr;
            }
            for (uint32_t i = 0; i < resource_grouping_ptr->image_identifiers.size(); i++)
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

                m_samplers.resize(m_samplers.size() + 1);
                VALIDATE_VKRESULT(vkCreateSampler(*m_logical_device_wrapper_ptr->Get_Device(), &sampler_create_info, nullptr, &m_samplers.back()), "Vulkan Backend: Failed to create image sampler");

                descriptor_set_data_ptr->image_descriptor_infos[i] = {};
                descriptor_set_data_ptr->image_descriptor_infos[i].sampler = m_samplers.back();
                descriptor_set_data_ptr->image_descriptor_infos[i].imageView = m_storage_manager_ptr->Get_Image_Resource(resource_grouping_ptr->image_identifiers[i])->image_view;
                descriptor_set_data_ptr->image_descriptor_infos[i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

                descriptor_set_data_ptr->write_descriptor_sets[resource_grouping_ptr->buffer_identifiers.size() + i] = {};
                descriptor_set_data_ptr->write_descriptor_sets[resource_grouping_ptr->buffer_identifiers.size() + i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptor_set_data_ptr->write_descriptor_sets[resource_grouping_ptr->buffer_identifiers.size() + i].pNext = nullptr;
                descriptor_set_data_ptr->write_descriptor_sets[resource_grouping_ptr->buffer_identifiers.size() + i].dstSet = descriptor_set_data_ptr->descriptor_set;
                descriptor_set_data_ptr->write_descriptor_sets[resource_grouping_ptr->buffer_identifiers.size() + i].dstBinding = resource_grouping_ptr->buffer_identifiers.size() + i;
                descriptor_set_data_ptr->write_descriptor_sets[resource_grouping_ptr->buffer_identifiers.size() + i].dstArrayElement = 0;
                descriptor_set_data_ptr->write_descriptor_sets[resource_grouping_ptr->buffer_identifiers.size() + i].descriptorCount = 1;
                descriptor_set_data_ptr->write_descriptor_sets[resource_grouping_ptr->buffer_identifiers.size() + i].descriptorType = m_storage_manager_ptr->Get_Image_Resource(resource_grouping_ptr->image_identifiers[i])->descriptor_type;
                descriptor_set_data_ptr->write_descriptor_sets[resource_grouping_ptr->buffer_identifiers.size() + i].pBufferInfo = nullptr;
                descriptor_set_data_ptr->write_descriptor_sets[resource_grouping_ptr->buffer_identifiers.size() + i].pImageInfo = &descriptor_set_data_ptr->image_descriptor_infos[i];
                descriptor_set_data_ptr->write_descriptor_sets[resource_grouping_ptr->buffer_identifiers.size() + i].pTexelBufferView = nullptr;
            }

            vkUpdateDescriptorSets(*m_logical_device_wrapper_ptr->Get_Device(), static_cast<uint32_t>(descriptor_set_data_ptr->write_descriptor_sets.size()), descriptor_set_data_ptr->write_descriptor_sets.data(), 0, nullptr);
        }

        Identifier Descriptor_Set_Manager::Create_Descriptor_Set(Identifier resource_grouping_identifier)
        {
            LOG_INFO << "Vulkan Backend: Creating descriptor set for resource grouping " << resource_grouping_identifier.Get_Identifier_String();

            Identifier identifier = {};
            identifier.label = resource_grouping_identifier.label;
            identifier.index = 0;

            while (true)
            {
                bool index_in_use = false;

                for (uint32_t i = 0; i < m_descriptor_sets.size(); i++)
                {
                    index_in_use |= m_descriptor_sets[i].identifier == identifier;
                }

                if (!index_in_use)
                {
                    break;
                }
                else
                {
                    identifier.index++;
                }
            }

            Storage_Manager::Resource_Grouping* resource_grouping_ptr = m_storage_manager_ptr->Get_Resource_Grouping(resource_grouping_identifier);
            resource_grouping_ptr->has_descriptor_set = true;

            m_descriptor_sets.resize(m_descriptor_sets.size() + 1);
            m_descriptor_sets.back().identifier = identifier;
            m_descriptor_sets.back().resource_grouping_identifier = resource_grouping_identifier;
            m_descriptor_sets.back().descriptor_set = VK_NULL_HANDLE;
            m_descriptor_sets.back().descriptor_pool = VK_NULL_HANDLE;
            m_descriptor_sets.back().descriptor_set_layout = VK_NULL_HANDLE;
            m_descriptor_sets.back().write_descriptor_sets = std::vector<VkWriteDescriptorSet>(resource_grouping_ptr->buffer_identifiers.size() + resource_grouping_ptr->image_identifiers.size());
            m_descriptor_sets.back().buffer_descriptor_infos = std::vector<VkDescriptorBufferInfo>(resource_grouping_ptr->buffer_identifiers.size());
            m_descriptor_sets.back().image_descriptor_infos = std::vector<VkDescriptorImageInfo>(resource_grouping_ptr->image_identifiers.size());

            Create_Descriptor_Set_Layout(identifier);
            Create_Descriptor_Pool(identifier);
            Allocate_Descriptor_Set(identifier);
            Create_Write_Descriptor_Set(identifier);

            return identifier;
        }

        void Descriptor_Set_Manager::Remove_Descriptor_Set(Identifier identifier)
        {
            LOG_TRACE << "Vulkan Backend: Destroying descriptor set " << identifier.Get_Identifier_String();

            for (uint32_t i = 0; i < m_descriptor_sets.size(); i++)
            {
                if (m_descriptor_sets[i].identifier == identifier)
                {
                    vkDestroyDescriptorSetLayout(*m_logical_device_wrapper_ptr->Get_Device(), m_descriptor_sets[i].descriptor_set_layout, nullptr);
                    vkDestroyDescriptorPool(*m_logical_device_wrapper_ptr->Get_Device(), m_descriptor_sets[i].descriptor_pool, nullptr);

                    m_storage_manager_ptr->Get_Resource_Grouping(m_descriptor_sets[i].resource_grouping_identifier)->has_descriptor_set = false;
                    m_descriptor_sets.erase(m_descriptor_sets.begin() + i);
                    return;
                }
            }

            LOG_ERROR << "Vulkan Backend: The descriptor set " << identifier.Get_Identifier_String() << " does not exist";
            exit(EXIT_FAILURE);
        }

        Descriptor_Set_Manager::Descriptor_Set_Data* Descriptor_Set_Manager::Get_Descriptor_Set_Data(Identifier identifier)
        {
            for (uint32_t i = 0; i < m_descriptor_sets.size(); i++)
            {
                if (m_descriptor_sets[i].identifier == identifier)
                {
                    return &m_descriptor_sets[i];
                }
            }

            LOG_ERROR << "Vulkan Backend: The descriptor set " << identifier.Get_Identifier_String() << " does not exist";
            exit(EXIT_FAILURE);
        }
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics