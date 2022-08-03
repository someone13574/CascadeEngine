#include "object_manager.hpp"

#include "cascade_logging.hpp"
#include <stack>

namespace Cascade_Graphics
{
    Object_Manager::Object_Manager()
    {
    }

    void Object_Manager::Voxel_Sample_Volume_Function(Voxel voxel, std::function<bool(Vector_3<double>)> volume_sample_function, bool& is_fully_contained, bool& is_intersecting, bool& is_center_intersecting)
    {
        const unsigned int SAMPLES = 8;

        is_center_intersecting = volume_sample_function(voxel.position);
        is_fully_contained = is_center_intersecting;
        is_intersecting = is_center_intersecting;

        double step_size = (1.0 / (SAMPLES - 1)) * voxel.size * 2.0;
        Vector_3<double> sample_position(0.0, 0.0, 0.0);

        for (unsigned int i = 0; i < SAMPLES; i++)
        {
            for (unsigned int j = 0; j < SAMPLES; j++)
            {
                for (unsigned int k = 0; k < SAMPLES; k++)
                {
                    sample_position = voxel.position - Vector_3<double>(voxel.size, voxel.size, voxel.size) + Vector_3<double>(i * step_size, j * step_size, k * step_size);

                    bool sample = volume_sample_function(sample_position);

                    is_fully_contained = is_fully_contained && sample;
                    is_intersecting = is_intersecting || sample;

                    if (!is_fully_contained && is_intersecting)
                    {
                        return;
                    }
                }
            }
        }
    }

    void Object_Manager::Create_Object_Volume_Function(std::string label, unsigned int max_depth, Vector_3<double> sample_region_center, double sample_region_size, std::function<bool(Vector_3<double>)> volume_sample_function)
    {
        LOG_INFO << "Graphics: Creating object with label '" << label << "'";

        for (unsigned int i = 0; i < m_objects.size(); i++)
        {
            if (m_objects[i].label == label)
            {
                LOG_ERROR << "Graphics: The label '" << label << "' is already in use";
                exit(EXIT_FAILURE);
            }
        }

        m_objects.emplace_back();
        m_objects.back() = {};
        m_objects.back().label = label;

        Voxel root_voxel = {};
        root_voxel.size = sample_region_size;
        root_voxel.position = sample_region_center;
        root_voxel.parent_index = -1;
        root_voxel.child_indices[0] = 0;
        root_voxel.child_indices[1] = 0;
        root_voxel.child_indices[2] = 0;
        root_voxel.child_indices[3] = 0;
        root_voxel.child_indices[4] = 0;
        root_voxel.child_indices[5] = 0;
        root_voxel.child_indices[6] = 0;
        root_voxel.child_indices[7] = 0;
        root_voxel.child_index = 0;
        root_voxel.hit_link = -1;
        root_voxel.miss_link = -1;
        root_voxel.depth = 0;
        root_voxel.is_leaf = false;

        m_objects.back().voxels.push_back(root_voxel);

        std::stack<unsigned int> voxel_stack;
        voxel_stack.push(0);

        while (!voxel_stack.empty())
        {
            unsigned int current_voxel_index = voxel_stack.top();
            Voxel current_voxel = m_objects.back().voxels[current_voxel_index];

            if (current_voxel.depth != max_depth)
            {
                unsigned int previous_child = -1;

                for (unsigned int i = 0; i < 8; i++)
                {
                    Voxel child_voxel = {};
                    child_voxel.size = current_voxel.size * 0.5;
                    child_voxel.position = current_voxel.position + Vector_3<double>((i & 1) * current_voxel.size - child_voxel.size, ((i & 2) >> 1) * current_voxel.size - child_voxel.size, ((i & 4) >> 2) * current_voxel.size - child_voxel.size);
                    child_voxel.parent_index = current_voxel_index;
                    child_voxel.child_indices[0] = 0;
                    child_voxel.child_indices[1] = 0;
                    child_voxel.child_indices[2] = 0;
                    child_voxel.child_indices[3] = 0;
                    child_voxel.child_indices[4] = 0;
                    child_voxel.child_indices[5] = 0;
                    child_voxel.child_indices[6] = 0;
                    child_voxel.child_indices[7] = 0;
                    child_voxel.child_index = i;
                    child_voxel.hit_link = -1;
                    child_voxel.miss_link = current_voxel.miss_link;
                    child_voxel.depth = current_voxel.depth + 1;

                    bool is_fully_contained;
                    bool is_intersecting;
                    bool is_center_intersecting;
                    Voxel_Sample_Volume_Function(child_voxel, volume_sample_function, is_fully_contained, is_intersecting, is_center_intersecting);

                    if (is_intersecting)
                    {
                        child_voxel.is_leaf = child_voxel.depth == max_depth || is_fully_contained;

                        current_voxel.child_indices[i] = m_objects.back().voxels.size();

                        if (previous_child != -1)
                        {
                            m_objects.back().voxels[previous_child].miss_link = m_objects.back().voxels.size();
                        }
                        else
                        {
                            current_voxel.hit_link = m_objects.back().voxels.size();
                        }

                        previous_child = m_objects.back().voxels.size();
                        m_objects.back().voxels.push_back(child_voxel);
                    }
                    else
                    {
                        current_voxel.child_indices[i] = -1;
                    }
                }

                if (previous_child != -1)
                {
                    m_objects.back().voxels[previous_child].miss_link = current_voxel.miss_link;
                }
            }

            m_objects.back().voxels[current_voxel_index] = current_voxel;
            voxel_stack.pop();

            if (current_voxel.depth != max_depth)
            {
                for (unsigned int i = 0; i < 8; i++)
                {
                    if (current_voxel.child_indices[i] != -1)
                    {
                        if (!m_objects.back().voxels[current_voxel.child_indices[i]].is_leaf)
                        {
                            voxel_stack.push(current_voxel.child_indices[i]);
                        }
                    }
                }
            }
        }
    }

    std::vector<Object_Manager::GPU_Voxel> Object_Manager::Get_GPU_Voxels()
    {
        std::vector<GPU_Voxel> gpu_voxels;

        for (unsigned int i = 0; i < m_objects.size(); i++)
        {
            for (unsigned int j = 0; j < m_objects[i].voxels.size(); j++)
            {
                Voxel* current_voxel = &m_objects[i].voxels[j];

                GPU_Voxel gpu_voxel = {};
                gpu_voxel.position_x = current_voxel->position.m_x;
                gpu_voxel.position_y = current_voxel->position.m_y;
                gpu_voxel.position_z = current_voxel->position.m_z;
                gpu_voxel.size = current_voxel->size;
                gpu_voxel.hit_index = current_voxel->hit_link;
                gpu_voxel.miss_index = current_voxel->miss_link;

                gpu_voxels.push_back(gpu_voxel);
            }
        }

        return gpu_voxels;
    }
} // namespace Cascade_Graphics