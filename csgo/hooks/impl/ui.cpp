#include "../../csgo.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

namespace hooks {
    LRESULT __stdcall wnd_proc( HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam ) {
        if ( msg == WM_KEYUP
            && wparam == VK_INSERT )
            g_menu->main( ).m_hidden ^= 1;

        if ( !g_menu->main( ).m_hidden ) {
            ImGui_ImplWin32_WndProcHandler( wnd, msg, wparam, lparam );

            return 1;
        }

        return o_wnd_proc( wnd, msg, wparam, lparam );
    }

    long D3DAPI dx9_reset( IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params ) {
        ImGui_ImplDX9_InvalidateDeviceObjects( );

        const auto ret = o_dx9_reset( device, params );

        ImGui_ImplDX9_CreateDeviceObjects( );

        return ret;
    }

    long D3DAPI dx9_present( IDirect3DDevice9* device,
        RECT* src_rect, RECT* dst_rect, HWND dst_wnd_override, RGNDATA* dirty_region
    ) {
        ImGui_ImplDX9_NewFrame( );
        ImGui_ImplWin32_NewFrame( );

        ImGui::NewFrame( );

        g_menu->render( );

        g_render->replace_draw_list( );

        ImGui::EndFrame( );

        ImGui::Render( );

        ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData( ) );

        return o_dx9_present( device, src_rect, dst_rect, dst_wnd_override, dirty_region );
    }

    void __fastcall lock_cursor( std::uintptr_t ecx, std::uintptr_t edx ) {
        using unlock_cursor_t = void( __thiscall* )( std::uintptr_t );

        if ( !g_menu->main( ).m_hidden )
            return ( *sdk::address_t{ ecx }.as< unlock_cursor_t** >( ) )[ 66u ]( ecx );

        o_lock_cursor( ecx, edx );
    }

    void __fastcall paint_traverse( void* ecx, void* edx, uint32_t id, bool force_repaint, bool allow_force ) {
        o_paint_traverse( ecx, edx, id, force_repaint, allow_force );

        {
            static auto draw_panel_id = 0u;

            if ( !draw_panel_id ) {
                if ( sdk::hash( valve::g_panel->name( id ) ) != HASH( "MatSystemTopPanel" ) )
                    return;

                draw_panel_id = id;
            }

            if ( id != draw_panel_id )
                return;
        }

        static auto in_game = false;
        if ( !in_game && valve::g_engine->in_game( ) ) {
            in_game = true;


        }
        else if ( in_game && !valve::g_engine->in_game( ) ) {
            in_game = false;

            hacks::g_networking->reset( );
        }

        g_render->process( );
    }
}