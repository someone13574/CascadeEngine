#include "object.hpp"

#include "cascade_logging.hpp"

#include <cmath>

namespace Cascade_Graphics
{
    Object::Object(std::string label) : m_label(label), m_root_min(0.0, 0.0, 0.0), m_root_max(0.0, 0.0, 0.0)
    {
        LOG_INFO << "Graphics: Creating new object with label " << m_label;
    }

    uint32_t Object::Get_Voxel_Index(Vector_3<double> position)
    {
        uint32_t current_index = 0;

        if (m_voxels.size() != 0)
        {
            return 0;
        }

        while (true)
        {
            uint8_t child_index = 0;
            child_index |= (m_voxels[current_index].x_position > position.m_x);
            child_index |= (m_voxels[current_index].y_position > position.m_y) << 1;
            child_index |= (m_voxels[current_index].z_position > position.m_z) << 2;

            uint32_t child_index_in_array = m_voxels[current_index].child_indices[child_index];

            if (child_index_in_array == 0)
            {
                return current_index;
            }
            else
            {
                current_index = child_index_in_array;
            }
        }

        return 0;
    }

    uint8_t Object::Voxel_Intersects_Volume_Function(std::function<bool(Vector_3<double>)> volume_sample_function, Voxel voxel)
    {
        bool intersects_center = volume_sample_function(Vector_3<double>(voxel.x_position, voxel.y_position, voxel.z_position));
        bool intersects_nnn = volume_sample_function(Vector_3<double>(voxel.x_position - voxel.size, voxel.y_position - voxel.size, voxel.z_position - voxel.size));
        bool intersects_nnp = volume_sample_function(Vector_3<double>(voxel.x_position - voxel.size, voxel.y_position - voxel.size, voxel.z_position + voxel.size));
        bool intersects_npn = volume_sample_function(Vector_3<double>(voxel.x_position - voxel.size, voxel.y_position + voxel.size, voxel.z_position - voxel.size));
        bool intersects_npp = volume_sample_function(Vector_3<double>(voxel.x_position - voxel.size, voxel.y_position + voxel.size, voxel.z_position + voxel.size));
        bool intersects_pnn = volume_sample_function(Vector_3<double>(voxel.x_position + voxel.size, voxel.y_position - voxel.size, voxel.z_position - voxel.size));
        bool intersects_pnp = volume_sample_function(Vector_3<double>(voxel.x_position + voxel.size, voxel.y_position - voxel.size, voxel.z_position + voxel.size));
        bool intersects_ppn = volume_sample_function(Vector_3<double>(voxel.x_position + voxel.size, voxel.y_position + voxel.size, voxel.z_position - voxel.size));
        bool intersects_ppp = volume_sample_function(Vector_3<double>(voxel.x_position + voxel.size, voxel.y_position + voxel.size, voxel.z_position + voxel.size));

        uint8_t out_data = 0;
        out_data |= intersects_center;
        out_data |= (intersects_center || intersects_nnn || intersects_nnp || intersects_npn || intersects_npp || intersects_pnn || intersects_pnp || intersects_ppn || intersects_ppp) << 1;
        out_data |= (intersects_center && intersects_nnn && intersects_nnp && intersects_npn && intersects_npp && intersects_pnn && intersects_pnp && intersects_ppn && intersects_ppp) << 2;

        return out_data;
    }

    void Object::Create_Object_Sample_Function(uint8_t max_depth, Vector_3<double> boundary_min, Vector_3<double> boundary_max, std::function<bool(Vector_3<double>)> volume_sample_function)
    {
        m_voxels.clear();

        m_max_depth = max_depth;

        Vector_3<double> boundary_center = (boundary_max + boundary_min) * 0.5;
        double max_size = std::max({std::abs(boundary_max.m_x - boundary_center.m_x), std::abs(boundary_max.m_y - boundary_center.m_y), std::abs(boundary_max.m_z - boundary_center.m_z)});

        m_root_min = boundary_center - max_size;
        m_root_max = boundary_center + max_size;

        Voxel root_voxel = {};
        root_voxel.parent_index = 0;
        root_voxel.child_indices[0] = 0;
        root_voxel.child_indices[1] = 0;
        root_voxel.child_indices[2] = 0;
        root_voxel.child_indices[3] = 0;
        root_voxel.child_indices[4] = 0;
        root_voxel.child_indices[5] = 0;
        root_voxel.child_indices[6] = 0;
        root_voxel.child_indices[7] = 0;
        root_voxel.child_index = 0x80;
        root_voxel.depth = 0;
        root_voxel.x_position = boundary_center.m_x;
        root_voxel.y_position = boundary_center.m_y;
        root_voxel.z_position = boundary_center.m_z;
        root_voxel.size = max_size;

        m_voxels.push_back(root_voxel);

        uint32_t current_index = 0;
        while (true)
        {
            if ((m_voxels[current_index].child_index & 0x80) && !(m_voxels[current_index].depth == max_depth))
            {
                bool created_child = false;
                for (unsigned int child = 0; child < 8; child++)
                {
                    if (m_voxels[current_index].child_indices[child] == 0)
                    {
                        Voxel new_voxel = {};
                        new_voxel.parent_index = current_index;
                        new_voxel.child_indices[0] = 0;
                        new_voxel.child_indices[1] = 0;
                        new_voxel.child_indices[2] = 0;
                        new_voxel.child_indices[3] = 0;
                        new_voxel.child_indices[4] = 0;
                        new_voxel.child_indices[5] = 0;
                        new_voxel.child_indices[6] = 0;
                        new_voxel.child_indices[7] = 0;
                        new_voxel.size = m_voxels[current_index].size * 0.5;
                        new_voxel.x_position = m_voxels[current_index].x_position + ((child & 1) * m_voxels[current_index].size - new_voxel.size);
                        new_voxel.y_position = m_voxels[current_index].y_position + (((child & 2) >> 1) * m_voxels[current_index].size - new_voxel.size);
                        new_voxel.z_position = m_voxels[current_index].z_position + (((child & 4) >> 2) * m_voxels[current_index].size - new_voxel.size);
                        new_voxel.depth = m_voxels[current_index].depth + 1;

                        uint8_t intersection_data = Voxel_Intersects_Volume_Function(volume_sample_function, new_voxel);
                        if ((intersection_data & 4) >> 2)
                        {
                            new_voxel.child_index = 1 << 6;
                        }
                        else
                        {
                            if (new_voxel.depth == max_depth)
                            {
                                new_voxel.child_index = (intersection_data & 1) << 7;
                            }
                            else
                            {
                                new_voxel.child_index = ((intersection_data & 2) >> 1) << 7;
                            }
                        }

                        if (new_voxel.child_index != 0)
                        {
                            new_voxel.child_index |= (child & 1);
                            new_voxel.child_index |= (child & 2) << 1;
                            new_voxel.child_index |= (child & 4) << 2;

                            m_voxels.push_back(new_voxel);

                            m_voxels[current_index].child_indices[child] = m_voxels.size() - 1;

                            current_index = m_voxels.size() - 1;

                            created_child = true;

                            break;
                        }
                        else
                        {
                            m_voxels[current_index].child_indices[child] = -1;
                        }
                    }
                }

                if (created_child)
                {
                    continue;
                }
            }

            if (m_voxels[current_index].depth == 0)
            {
                break;
            }

            current_index = m_voxels[current_index].parent_index;
        }
    }

    std::string Object::Get_Label()
    {
        return m_label;
    }

    std::vector<Object::GPU_Voxel> Object::Get_GPU_Voxels()
    {
        std::vector<GPU_Voxel> gpu_voxels;
        for (unsigned int i = 0; i < m_voxels.size(); i++)
        {
            if ((m_voxels[i].depth == m_max_depth && m_voxels[i].child_index & 0x80) || m_voxels[i].child_index & 0x40) // || m_voxels[i].child_index & 0x40
            {
                GPU_Voxel gpu_voxel = {};
                gpu_voxel.x_position = m_voxels[i].x_position;
                gpu_voxel.y_position = m_voxels[i].y_position;
                gpu_voxel.z_position = m_voxels[i].z_position;
                gpu_voxel.size = m_voxels[i].size;

                gpu_voxels.push_back(gpu_voxel);
            }
        }

        return gpu_voxels;
    }
} // namespace Cascade_Graphics