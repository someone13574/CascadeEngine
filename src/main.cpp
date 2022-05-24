#include "cascade_engine.hpp"

#include <cmath>
#include <memory>

std::shared_ptr<Cascade_Core::Window> main_window_ptr;

void Update()
{
    std::shared_ptr<CascadeGraphics::Camera> camera_ptr = main_window_ptr->Get_Renderer()->Get_Camera();
    Cascade_Core::Event_Manager::Mouse_State mouse_state = main_window_ptr->Get_Event_Manager()->Get_Mouse_State();

    CascadeGraphics::Vector_2<double> screen_position = {mouse_state.x_position / 1280.0 * 2.0 - 1.0, mouse_state.y_position / 720.0 * 2.0 - 1.0};
    screen_position *= 3.0;

    camera_ptr->Update_Direction(CascadeGraphics::Vector_3<double>(std::cos(screen_position.m_x) * std::cos(-screen_position.m_y), std::sin(-screen_position.m_y), std::sin(screen_position.m_x) * std::cos(-screen_position.m_y)));
}

int main()
{
    Cascade_Core::Application application({"Test Cascade Application", 0, 4});

    main_window_ptr = application.Create_Window("Main Window", 1280, 720);

    application.Run_Program_Loop(Update, 60);
}