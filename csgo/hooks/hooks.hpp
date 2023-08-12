#pragma once

namespace hooks {
#pragma region ui
    LRESULT __stdcall wnd_proc( HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam );
    inline decltype( &wnd_proc ) o_wnd_proc{};

    long D3DAPI dx9_reset( IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* present_params );
    inline decltype( &dx9_reset ) o_dx9_reset{};

    long D3DAPI dx9_present( IDirect3DDevice9* device,
        RECT* src_rect, RECT* dst_rect, HWND dst_wnd_override, RGNDATA* dirty_region
    );
    inline decltype( &dx9_present ) o_dx9_present{};

    void __fastcall lock_cursor( std::uintptr_t ecx, std::uintptr_t edx );
    inline decltype( &lock_cursor ) o_lock_cursor{};

    void __fastcall paint_traverse( void* ecx, void* edx, uint32_t id, bool force_repaint, bool allow_force );
    inline decltype( &paint_traverse ) o_paint_traverse{};
#pragma endregion

#pragma region client
    void __stdcall frame_stage( valve::e_frame_stage stage );
    inline decltype( &frame_stage ) o_frame_stage{};

    void __stdcall create_move_proxy( int seq_number, float input_sample_frame_time, bool active );
    void __stdcall create_move( int seq_number, float input_sample_frame_time, bool active, bool& send_packet );

    using o_create_move_t = void( __thiscall* )( valve::c_client* const, int, float, bool );
    inline o_create_move_t o_create_move{};
#pragma endregion

#pragma region render
    void __fastcall draw_model( valve::studio_render_t* ecx, std::uintptr_t edx, std::uintptr_t results, const valve::draw_model_info_t& info,
        sdk::mat3x4_t* bones, float* flex_weights, float* flex_delayed_weights, const sdk::vec3_t& origin, int flags
    );
    inline decltype( &draw_model ) o_draw_model{};
#pragma endregion

#pragma region entity_list
    void __fastcall on_entity_add( void* ecx, void* edx, valve::base_entity_t* entity, valve::ent_handle_t handle );
    inline decltype( &on_entity_add ) o_on_entity_add{};

    void __fastcall on_entity_remove( void* ecx, void* edx, valve::base_entity_t* entity, valve::ent_handle_t handle );
    inline decltype( &on_entity_remove ) o_on_entity_remove{};
#pragma endregion

#pragma region engine
    void __cdecl cl_move( float frame_time, bool is_final_tick );
    inline decltype( &cl_move ) o_cl_move{};
#pragma endregion

#pragma region client_state
    void __fastcall packet_start( void* ecx, void* edx, int incoming, int outgoing );
    inline decltype( &packet_start ) o_packet_start{};
#pragma endregion
}