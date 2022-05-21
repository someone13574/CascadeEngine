#include "camera.hpp"

namespace CascadeGraphics
{
    Camera::Camera(Vector_3<double> position, Vector_3<double> direction) : m_position(position), m_direction(direction), m_camera_to_world_matrix(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0)
    {
        Update_Camera_To_World_Matrix();
    }

    void Camera::Update_Camera_To_World_Matrix()
    {
        Vector_3<double> cu = Vector_3<double>::Cross(m_direction, Vector_3<double>(0.0, 1.0, 0.0)).Normalized();
        Vector_3<double> cv = Vector_3<double>::Cross(cu, m_direction);

        m_camera_to_world_matrix = Matrix_3x3<double>(cu, cv, -m_direction);
    }

    Vector_3<double> Camera::Get_Camera_Position()
    {
        return m_position;
    }

    Matrix_3x3<double> Camera::Get_Camera_To_World_Matrix()
    {
        return m_camera_to_world_matrix;
    }
} // namespace CascadeGraphics