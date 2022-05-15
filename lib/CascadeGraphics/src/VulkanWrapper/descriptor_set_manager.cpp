#include "descriptor_set_manager.hpp"

#include "../debug_tools.hpp"

namespace CascadeGraphics
{
    namespace Vulkan
    {
        Descriptor_Set_Manager::Descriptor_Set_Manager(std::shared_ptr<Device> logical_device_ptr, std::shared_ptr<Storage_Manager> storage_manager_ptr) : m_logical_device_ptr(logical_device_ptr), m_storage_manager_ptr(storage_manager_ptr)
        {
        }

        Descriptor_Set_Manager::~Descriptor_Set_Manager()
        {
            LOG_INFO << "Vulkan: cleaning up descriptor sets";

            for (unsigned int i = 0; i < m_descriptor_set_layouts.size(); i++)
            {
                vkDestroyDescriptorSetLayout(*(m_logical_device_ptr->Get_Device()), m_descriptor_set_layouts[i], nullptr);
            }

            vkDestroyDescriptorPool(*(m_logical_device_ptr->Get_Device()), m_descriptor_pool, nullptr);

            LOG_TRACE << "Vulkan: finished cleaning up descriptor sets";
        }

        void Descriptor_Set_Manager::Create_Descriptor_Pool()
        {
            LOG_INFO << "Vulkan: creating descriptor pool";

            std::vector<VkDescriptorPoolSize> descriptor_pool_sizes;

            for (unsigned int i = 0; i < m_resource_groupings.size(); i++)
            {
                for (unsigned int j = 0; j < m_resource_groupings[i].resources.size(); j++)
                {
                    VkDescriptorType current_resource_descriptor_type = m_storage_manager_ptr->Get_Resource_Data(m_resource_groupings[i].resources[j]).descriptor_type;

                    bool found_resource_grouping = false;
                    for (unsigned int k = 0; k < descriptor_pool_sizes.size(); k++)
                    {
                        if (current_resource_descriptor_type == descriptor_pool_sizes[k].type)
                        {
                            descriptor_pool_sizes[k].descriptorCount++;

                            found_resource_grouping = true;
                            break;
                        }
                    }

                    if (!found_resource_grouping)
                    {
                        descriptor_pool_sizes.resize(descriptor_pool_sizes.size() + 1);

                        descriptor_pool_sizes.back().descriptorCount = 1;
                        descriptor_pool_sizes.back().type = current_resource_descriptor_type;
                    }
                }
            }

            VkDescriptorPoolCreateInfo descriptor_pool_create_info = {};
            descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            descriptor_pool_create_info.pNext = nullptr;
            descriptor_pool_create_info.flags = 0;
            descriptor_pool_create_info.maxSets = m_resource_groupings.size();
            descriptor_pool_create_info.poolSizeCount = descriptor_pool_sizes.size();
            descriptor_pool_create_info.pPoolSizes = descriptor_pool_sizes.data();

            VALIDATE_VKRESULT(vkCreateDescriptorPool(*(m_logical_device_ptr->Get_Device()), &descriptor_pool_create_info, nullptr, &m_descriptor_pool), "Vulkan: failed to create descriptor pool");

            LOG_TRACE << "Vulkan: finished creating descriptor pool";
        }

        void Descriptor_Set_Manager::Add_Descriptor_Set(std::string label, std::vector<Storage_Manager::Resource_ID> resources)
        {
            LOG_INFO << "Vulkan: adding descriptor set with label " << label;

            for (unsigned int i = 0; i < m_resource_groupings.size(); i++)
            {
                if (m_resource_groupings[i].label == label)
                {
                    LOG_ERROR << "Vulkan: cannot create descriptor set with label already in use";
                    exit(EXIT_FAILURE);
                }
            }

            for (unsigned int i = 0; i < resources.size(); i++)
            {
                if (!m_storage_manager_ptr->Does_Resource_Exist(resources[i]))
                {
                    LOG_ERROR << "Vulkan: resource " << resources[i].label << "-" << resources[i].index << " does not exist";
                    exit(EXIT_FAILURE);
                }

                LOG_TRACE << "Vulkan: descriptor set includes " << resources[i].label << "-" << resources[i].index;
            }

            m_descriptor_sets.resize(m_descriptor_sets.size() + 1);
            m_descriptor_set_layouts.resize(m_descriptor_set_layouts.size() + 1);
            m_resource_groupings.resize(m_resource_groupings.size() + 1);

            m_resource_groupings.back() = {};
            m_resource_groupings.back().label = label;
            m_resource_groupings.back().descriptor_set_ptr = &m_descriptor_sets.back();
            m_resource_groupings.back().descriptor_set_layout_ptr = &m_descriptor_set_layouts.back();
            m_resource_groupings.back().resources = resources;

            LOG_TRACE << "Vulkan: creating descriptor set layout";

            std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_bindings(resources.size());
            for (unsigned int i = 0; i < resources.size(); i++)
            {
                descriptor_set_layout_bindings[i] = {};
                descriptor_set_layout_bindings[i].binding = i;
                descriptor_set_layout_bindings[i].descriptorType = m_storage_manager_ptr->Get_Resource_Data(resources[i]).descriptor_type;
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

            VALIDATE_VKRESULT(vkCreateDescriptorSetLayout(*(m_logical_device_ptr->Get_Device()), &descriptor_set_layout_create_info, nullptr, &m_descriptor_set_layouts.back()), "Vulkan: failed to create descriptor set layout");

            LOG_TRACE << "Vulkan: added descriptor set";
        }

        void Descriptor_Set_Manager::Allocate_Descriptor_Sets()
        {
            LOG_INFO << "Vulkan: allocating descriptor sets";

            Create_Descriptor_Pool();

            VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {};
            descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            descriptor_set_allocate_info.pNext = nullptr;
            descriptor_set_allocate_info.descriptorPool = m_descriptor_pool;
            descriptor_set_allocate_info.descriptorSetCount = m_descriptor_sets.size();
            descriptor_set_allocate_info.pSetLayouts = m_descriptor_set_layouts.data();

            VALIDATE_VKRESULT(vkAllocateDescriptorSets(*(m_logical_device_ptr->Get_Device()), &descriptor_set_allocate_info, m_descriptor_sets.data()), "Vulkan: failed to allocate descriptor sets");
        }

        VkDescriptorSet* Descriptor_Set_Manager::Get_Descriptor_Set(std::string label)
        {
            for (unsigned int i = 0; i < m_resource_groupings.size(); i++)
            {
                if (m_resource_groupings[i].label == label)
                {
                    return m_resource_groupings[i].descriptor_set_ptr;
                }
            }

            LOG_ERROR << "Vulkan: no descriptor set with label '" << label << "'";
            exit(EXIT_FAILURE);
        }

        VkDescriptorSetLayout* Descriptor_Set_Manager::Get_Descriptor_Set_Layout(std::string label)
        {
            for (unsigned int i = 0; i < m_resource_groupings.size(); i++)
            {
                if (m_resource_groupings[i].label == label)
                {
                    return m_resource_groupings[i].descriptor_set_layout_ptr;
                }
            }

            LOG_ERROR << "Vulkan: no descriptor set with label '" << label << "'";
            exit(EXIT_FAILURE);
        }

        std::vector<Storage_Manager::Resource_ID> Descriptor_Set_Manager::Get_Resources(std::string resource_grouping_label)
        {
            for (unsigned int i = 0; i < m_resource_groupings.size(); i++)
            {
                if (m_resource_groupings[i].label == resource_grouping_label)
                {
                    return m_resource_groupings[i].resources;
                }
            }

            LOG_ERROR << "Vulkan: cannot find resource grouping with label '" << resource_grouping_label << "'";
            exit(EXIT_FAILURE);
        }
    } // namespace Vulkan
} // namespace CascadeGraphics