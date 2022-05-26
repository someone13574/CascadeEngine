#pragma once

#include "Data_Types/vector_3.hpp"

#include <functional>
#include <stdint.h>
#include <string>
#include <vector>

namespace Cascade_Graphics
{
    class Object
    {
    public:
        struct GPU_Voxel
        {
            float x_position;
            float y_position;
            float z_position;
            float size;
        };

    private:
        struct Voxel
        {
            uint32_t parent_index;
            uint32_t child_indices[8] = {0, 0, 0, 0, 0, 0, 0, 0};

            uint8_t child_index;
            uint8_t depth;

            float x_position;
            float y_position;
            float z_position;
            float size;
        };

    private:
        std::string m_label;
        std::vector<Voxel> m_voxels;

        Vector_3<double> m_root_min;
        Vector_3<double> m_root_max;

    private:
        uint32_t Get_Voxel_Index(Vector_3<double> position);

    public:
        Object(std::string label);

    public:
        void Create_Object_Sample_Function(uint8_t max_depth, Vector_3<double> boundary_min, Vector_3<double> boundary_max, std::function<bool(Vector_3<double>)> volume_sample_function);

        std::string Get_Label();
    };
} // namespace Cascade_Graphics