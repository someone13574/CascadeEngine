#include "cascade_engine.hpp"

#include <memory>

void Update()
{
}

int main()
{
    Cascade_Core::Application application({"Test Cascade Application", 0, 4});

    std::shared_ptr<Cascade_Core::Window> main_window_ptr = application.Create_Window("Main Window", 1280, 720);

    application.Run_Program_Loop(Update, 60);
}