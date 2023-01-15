#ifdef __linux__

    #include "xcb_window_factory.hpp"

    #include "xcb_window.hpp"

namespace Cascade_Core
{
    XCB_Window_Factory::XCB_Window_Factory(Cascade_Graphics::Graphics** graphics_ptr, Thread_Manager* thread_manager_ptr, Cascade_Graphics::Graphics_Factory* graphics_factory_ptr) :
        Window_Factory::Window_Factory(graphics_ptr, thread_manager_ptr, graphics_factory_ptr)
    {
    }

    Window* XCB_Window_Factory::Create_Window(std::string window_title, uint32_t window_width, uint32_t window_height) const
    {
        return new XCB_Window(window_title, window_width, window_height, m_graphics_ptr, m_thread_manager_ptr, m_graphics_factory_ptr);
    }
}    // namespace Cascade_Core

#endif