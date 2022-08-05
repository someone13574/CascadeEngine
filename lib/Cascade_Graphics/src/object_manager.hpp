#pragma once

#include "Data_Types/vector_3.hpp"
#include <functional>
#include <string>
#include <vector>


namespace Cascade_Graphics
{
    class Object_Manager
    {
    public:
        struct GPU_Voxel
        {
            float position_x;
            float position_y;
            float position_z;
            float size;

            unsigned int hit_links[8];
            unsigned int miss_links[8];
        };

    private:
        struct Voxel
        {
            double size;
            Vector_3<double> position;

            unsigned int parent_index;
            unsigned int child_indices[8] = {0, 0, 0, 0, 0, 0, 0, 0};
            unsigned int child_index;
            unsigned int hit_links[8];
            unsigned int miss_links[8];

            unsigned int depth;
            bool is_leaf;
        };

        struct Object
        {
            std::string label;
            std::vector<Voxel> voxels;
        };

    private:
        std::vector<Object> m_objects;

    private:
        void Voxel_Sample_Volume_Function(Voxel voxel, unsigned int max_depth, std::function<bool(Vector_3<double>)> volume_sample_function, bool& is_fully_contained, bool& is_intersecting, bool& is_center_intersecting);

    public:
        Object_Manager();

    public:
        void Create_Object_Volume_Function(std::string label, unsigned int max_depth, Vector_3<double> sample_region_center, double sample_region_size, std::function<bool(Vector_3<double>)> volume_sample_function);

        std::vector<GPU_Voxel> Get_GPU_Voxels();
    };
} // namespace Cascade_Graphics