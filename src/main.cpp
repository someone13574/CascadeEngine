#include "cascade_engine.hpp"

#include <cmath>
#include <memory>

std::shared_ptr<Cascade_Core::Window> main_window_ptr;

void Update(Cascade_Core::Application* instance_ptr)
{
    double elapsed_seconds = instance_ptr->Get_Elapsed_Time().count() / 10000.0;

    std::shared_ptr<Cascade_Graphics::Camera> camera_ptr = main_window_ptr->Get_Renderer()->m_camera_ptr;

    camera_ptr->Set_Position({std::sin(elapsed_seconds) * 1.9, std::sin(elapsed_seconds * 2.0) * 1.9, std::cos(elapsed_seconds) * 1.9});
    camera_ptr->Look_At(Cascade_Graphics::Vector_3<double>(std::sin(elapsed_seconds + 0.1) * 1.9, std::sin(elapsed_seconds * 2.0 + 0.1) * 1.9, std::cos(elapsed_seconds + 0.1) * 1.9) / (camera_ptr->Get_Camera_Position().Length()) * 1.8);
    camera_ptr->Set_Up_Direction(camera_ptr->Get_Camera_Position().Normalized());
}

double Volume_Sample_Function(Cascade_Graphics::Vector_3<double> position)
{
    double position_length = position.Length();

    position /= position_length;

    return position_length - 1.8 + (std::sin(position.m_x * 20.0) * std::sin(position.m_y * 20.0) * std::sin(position.m_z * 20.0) * 0.1) + (std::sin(position.m_x * 40.0) * std::sin(position.m_y * 40.0) * std::sin(position.m_z * 40.0) * 0.05);

    return position.Length() - 1.8;
}

double Volume_Sample_Function_Cube(Cascade_Graphics::Vector_3<double> position)
{
    return position.m_x;
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

Cascade_Graphics::Vector_3<double> Color_Sample_Function_Moon(Cascade_Graphics::Vector_3<double> position, Cascade_Graphics::Vector_3<double> normal)
{
    return {0.75, 0.75, 0.75};
}

int main()
{
    Cascade_Core::Application application({"Test Cascade Application", 0, 5});
    main_window_ptr = application.Create_Window("Main Window", 1920, 1080);

    main_window_ptr->Get_Renderer()->m_object_manager_ptr->Create_Object_From_Volume_Function("planet", 8, Cascade_Graphics::Vector_3<double>(0, 0, 0), 2.0, Volume_Sample_Function, Color_Sample_Function);
    // main_window_ptr->Get_Renderer()->m_object_manager_ptr->Create_Object_From_Volume_Function("moon", 8, Cascade_Graphics::Vector_3<double>(0, 0, 0), 2.0, Volume_Sample_Function, Color_Sample_Function_Moon);
    main_window_ptr->Get_Renderer()->Update_Voxels();
    main_window_ptr->Get_Renderer()->Start_Rendering();

    // main_window_ptr->Get_Renderer()->m_object_manager_ptr->Get_Object("moon")->position = {2.0, 0.0, 0.0};
    // main_window_ptr->Get_Renderer()->m_object_manager_ptr->Get_Object("moon")->scale = {0.25, 0.25, 0.25};
    // main_window_ptr->Get_Renderer()->m_object_manager_ptr->Get_Object("moon")->rotation = {0.0, 0.0, 0.78};
    // main_window_ptr->Get_Renderer()->Update_Objects();

    application.Run_Program_Loop(Update, 60);
}