#include "window_factory.hpp"

namespace Cascade_Core
{
    Window_Factory::Window_Factory(Cascade_Graphics::Graphics** graphics_ptr, Thread_Manager* thread_manager_ptr, Cascade_Graphics::Graphics_Factory* graphics_factory_ptr) :
        m_graphics_ptr(graphics_ptr), m_thread_manager_ptr(thread_manager_ptr), m_graphics_factory_ptr(graphics_factory_ptr)
    {
    }
}    // namespace Cascade_Core