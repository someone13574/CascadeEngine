#ifdef __linux__

#pragma once

#include "window_factory.hpp"

namespace Cascade_Core
{
    class XCB_Window_Factory : public Window_Factory
    {
    public:
        XCB_Window_Factory(Cascade_Graphics::Graphics** graphics_ptr, Thread_Manager* thread_manager_ptr, Cascade_Graphics::Graphics_Factory* graphics_factory_ptr);

        Window* Create_Window(std::string window_title, uint32_t window_width, uint32_t window_height) const override;
    };
} // namespace Cascade_Core

#endif