#pragma once

#include "window_info.hpp"

namespace Cascade_Graphics
{
    class XCB_Window_Info : public Window_Info
    {
    private:
        void** m_connection_ptr; // Pointer to an xcb_connection_t*
        void* m_window_ptr;      // Pointer to an xcb_window_t

    public:
        XCB_Window_Info(uint32_t width, uint32_t height, void** connection_ptr, void* window_ptr);
        virtual ~XCB_Window_Info() = default;

        void** Get_Connection();
        void* Get_Window();
    };
} // namespace Cascade_Graphics