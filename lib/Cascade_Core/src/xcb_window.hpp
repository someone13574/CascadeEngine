#ifdef __linux__

	#pragma once

	#include "thread_manager.hpp"
	#include "window.hpp"
	#include <platform_info.hpp>
	#include <xcb/xcb.h>

namespace Cascade_Core
{
	class XCB_Window : public Window
	{
		friend class XCB_Window_Factory;

	private:
		const uint32_t m_enabled_event_types[1] = {XCB_EVENT_MASK_STRUCTURE_NOTIFY};

		xcb_connection_t* m_xcb_connection_ptr;
		xcb_screen_t* m_xcb_screen_ptr;
		xcb_window_t m_xcb_window;
		xcb_generic_error_t* m_xcb_error_ptr;
		xcb_void_cookie_t m_xcb_request_check_cookie;
		xcb_intern_atom_cookie_t m_xcb_close_window_cookie;
		xcb_intern_atom_reply_t* m_xcb_close_window_reply_ptr;
		xcb_generic_event_t* m_event_ptr;

		Cascade_Graphics::Window_Info* m_renderer_window_info_ptr;

	protected:
		void Create_Window() override;
		void Process_Events() override;
		void Destroy_Window() override;

	private:
		XCB_Window(std::string window_title, uint32_t window_width, uint32_t window_height, Cascade_Graphics::Graphics** graphics_ptr, Thread_Manager* thread_manager_ptr, Cascade_Graphics::Graphics_Factory* m_graphics_factory_ptr);

	public:
		~XCB_Window();
	};
}	 // namespace Cascade_Core

#endif