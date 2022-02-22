#include "window.hpp"

namespace CascadeCore
{
    Window::Window(unsigned int width, unsigned int height) : m_window_width(width), m_window_height(height)
    {
        glfwInit();

        m_window_ptr = glfwCreateWindow(m_window_width, m_window_height, "Title here", nullptr, nullptr);
    }
} // namespace CascadeCore