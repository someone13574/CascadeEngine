#include "window_info.hpp"

namespace Cascade_Graphics
{
    Window_Info::Window_Info(uint32_t window_width, uint32_t window_height) : m_window_width(window_width), m_window_height(window_height)
    {
    }

    uint32_t Window_Info::Get_Window_Width()
    {
        return m_window_width;
    }

    uint32_t Window_Info::Get_Window_Height()
    {
        return m_window_height;
    }
} // namespace Cascade_Graphics