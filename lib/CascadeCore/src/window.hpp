#pragma once

#include "application.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace CascadeCore
{
    class Window
    {
      private:
        unsigned int m_window_width;
        unsigned int m_window_height;
        std::string m_window_title;
        GLFWwindow* m_window_ptr;

      public:
        Window(unsigned int width, unsigned int height);
    };
} // namespace CascadeCore