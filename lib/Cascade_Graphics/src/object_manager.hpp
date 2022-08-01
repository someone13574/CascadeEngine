#pragma once

#include "object.hpp"

#include <functional>
#include <string>
#include <vector>

namespace Cascade_Graphics
{
    class Object_Manager
    {
    private:
        std::vector<Object> m_objects;

    private:
        bool Does_Object_Exist(std::string label);
        std::string Add_Postfix(std::string label);

    public:
        Object_Manager();

    public:
        void Create_Object_From_Sample_Function(std::string label, uint8_t max_depth, Vector_3<double> boundary_min, Vector_3<double> boundary_max, std::function<bool(Vector_3<double>)> volume_sample_function);
        std::vector<Object::GPU_Voxel> Get_GPU_Voxels();
    };
} // namespace Cascade_Graphics