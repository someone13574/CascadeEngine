#pragma once

#include "platform_info.hpp"

namespace Cascade_Graphics
{
    class XCB_Platform_Info : public Platform_Info
    {
    private:
        void** m_connection_ptr;    // Pointer to an xcb_connection_t*
        void* m_visual_id_ptr;      // Pointer to an xcb_visualid_t

    public:
        XCB_Platform_Info(void** connection_ptr, void* visual_id_ptr);
        virtual ~XCB_Platform_Info() = default;

        void** Get_Connection();
        void* Get_Visual_Id();
    };

    class XCB_Window_Info : public Window_Info
    {
    private:
        void** m_connection_ptr;    // Pointer to an xcb_connection_t*
        void* m_window_ptr;         // Pointer to an xcb_window_t

    public:
        XCB_Window_Info(uint32_t width, uint32_t height, void** connection_ptr, void* window_ptr);
        virtual ~XCB_Window_Info() = default;

        void** Get_Connection();
        void* Get_Window();

        void Update_Window_Info() override;
    };
}    // namespace Cascade_Graphics