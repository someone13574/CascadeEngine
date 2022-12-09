#pragma once

#include <cstdint>

namespace Cascade_Graphics
{
    class Window_Info
    {
    protected:
        uint32_t m_window_width;
        uint32_t m_window_height;

    protected:
        Window_Info(uint32_t window_width, uint32_t window_height);

    public:
        virtual ~Window_Info() = default;

        uint32_t Get_Window_Width();
        uint32_t Get_Window_Height();
    };
} // namespace Cascade_Graphics