#include "resource_grouping_manager.hpp"

#include "debug_tools.hpp"

#include <list>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        Resource_Grouping_Manager::Resource_Grouping_Manager(std::shared_ptr<Device> logical_device_ptr, std::shared_ptr<Storage_Manager> storage_manager_ptr) : m_logical_device_ptr(logical_device_ptr), m_storage_manager_ptr(storage_manager_ptr)
        {
        }

        Resource_Grouping_Manager::~Resource_Grouping_Manager()
        {
            LOG_INFO << "Vulkan: Cleaning up resource grouping manager";

            for (unsigned int i = 0; i < m_descriptor_set_layouts.size(); i++)
            {
                vkDestroyDescriptorSetLayout(*m_logical_device_ptr->Get_Device(), m_descriptor_set_layouts[i], nullptr);
            }

            vkDestroyDescriptorPool(*m_logical_device_ptr->Get_Device(), m_descriptor_pool, nullptr);

            LOG_TRACE << "Vulkan: Finished cleaning up resource grouping manager";
        }

        void Resource_Grouping_Manager::Create_Descriptor_Pool()
        {
            LOG_INFO << "Vulkan: Creating descriptor pool";

            std::vector<VkDescriptorPoolSize> descriptor_pool_sizes;

            for (unsigned int i = 0; i < m_resource_groupings.size(); i++)
            {
                if (m_resource_groupings[i].descriptor_set_info.has_value())
                {
                    for (unsigned int j = 0; j < m_resource_groupings[i].resources.size(); j++)
                    {
                        descriptor_pool_sizes.resize(descriptor_pool_sizes.size() + 1);

                        descriptor_pool_sizes.back().descriptorCount = 1;
                        descriptor_pool_sizes.back().type = (m_resource_groupings[i].resources[j].resource_type == Storage_Manager::Resource_ID::BUFFER_RESOURCE) ?
                                                                m_storage_manager_ptr->Get_Buffer_Resource(m_resource_groupings[i].resources[j])->buffer_type :
                                                                m_storage_manager_ptr->Get_Image_Resource(m_resource_groupings[i].resources[j])->image_type;
                    }
                }
            }

            VkDescriptorPoolCreateInfo descriptor_pool_create_info = {};
            descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            descriptor_pool_create_info.pNext = nullptr;
            descriptor_pool_create_info.flags = 0;
            descriptor_pool_create_info.maxSets = m_descriptor_sets.size();
            descriptor_pool_create_info.poolSizeCount = descriptor_pool_sizes.size();
            descriptor_pool_create_info.pPoolSizes = descriptor_pool_sizes.data();

            VALIDATE_VKRESULT(vkCreateDescriptorPool(*m_logical_device_ptr->Get_Device(), &descriptor_pool_create_info, nullptr, &m_descriptor_pool), "Vulkan: Failed to create descriptor pool");

            LOG_TRACE << "Vulkan: Finished creating descriptor pool";
        }

        void Resource_Grouping_Manager::Allocate_Descriptor_Sets()
        {
            LOG_INFO << "Vulkan: Allocating descriptor sets";

            Create_Descriptor_Pool();

            VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {};
            descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            descriptor_set_allocate_info.pNext = nullptr;
            descriptor_set_allocate_info.descriptorPool = m_descriptor_pool;
            descriptor_set_allocate_info.descriptorSetCount = m_descriptor_sets.size();
            descriptor_set_allocate_info.pSetLayouts = m_descriptor_set_layouts.data();

            VALIDATE_VKRESULT(vkAllocateDescriptorSets(*m_logical_device_ptr->Get_Device(), &descriptor_set_allocate_info, m_descriptor_sets.data()), "Vulkan: Failed to allocate descriptor sets");

            LOG_TRACE << "Vulkan: Allocated descriptor set";
        }

        void Resource_Grouping_Manager::Create_Write_Descriptor_Sets()
        {
            LOG_INFO << "Vulkan: Creating write descriptor sets";

            for (unsigned int i = 0; i < m_resource_groupings.size(); i++)
            {
                if (m_resource_groupings[i].descriptor_set_info.has_value())
                {
                    LOG_TRACE << "Vulkan: Creating write descriptor sets for resource grouping '" << m_resource_groupings[i].label << "'";

                    std::list<VkDescriptorBufferInfo> buffer_descriptor_infos;
                    std::list<VkDescriptorImageInfo> image_descriptor_infos;

                    for (unsigned int j = 0; j < m_resource_groupings[i].resources.size(); j++)
                    {
                        if (m_resource_groupings[i].resources[j].resource_type == Storage_Manager::Resource_ID::BUFFER_RESOURCE)
                        {
                            buffer_descriptor_infos.resize(buffer_descriptor_infos.size() + 1);

                            buffer_descriptor_infos.back() = {};
                            buffer_descriptor_infos.back().buffer = m_storage_manager_ptr->Get_Buffer_Resource(m_resource_groupings[i].resources[j])->buffer;
                            buffer_descriptor_infos.back().offset = 0;
                            buffer_descriptor_infos.back().range = VK_WHOLE_SIZE;

                            m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.resize(m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.size() + 1);

                            m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.back() = {};
                            m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.back().sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                            m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.back().pNext = nullptr;
                            m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.back().dstSet = *m_resource_groupings[i].descriptor_set_info->descriptor_set_ptr;
                            m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.back().dstBinding = j;
                            m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.back().dstArrayElement = 0;
                            m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.back().descriptorCount = 1;
                            m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.back().descriptorType = m_storage_manager_ptr->Get_Buffer_Resource(m_resource_groupings[i].resources[j])->buffer_type;
                            m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.back().pBufferInfo = &buffer_descriptor_infos.back();
                            m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.back().pImageInfo = nullptr;
                            m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.back().pTexelBufferView = nullptr;
                        }
                        else if (m_resource_groupings[i].resources[j].resource_type == Storage_Manager::Resource_ID::IMAGE_RESOURCE)
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

                            image_descriptor_infos.resize(image_descriptor_infos.size() + 1);

                            image_descriptor_infos.back() = {};
                            image_descriptor_infos.back().sampler = sampler;
                            image_descriptor_infos.back().imageView = m_storage_manager_ptr->Get_Image_Resource(m_resource_groupings[i].resources[j])->image_view;
                            image_descriptor_infos.back().imageLayout = VK_IMAGE_LAYOUT_GENERAL;

                            m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.resize(m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.size() + 1);

                            m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.back() = {};
                            m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.back().sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                            m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.back().pNext = nullptr;
                            m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.back().dstSet = *m_resource_groupings[i].descriptor_set_info->descriptor_set_ptr;
                            m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.back().dstBinding = j;
                            m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.back().dstArrayElement = 0;
                            m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.back().descriptorCount = 1;
                            m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.back().descriptorType = m_storage_manager_ptr->Get_Image_Resource(m_resource_groupings[i].resources[j])->image_type;
                            m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.back().pBufferInfo = nullptr;
                            m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.back().pImageInfo = &image_descriptor_infos.back();
                            m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.back().pTexelBufferView = nullptr;
                        }
                    }

                    vkUpdateDescriptorSets(*m_logical_device_ptr->Get_Device(), m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.size(), m_resource_groupings[i].descriptor_set_info->write_descriptor_sets.data(), 0, nullptr);
                }
            }

            LOG_TRACE << "Vulkan: Finished creating write descriptor sets";
        }

        void Resource_Grouping_Manager::Add_Resource_Grouping(std::string label, std::vector<Storage_Manager::Resource_ID> resources, bool add_descriptor_set)
        {
            LOG_INFO << "Vulkan: Adding resource grouping with label " << label;

            for (unsigned int i = 0; i < m_resource_groupings.size(); i++)
            {
                if (m_resource_groupings[i].label == label)
                {
                    LOG_ERROR << "Vulkan: Label already in use";
                    exit(EXIT_FAILURE);
                }
            }

            m_resource_groupings.resize(m_resource_groupings.size() + 1);
            m_resource_groupings.back() = {};
            m_resource_groupings.back().label = label;
            m_resource_groupings.back().resources = resources;
            m_resource_groupings.back().descriptor_set_info.reset();

            if (add_descriptor_set)
            {
                m_descriptor_sets.resize(m_descriptor_sets.size() + 1);
                m_descriptor_set_layouts.resize(m_descriptor_set_layouts.size() + 1);

                Descriptor_Set_Info descriptor_set_info = {};
                descriptor_set_info.descriptor_set_ptr = &m_descriptor_sets.back();
                descriptor_set_info.descriptor_set_layout_ptr = &m_descriptor_set_layouts.back();

                m_resource_groupings.back().descriptor_set_info = descriptor_set_info;

                LOG_TRACE << "Vulkan: Creating descriptor set layout";

                std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_bindings(resources.size());
                for (unsigned int i = 0; i < resources.size(); i++)
                {
                    descriptor_set_layout_bindings[i] = {};
                    descriptor_set_layout_bindings[i].binding = i;
                    descriptor_set_layout_bindings[i].descriptorType
                        = (resources[i].resource_type == Storage_Manager::Resource_ID::BUFFER_RESOURCE) ? m_storage_manager_ptr->Get_Buffer_Resource(resources[i])->buffer_type : m_storage_manager_ptr->Get_Image_Resource(resources[i])->image_type;
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

                VALIDATE_VKRESULT(vkCreateDescriptorSetLayout(*m_logical_device_ptr->Get_Device(), &descriptor_set_layout_create_info, nullptr, &m_descriptor_set_layouts.back()), "Vulkan: Failed to create descriptor set layout");

                LOG_TRACE << "Vulkan: Created descriptor set layout. Descriptor set will be allocated after calling Allocate_Descriptor_Sets";
            }

            LOG_TRACE << "Vulkan: Added resource grouping";
        }

        void Resource_Grouping_Manager::Create_Descriptor_Sets()
        {
            Allocate_Descriptor_Sets();
            Create_Write_Descriptor_Sets();
        }

        bool Resource_Grouping_Manager::Resource_Group_Has_Descriptor_Set(std::string resource_group_label)
        {
            for (unsigned int i = 0; i < m_resource_groupings.size(); i++)
            {
                if (m_resource_groupings[i].label == resource_group_label)
                {
                    return m_resource_groupings[i].descriptor_set_info.has_value();
                }
            }

            LOG_ERROR << "Vulkan: No resource group with label '" << resource_group_label << "'";
            exit(EXIT_FAILURE);
        }

        VkDescriptorSet* Resource_Grouping_Manager::Get_Descriptor_Set(std::string resource_grouping_label)
        {
            for (unsigned int i = 0; i < m_resource_groupings.size(); i++)
            {
                if (m_resource_groupings[i].descriptor_set_info.has_value())
                {
                    if (m_resource_groupings[i].label == resource_grouping_label)
                    {
                        return m_resource_groupings[i].descriptor_set_info->descriptor_set_ptr;
                    }
                }
            }

            LOG_ERROR << "Vulkan: No descriptor set with label '" << resource_grouping_label << "'";
            exit(EXIT_FAILURE);
        }

        VkDescriptorSetLayout* Resource_Grouping_Manager::Get_Descriptor_Set_Layout(std::string resource_grouping_label)
        {
            for (unsigned int i = 0; i < m_resource_groupings.size(); i++)
            {
                if (m_resource_groupings[i].descriptor_set_info.has_value())
                {
                    if (m_resource_groupings[i].label == resource_grouping_label)
                    {
                        return m_resource_groupings[i].descriptor_set_info->descriptor_set_layout_ptr;
                    }
                }
            }

            LOG_ERROR << "Vulkan: No descriptor set layout with label '" << resource_grouping_label << "'";
            exit(EXIT_FAILURE);
        }

        std::vector<Storage_Manager::Resource_ID> Resource_Grouping_Manager::Get_Resources(std::string resource_grouping_label)
        {
            for (unsigned int i = 0; i < m_resource_groupings.size(); i++)
            {
                if (m_resource_groupings[i].label == resource_grouping_label)
                {
                    return m_resource_groupings[i].resources;
                }
            }

            LOG_ERROR << "Vulkan: Cannot find resource grouping with label '" << resource_grouping_label << "'";
            exit(EXIT_FAILURE);
        }
    } // namespace Vulkan
} // namespace Cascade_Graphics