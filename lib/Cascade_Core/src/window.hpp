#pragma once

#include "thread_manager.hpp"
#include <cascade_graphics.hpp>
#include <string>

namespace Cascade_Core
{
	class Window
	{
	protected:
		std::string m_window_title;
		uint32_t m_window_width;
		uint32_t m_window_height;

		Cascade_Graphics::Renderer* m_renderer_ptr;
		Cascade_Graphics::Graphics** m_graphics_ptr;

		Thread* m_window_thread_ptr;
		Thread_Manager* m_thread_manager_ptr;

	protected:
		static void Thread_Start_Function(Thread* window_thread_ptr, void* window_void_ptr);
		static void Thread_Loop_Function(Thread* window_thread_ptr, void* window_void_ptr);
		static void Thread_Exit_Function(Thread* window_thread_ptr, void* window_void_ptr);

		virtual void Create_Window() = 0;
		virtual void Process_Events() = 0;
		virtual void Destroy_Window() = 0;

	public:
		Window(std::string window_title, uint32_t window_width, uint32_t window_height, Cascade_Graphics::Graphics** graphics_ptr, Thread_Manager* thread_manager_ptr);
		virtual ~Window();
	};
}	 // namespace Cascade_Core