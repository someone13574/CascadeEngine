#include "cascade_engine.hpp"

int main()
{
    CascadeCore::Application application("Test Application", "v0.0.2");

    application.Create_Window(1280, 720);
    application.Create_Window(300, 300);

    application.Wait_For_Windows_To_Exit();

    return 0;
}
