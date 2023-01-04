#pragma once

#include "thread_manager.hpp"
#include "window.hpp"
#include <cascade_graphics.hpp>

namespace Cascade_Core
{
	class Window_Factory
	{
	protected:
		Cascade_Graphics::Graphics** m_graphics_ptr;
		Thread_Manager* m_thread_manager_ptr;
		Cascade_Graphics::Graphics_Factory* m_graphics_factory_ptr;

	public:
		Window_Factory(Cascade_Graphics::Graphics** graphics_ptr, Thread_Manager* thread_manager_ptr, Cascade_Graphics::Graphics_Factory* graphics_factory_ptr);
		virtual ~Window_Factory() = default;

		virtual Window* Create_Window(std::string window_title, uint32_t window_width, uint32_t window_height) const = 0;
	};
}	 // namespace Cascade_Core