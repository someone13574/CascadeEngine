#pragma once


#include "Data_Types/matrix_3x3.hpp"
#include "Data_Types/vector_3.hpp"
#include <cstdint>

namespace Cascade_Graphics
{
    class Camera
    {
    public:
        struct GPU_Camera_Data
        {
            float matrix_x0;
            float matrix_x1;
            float matrix_x2;
            float matrix_y0;
            float matrix_y1;
            float matrix_y2;
            float matrix_z0;
            float matrix_z1;
            float matrix_z2;

            float origin_x;
            float origin_y;
            float origin_z;

            uint32_t which_hit_buffer;

            uint32_t padding_a;
            uint32_t padding_b;
            uint32_t padding_c;
        };

    private:
        Vector_3<double> m_position;
        Vector_3<double> m_direction;
        Vector_3<double> m_up_direction = {0.0, 1.0, 0.0};
        Matrix_3x3<double> m_camera_to_world_matrix;

    private:
        void Update_Camera_To_World_Matrix();

    public:
        Camera(Vector_3<double> position, Vector_3<double> direction);

        void Set_Position(Vector_3<double> position);
        void Set_Direction(Vector_3<double> direction);
        void Set_Up_Direction(Vector_3<double> up_direction);
        void Look_At(Vector_3<double> position);

        Vector_3<double> Get_Camera_Position();
        Matrix_3x3<double> Get_Camera_To_World_Matrix();
        GPU_Camera_Data Get_GPU_Camera_Data(uint32_t which_hit_buffer);
    };
} // namespace Cascade_Graphics