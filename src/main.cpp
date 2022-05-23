#include "cascade_engine.hpp"

#include <memory>

int main()
{
    Cascade_Core::Application application({"Test Cascade Application", 0, 3});

    std::shared_ptr<Cascade_Core::Window> main_window_ptr = application.Create_Window("Main Window", 1280, 720);
    // std::shared_ptr<Cascade_Core::Window> secondary_window_ptr = application.Create_Window("Secondary Window", 300, 300);

    application.Wait_For_Windows_To_Exit();
}