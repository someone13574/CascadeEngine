#pragma once

#include "Data_Types/matrix_3x3.hpp"
#include "Data_Types/vector_3.hpp"

namespace CascadeGraphics
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
        };

    private:
        Vector_3<double> m_position;
        Vector_3<double> m_direction;
        Matrix_3x3<double> m_camera_to_world_matrix;

    private:
        void Update_Camera_To_World_Matrix();

    public:
        Camera(Vector_3<double> position, Vector_3<double> direction);

        void Update_Direction(Vector_3<double> direction);

        Vector_3<double> Get_Camera_Position();
        Matrix_3x3<double> Get_Camera_To_World_Matrix();
    };
} // namespace CascadeGraphics