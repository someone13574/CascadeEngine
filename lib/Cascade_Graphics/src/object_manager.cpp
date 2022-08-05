#include "object_manager.hpp"

#include "cascade_logging.hpp"
#include <cmath>
#include <stack>

namespace Cascade_Graphics
{
    Object_Manager::Object_Manager()
    {
    }

    void Object_Manager::Voxel_Sample_Volume_Function(Voxel voxel, unsigned int max_depth, std::function<bool(Vector_3<double>)> volume_sample_function, bool& is_fully_contained, bool& is_intersecting, bool& is_center_intersecting)
    {
        unsigned int samples = std::pow(2, (max_depth - voxel.depth)) + 1;

        is_center_intersecting = volume_sample_function(voxel.position);
        is_fully_contained = is_center_intersecting;
        is_intersecting = is_center_intersecting;

        double step_size = (1.0 / (samples - 1)) * voxel.size * 2.0;
        Vector_3<double> start_position = voxel.position - Vector_3<double>(voxel.size, voxel.size, voxel.size);
        Vector_3<double> sample_position = start_position;

        for (unsigned int i = 0; i < samples; i++)
        {
            sample_position.m_y = start_position.m_y;
            for (unsigned int j = 0; j < samples; j++)
            {
                sample_position.m_z = start_position.m_z;
                for (unsigned int k = 0; k < samples; k++)
                {
                    bool sample = volume_sample_function(sample_position);

                    is_fully_contained = is_fully_contained && sample;
                    is_intersecting = is_intersecting || sample;

                    if (!is_fully_contained && is_intersecting)
                    {
                        return;
                    }

                    sample_position.m_z += step_size;
                }
                sample_position.m_y += step_size;
            }
            sample_position.m_x += step_size;
        }
    }

    void Object_Manager::Create_Object_Volume_Function(std::string label, unsigned int max_depth, Vector_3<double> sample_region_center, double sample_region_size, std::function<bool(Vector_3<double>)> volume_sample_function)
    {
        LOG_INFO << "Graphics: Creating object with label '" << label << "'";

        static const unsigned int link_order_lookup[8][8]
            = {{0, 1, 4, 2, 5, 3, 6, 7}, {1, 5, 0, 3, 4, 7, 2, 6}, {2, 3, 6, 7, 0, 1, 4, 5}, {3, 7, 2, 6, 1, 5, 0, 4}, {4, 0, 5, 6, 1, 2, 7, 3}, {5, 4, 1, 7, 0, 6, 3, 2}, {6, 2, 7, 3, 4, 0, 5, 1}, {7, 6, 3, 2, 5, 4, 1, 0}};

        static const unsigned int inverse_link_order_lookup[8][8]
            = {{0, 1, 3, 5, 2, 4, 6, 7}, {2, 0, 6, 3, 4, 1, 7, 5}, {4, 5, 0, 1, 6, 7, 2, 3}, {6, 4, 2, 0, 7, 5, 3, 1}, {1, 4, 5, 7, 0, 2, 3, 6}, {4, 2, 7, 6, 1, 0, 5, 3}, {5, 7, 1, 3, 4, 6, 0, 2}, {7, 6, 3, 2, 5, 4, 1, 0}};

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
        root_voxel.hit_links[0] = -1;
        root_voxel.hit_links[1] = -1;
        root_voxel.hit_links[2] = -1;
        root_voxel.hit_links[3] = -1;
        root_voxel.hit_links[4] = -1;
        root_voxel.hit_links[5] = -1;
        root_voxel.hit_links[6] = -1;
        root_voxel.hit_links[7] = -1;
        root_voxel.miss_links[0] = -1;
        root_voxel.miss_links[1] = -1;
        root_voxel.miss_links[2] = -1;
        root_voxel.miss_links[3] = -1;
        root_voxel.miss_links[4] = -1;
        root_voxel.miss_links[5] = -1;
        root_voxel.miss_links[6] = -1;
        root_voxel.miss_links[7] = -1;
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
                    child_voxel.hit_links[0] = -1;
                    child_voxel.hit_links[1] = -1;
                    child_voxel.hit_links[2] = -1;
                    child_voxel.hit_links[3] = -1;
                    child_voxel.hit_links[4] = -1;
                    child_voxel.hit_links[5] = -1;
                    child_voxel.hit_links[6] = -1;
                    child_voxel.hit_links[7] = -1;
                    child_voxel.miss_links[0] = current_voxel.miss_links[0];
                    child_voxel.miss_links[1] = current_voxel.miss_links[1];
                    child_voxel.miss_links[2] = current_voxel.miss_links[2];
                    child_voxel.miss_links[3] = current_voxel.miss_links[3];
                    child_voxel.miss_links[4] = current_voxel.miss_links[4];
                    child_voxel.miss_links[5] = current_voxel.miss_links[5];
                    child_voxel.miss_links[6] = current_voxel.miss_links[6];
                    child_voxel.miss_links[7] = current_voxel.miss_links[7];
                    child_voxel.depth = current_voxel.depth + 1;

                    bool is_fully_contained;
                    bool is_intersecting;
                    bool is_center_intersecting;
                    Voxel_Sample_Volume_Function(child_voxel, max_depth, volume_sample_function, is_fully_contained, is_intersecting, is_center_intersecting);

                    if (is_intersecting)
                    {
                        child_voxel.is_leaf = is_fully_contained || child_voxel.depth == max_depth;

                        current_voxel.child_indices[i] = m_objects.back().voxels.size();

                        m_objects.back().voxels.push_back(child_voxel);
                    }
                    else
                    {
                        current_voxel.child_indices[i] = -1;
                    }
                }

                for (unsigned int direction_index = 0; direction_index < 8; direction_index++)
                {
                    for (unsigned int link_index = 0; link_index < 8; link_index++)
                    {
                        unsigned int link = current_voxel.child_indices[link_order_lookup[direction_index][link_index]];

                        if (link != -1)
                        {
                            current_voxel.hit_links[direction_index] = link;
                            break;
                        }
                    }
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
                        for (unsigned int direction_index = 0; direction_index < 8; direction_index++)
                        {
                            for (unsigned int link_index = inverse_link_order_lookup[direction_index][i] + 1; link_index < 8; link_index++)
                            {
                                unsigned int link = current_voxel.child_indices[link_order_lookup[direction_index][link_index]];

                                if (link != -1)
                                {
                                    m_objects.back().voxels[current_voxel.child_indices[i]].miss_links[direction_index] = link;
                                    break;
                                }
                            }
                        }

                        if (!m_objects.back().voxels[current_voxel.child_indices[i]].is_leaf)
                        {
                            voxel_stack.push(current_voxel.child_indices[i]);
                        }
                    }
                }
            }
        }

        LOG_TRACE << "complete";
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
                gpu_voxel.hit_links[0] = current_voxel->hit_links[0];
                gpu_voxel.hit_links[1] = current_voxel->hit_links[1];
                gpu_voxel.hit_links[2] = current_voxel->hit_links[2];
                gpu_voxel.hit_links[3] = current_voxel->hit_links[3];
                gpu_voxel.hit_links[4] = current_voxel->hit_links[4];
                gpu_voxel.hit_links[5] = current_voxel->hit_links[5];
                gpu_voxel.hit_links[6] = current_voxel->hit_links[6];
                gpu_voxel.hit_links[7] = current_voxel->hit_links[7];
                gpu_voxel.miss_links[0] = current_voxel->miss_links[0];
                gpu_voxel.miss_links[1] = current_voxel->miss_links[1];
                gpu_voxel.miss_links[2] = current_voxel->miss_links[2];
                gpu_voxel.miss_links[3] = current_voxel->miss_links[3];
                gpu_voxel.miss_links[4] = current_voxel->miss_links[4];
                gpu_voxel.miss_links[5] = current_voxel->miss_links[5];
                gpu_voxel.miss_links[6] = current_voxel->miss_links[6];
                gpu_voxel.miss_links[7] = current_voxel->miss_links[7];

                gpu_voxels.push_back(gpu_voxel);
            }
        }

        return gpu_voxels;
    }
} // namespace Cascade_Graphics