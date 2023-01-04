#pragma once

#include "platform.hpp"
#include <cstdint>

namespace Cascade_Graphics
{
	class Platform_Info
	{
	protected:
		Platform m_platform;

	protected:
		Platform_Info(Platform platform);

	public:
		virtual ~Platform_Info() = default;
	};

	class Window_Info
	{
	protected:
		Platform m_platform;

		uint32_t m_window_width;
		uint32_t m_window_height;

	protected:
		Window_Info(Platform platform, uint32_t window_width, uint32_t window_height);

	public:
		virtual ~Window_Info() = default;

		uint32_t Get_Window_Width();
		uint32_t Get_Window_Height();
	};
}	 // namespace Cascade_Graphics