#include "platform_info.hpp"

namespace Cascade_Graphics
{
    Platform_Info::Platform_Info(Platform platform) : m_platform(platform)
    {
    }

    Window_Info::Window_Info(Platform platform, uint32_t window_width, uint32_t window_height) : m_platform(platform), m_window_width(window_width), m_window_height(window_height)
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