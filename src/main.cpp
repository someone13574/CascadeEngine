#include "cascade_engine.hpp"

#include <cmath>
#include <memory>

std::shared_ptr<Cascade_Core::Window> main_window_ptr;

void Update(Cascade_Core::Application* instance_ptr)
{
    double elapsed_seconds = instance_ptr->Get_Elapsed_Time().count() / 1000.0 / 10.0;

    std::shared_ptr<Cascade_Graphics::Camera> camera_ptr = main_window_ptr->Get_Renderer()->Get_Camera();

    camera_ptr->Set_Position({std::sin(elapsed_seconds) * 2.2, std::sin(elapsed_seconds * 3.0) * 2, std::cos(elapsed_seconds) * 2.1});
    camera_ptr->Look_At(Cascade_Graphics::Vector_3<double>(std::sin(elapsed_seconds + 0.1) * 2.2, std::sin((elapsed_seconds + 0.1) * 3.0) * 2, std::cos(elapsed_seconds + 0.1) * 2.1) * 0.75);
    camera_ptr->Set_Up_Direction(camera_ptr->Get_Camera_Position().Normalized());
}

double Volume_Sample_Function(Cascade_Graphics::Vector_3<double> position)
{
    return position.Length() - 1.8 + (std::sin(position.m_x * 10.0) * std::sin(position.m_y * 10.0) * std::sin(position.m_z * 10.0) * 0.2)
           + (std::sin(position.m_x + 0.346 * 20.0) * std::sin(position.m_y + 0.574 * 20.0) * std::sin(position.m_z + 0.875 * 20.0) * 0.05);
}

Cascade_Graphics::Vector_3<double> Color_Sample_Function(Cascade_Graphics::Vector_3<double> position, Cascade_Graphics::Vector_3<double> normal)
{
    if (Cascade_Graphics::Vector_3<double>::Dot(normal, position.Normalized()) > 0.75)
    {
        return {0.0, 0.6, 0.1};
    }
    else
    {
        return {0.6, 0.4, 0.3};
    }
}

int main()
{
    Cascade_Core::Application application({"Test Cascade Application", 0, 5});
    main_window_ptr = application.Create_Window("Main Window", 1920, 1080);

    main_window_ptr->Get_Renderer()->Get_Object_Manager()->Create_Object_From_Volume_Function("test", 10, Cascade_Graphics::Vector_3<double>(0, 0, 0), 2, Volume_Sample_Function, Color_Sample_Function);
    main_window_ptr->Get_Renderer()->Update_Voxels();
    main_window_ptr->Get_Renderer()->Start_Rendering();

    application.Run_Program_Loop(Update, 60);
}