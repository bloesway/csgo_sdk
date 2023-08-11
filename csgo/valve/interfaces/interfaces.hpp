#pragma once

namespace valve {
    class c_client {
    public:
        VFUNC( client_class_t*( __thiscall* )( decltype( this ) ), all_classes( ), 8u );
    } inline* g_client{};

    class c_engine {
    public:
        VFUNC( bool( __thiscall* )( decltype( this ), int, player_info_t* ),
            get_player_info( int index, player_info_t* info ), 8u, index, info
        );

        VFUNC( int( __thiscall* )( decltype( this ), int ), index_for_uid( int uid ), 9u, uid );

        VFUNC( bool( __thiscall* )( decltype( this ) ), in_game( ), 26u );
    } inline* g_engine{};

    class c_entity_list {
    public:
        VFUNC( base_entity_t*( __thiscall* )( decltype( this ), int ), get_entity( int index ), 3u, index );

        VFUNC( base_entity_t*( __thiscall* )( decltype( this ), ent_handle_t ), get_entity( ent_handle_t handle ), 4u, handle );
    } inline* g_entity_list{};

    struct global_vars_base_t {
        float           m_real_time{};
        int             m_frame_count{};

        float           m_abs_frame_time{},
                        m_abs_frame_start_time_std_dev{},
                        m_cur_time{},
                        m_frame_time{};

        int             m_max_clients{}, m_tick_count{};
        float           m_interval_per_tick{}, m_interp_amt{};
        int             m_sim_ticks_this_frame{}, m_network_protocol{};

        sdk::address_t  m_save_data{};
        bool            m_client{}, m_remote_client{};
    } inline* g_global_vars{};

    struct client_state_t {
        struct net_chan_t {
            VFUNC( float( __thiscall* )( decltype( this ), e_net_flow ), latency( const e_net_flow flow ), 9u, flow );

            VFUNC( int( __thiscall* )( decltype( this ), std::uintptr_t ), send_datagram( ), VARVAL( 48u, 46u ), 0u );

            std::uint8_t    pad0[ 20u ]{};
            bool            m_processing_messages{},
                            m_should_delete{};
            std::uint8_t    pad1[ 2u ]{};
            int             m_out_seq{},
                            m_in_seq{},
                            m_out_seq_ack{},
                            m_out_rel_state{},
                            m_in_rel_state{},
                            m_choked_packets{};
            std::uint8_t    pad2[ 1044u ]{};
        };

        std::uint8_t    pad0[ 156u ]{};
        net_chan_t*     m_net_chan{};
        int             m_challenge_number{};
        std::uint8_t    pad1[ 4u ]{};
        double          m_connect_time{};
        int             m_retry_number{};
        std::uint8_t    pad2[ 84u ]{};
        int             m_signon_state{};
        std::uint8_t    pad3[ 4u ]{};
        double          m_next_cmd_time{};
        int             m_server_count{},
                        m_cur_seq{};
        std::uint8_t    pad4[ 8u ]{};
        float           m_clock_offsets[ 16u ]{};
        int             m_cur_clock_offset{},
                        m_server_tick{},
                        m_client_tick{},
                        m_delta_tick{};
        std::uint8_t    pad5[ 4u ]{};
        int             m_view_entity{},
                        m_player_slot{};
        bool            m_paused{};
        std::uint8_t    pad6[ 3u ]{};
        char            m_level_name[ 260u ]{},
                        m_level_name_short[ 40u ]{};
        std::uint8_t    pad7[ VARVAL( 92u, 212u ) ]{};
        int             m_max_clients{};
        std::uint8_t    pad8[ VARVAL( 18828u, 18836u ) ]{};
        int             m_old_tick_count{};
        float           m_tick_remainder{},
                        m_frame_time{};
        int             m_last_cmd_out{},
                        m_choked_cmds{},
                        m_last_cmd_ack{},
                        m_last_server_tick{},
                        m_cmd_ack{},
                        m_sound_seq{},
                        m_last_progress_percent{};
        bool            m_is_hltv{};
        std::uint8_t    pad9[ 75u ]{};
        sdk::qang_t     m_view_angles{};
        std::uint8_t    pad10[ 208u ]{};
    } inline* g_client_state{};

    struct input_t {
        std::uint8_t        pad0[ VARVAL( 4u, 12u ) ]{};
        bool                m_track_ir_available{},
                            m_mouse_initialized{},
                            m_mouse_active{};
        std::uint8_t        pad1[ VARVAL( 158u, 154u ) ]{};
        bool                m_camera_in_third_person{};
        std::uint8_t        pad2[ VARVAL( 1u, 2u ) ]{};
        sdk::vec3_t         m_camera_offset{};
        std::uint8_t        pad3[ VARVAL( 54u, 56u ) ]{};
        user_cmd_t*         m_cmds{};
        vfyd_user_cmd_t*    m_vfyd_cmds{};
    } inline* g_input{};

    class c_cvar {
    public:
        VFUNC( cvar_t*( __thiscall* )( decltype( this ), const char* ), find_var( const char* name ), 16u, name );
    } inline* g_cvar{};

    struct base_player_t;

    class c_move_helper {
    public:
        VFUNC( void( __thiscall* )( decltype( this ), base_player_t* ), set_host( base_player_t* player ), 1u, player );
    } inline* g_move_helper{};

    struct prediction_t {
        VFUNC( void( __thiscall* )( decltype( this ), int, bool, int, int ),
            update( int start, bool valid, int in_ack, int out_cmd ), 3u, start, valid, in_ack, out_cmd
        );

        VFUNC( void( __thiscall* )( decltype( this ), base_player_t*, user_cmd_t*, c_move_helper*, move_data_t* ),
            setup_move( base_player_t* player, user_cmd_t* cmd, c_move_helper* move_helper, move_data_t* move_data ),
            20u, player, cmd, move_helper, move_data
        );
       
        VFUNC( void( __thiscall* )( decltype( this ), base_player_t*, user_cmd_t*, move_data_t* ),
            finish_move( base_player_t* player, user_cmd_t* cmd, move_data_t* move_data ), 21u, player, cmd, move_data
        );

        std::uint8_t    pad0[ 8u ]{};
        bool            m_in_prediction{};
        std::uint8_t    pad1{};
        bool            m_engine_paused{},
                        m_old_cl_predict_value{};
        int             m_prev_start_frame{},
                        m_incoming_packet_number{};
        float           m_last_server_world_time_stamp{};
        bool            m_first_time_predicted{};
        std::uint8_t    pad2[ 3u ]{};
        int             m_cmds_predicted{},
                        m_server_cmds_acked{};
        bool            m_prev_ack_had_errors{};
    } inline* g_prediction{};

    class c_movement {
    public:
        VFUNC( void( __thiscall* )( decltype( this ), base_player_t*, move_data_t* ),
            process_movement( base_player_t* player, move_data_t* move_data ), 1u, player, move_data
        );
        
        VFUNC( void( __thiscall* )( decltype( this ), base_player_t* ),
            start_track_pred_errors( base_player_t* player ), 3u, player
        );

        VFUNC( void( __thiscall* )( decltype( this ), base_player_t* ),
            finish_track_pred_errors( base_player_t* player ), 4u, player
        );
    } inline* g_movement{};

    struct studio_render_t {
        VFUNC( void( __thiscall* )( decltype( this ), float* ), set_clr_mod( float* value ), 27u, value );
        VFUNC( void( __thiscall* )( decltype( this ), float ), set_alpha_mod( float value ), 28u, value );

        VFUNC( void( __thiscall* )( decltype( this ), material_t*, e_override_type, int ),
            forced_material_override(
                material_t* material = nullptr,
                e_override_type type = e_override_type::normal,
                int material_index = 0
            ),
            33u, material, type, material_index
        );

        ALWAYS_INLINE bool is_forced_material_override( );

        std::uint8_t    pad0[ 592u ]{};
        material_t*     m_override_material{};
        std::uint8_t    pad1[ 12u ]{};
        e_override_type m_override_type{};
    } inline* g_studio_render{};

    class c_material_system {
    public:
        VFUNC( material_t* ( __thiscall* )( decltype( this ), const char*, key_values_t* ),
            create_material( const char* name, key_values_t* key_values ), 83u, name, key_values
        );
    } inline* g_material_system{};

    struct base_entity_t;

    class c_engine_trace {
    public:
        VFUNC( e_mask( __thiscall* )( decltype( this ), const sdk::vec3_t&, e_mask, base_entity_t** ),
            get_point_contents( const sdk::vec3_t& point, e_mask mask, base_entity_t** entity = nullptr ), 0u, point, mask, entity
        );

        VFUNC( void( __thiscall* )( decltype( this ), const ray_t&, e_mask, base_entity_t*, trace_t* ),
            clip_ray_to_entity( const ray_t& ray, e_mask mask, base_entity_t* entity, trace_t* trace ), 3u, ray, mask, entity, trace
        );

        VFUNC( void( __thiscall* )( decltype( this ), const ray_t&, e_mask, base_trace_filter_t*, trace_t* ),
            trace_ray( const ray_t& ray, e_mask mask, base_trace_filter_t* filter, trace_t* trace ), 5u, ray, mask, filter, trace
        );
    } inline* g_engine_trace{};

    class c_physics_surface_props {
    public:
        VFUNC( surface_data_t*( __thiscall* )( decltype( this ), int ), get_surface_data( int index ), 5u, index );
    } inline* g_phys_props{};

    struct game_rules_t {
        OFFSET( bool, warmup_period( ), g_ctx->offsets( ).m_game_rules.m_warmup_period );
        OFFSET( bool, freeze_period( ), g_ctx->offsets( ).m_game_rules.m_freeze_period );

        OFFSET( bool, valve_ds( ), g_ctx->offsets( ).m_game_rules.m_valve_ds );
        OFFSET( bool, bomb_planted( ), g_ctx->offsets( ).m_game_rules.m_bomb_planted );
    } inline** g_game_rules{};

    class c_game_types {
    public:
        VFUNC( e_game_type( __thiscall* )( decltype( this ) ), game_type( ), 8u );
    } inline* g_game_types{};
}

#include "impl/interfaces.inl"