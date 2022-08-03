#include "object_manager.hpp"

#include "cascade_logging.hpp"
#include <stack>

namespace Cascade_Graphics
{
    Object_Manager::Object_Manager()
    {
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
        root_voxel.add_to_stack = true;

        m_objects.back().voxels.push_back(root_voxel);

        std::stack<unsigned int> voxel_stack;
        voxel_stack.push(0);

        while (!voxel_stack.empty())
        {
            unsigned int current_voxel_index = voxel_stack.top();
            Voxel current_voxel = m_objects.back().voxels[current_voxel_index];

            if (current_voxel.depth != max_depth)
            {
                unsigned int first_child = -1;
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

                    bool center_intersection = volume_sample_function(child_voxel.position);
                    bool corner_000_intersection = volume_sample_function(child_voxel.position + Vector_3<double>(-child_voxel.size, -child_voxel.size, -child_voxel.size));
                    bool corner_001_intersection = volume_sample_function(child_voxel.position + Vector_3<double>(-child_voxel.size, -child_voxel.size, child_voxel.size));
                    bool corner_010_intersection = volume_sample_function(child_voxel.position + Vector_3<double>(-child_voxel.size, child_voxel.size, -child_voxel.size));
                    bool corner_011_intersection = volume_sample_function(child_voxel.position + Vector_3<double>(-child_voxel.size, child_voxel.size, child_voxel.size));
                    bool corner_100_intersection = volume_sample_function(child_voxel.position + Vector_3<double>(child_voxel.size, -child_voxel.size, -child_voxel.size));
                    bool corner_101_intersection = volume_sample_function(child_voxel.position + Vector_3<double>(child_voxel.size, -child_voxel.size, child_voxel.size));
                    bool corner_110_intersection = volume_sample_function(child_voxel.position + Vector_3<double>(child_voxel.size, child_voxel.size, -child_voxel.size));
                    bool corner_111_intersection = volume_sample_function(child_voxel.position + Vector_3<double>(child_voxel.size, child_voxel.size, child_voxel.size));

                    if ((center_intersection && child_voxel.depth == max_depth)
                        || (corner_000_intersection || corner_001_intersection || corner_010_intersection || corner_011_intersection || corner_100_intersection || corner_101_intersection || corner_110_intersection || corner_111_intersection
                            || center_intersection))
                    {
                        child_voxel.add_to_stack = !((corner_000_intersection && corner_001_intersection && corner_010_intersection && corner_011_intersection && corner_100_intersection && corner_101_intersection && corner_110_intersection
                                                      && corner_111_intersection && center_intersection)
                                                     || (child_voxel.depth == max_depth));

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
                        if (m_objects.back().voxels[current_voxel.child_indices[i]].add_to_stack)
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