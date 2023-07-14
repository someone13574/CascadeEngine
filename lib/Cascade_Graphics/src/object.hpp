#pragma once

#include "Data_Types/vector_3.hpp"
#include <string>
#include <vector>

namespace Cascade_Graphics
{
    struct Triangle
    {
        Vector_3<float> point_a;
        Vector_3<float> point_b;
        Vector_3<float> point_c;
    };

    std::vector<Triangle> Load_Ascii_Stl(std::string path);
}    // namespace Cascade_Graphics