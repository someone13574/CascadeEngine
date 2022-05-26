#include "cascade_engine.hpp"

#include <cmath>
#include <memory>

std::shared_ptr<Cascade_Core::Window> main_window_ptr;

void Update(Cascade_Core::Application* instance_ptr)
{
    double elapsed_seconds = instance_ptr->Get_Elapsed_Time().count() / 1000.0;

    std::shared_ptr<Cascade_Graphics::Camera> camera_ptr = main_window_ptr->Get_Renderer()->Get_Camera();

    camera_ptr->Set_Position({std::sin(elapsed_seconds) * 3.0, std::sin(elapsed_seconds), std::cos(elapsed_seconds) * 3.0});
    camera_ptr->Look_At({0.0, 0.0, 0.0});
}

bool Volume_Sample_Function(Cascade_Graphics::Vector_3<double> position)
{
    return position.m_y + position.m_x < 0.0;
}

int main()
{
    Cascade_Core::Application application({"Test Cascade Application", 0, 4});
    main_window_ptr = application.Create_Window("Main Window", 1280, 720);

    main_window_ptr->Get_Renderer()->Get_Object_Manager()->Create_Object_From_Sample_Function("test", 3, {-1.0, -1.0, -1.0}, {1.0, 1.0, 1.0}, Volume_Sample_Function);

    application.Run_Program_Loop(Update, 60);
}