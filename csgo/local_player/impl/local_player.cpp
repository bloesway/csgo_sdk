#include "../../csgo.hpp"

void c_local_player::frame_stage( valve::e_frame_stage stage, bool post ) {
    if ( post ) {
        /* here if not need to check local */

        if ( !self( ) || !self( )->alive( ) )
            return;

        if ( stage == valve::e_frame_stage::render_start )
            hacks::g_interpolation->handle( true );

        return;
    }

    /* here if not need to check local */

    if ( !self( ) || !self( )->alive( ) )
        return;

    if ( stage == valve::e_frame_stage::render_start )
        hacks::g_interpolation->handle( false );
}

void c_local_player::update_prediction( ) const {
    if ( valve::g_client_state->m_delta_tick <= 0 )
        return;

    valve::g_prediction->update( valve::g_client_state->m_delta_tick, true,
        valve::g_client_state->m_last_cmd_ack, valve::g_client_state->m_last_cmd_out + valve::g_client_state->m_choked_cmds
    );
}

void c_local_player::simulate_prediction( valve::user_cmd_t& cmd ) {
    m_prediction.store( );

    {
        if ( !m_prediction.m_pred_player )
            m_prediction.m_pred_player = *g_ctx->offsets( ).m_pred_player.as< valve::cs_player_t** >( );

        if ( !m_prediction.m_random_seed )
            m_prediction.m_random_seed = *g_ctx->offsets( ).m_random_seed.as< int** >( );

        if ( !m_prediction.m_move_data )
            m_prediction.m_move_data = reinterpret_cast< valve::move_data_t* >(
                valve::g_mem_alloc->alloc( sizeof( valve::move_data_t ) ) );
    }

    self( )->last_cmd( ) = &cmd;
    self( )->cur_cmd( ) = &cmd;

    *m_prediction.m_random_seed = cmd.m_random_seed;
    m_prediction.m_pred_player = self( );

    valve::g_global_vars->m_cur_time = valve::to_time( self( )->tick_base( ) );

    valve::g_global_vars->m_frame_time = valve::g_prediction->m_engine_paused ? 0.f 
                                         : valve::g_global_vars->m_interval_per_tick;

    valve::g_prediction->m_in_prediction = true;
    valve::g_prediction->m_first_time_predicted = false;

    {
        valve::g_movement->start_track_pred_errors( self( ) );

        valve::g_move_helper->set_host( self( ) );

        valve::g_prediction->check_moving_ground( self( ), valve::g_global_vars->m_frame_time );

        /* need some fixes for use
            valve::g_prediction->set_view_angles( cmd.m_view_angles );
        */

        valve::g_prediction->setup_move( self( ), &cmd, valve::g_move_helper, m_prediction.m_move_data );

        valve::g_movement->process_movement( self( ), m_prediction.m_move_data );

        valve::g_prediction->finish_move( self( ), &cmd, m_prediction.m_move_data );
    }
}

void c_local_player::finish_prediction( ) {
    m_prediction.restore( );

    valve::g_movement->finish_track_pred_errors( self( ) );

    *m_prediction.m_random_seed = -1;
    m_prediction.m_pred_player = nullptr;

    self( )->cur_cmd( ) = nullptr;

    valve::g_movement->reset( );

    valve::g_move_helper->set_host( nullptr );
}

void c_local_player::create_move( bool& send_packet,
    valve::user_cmd_t& cmd, valve::vfyd_user_cmd_t& vfyd_cmd
) {
    const auto old_angles = cmd.m_view_angles;

    update_prediction( );

    {
        if ( ( m_weapon = self( )->weapon( ) ) )
            m_weapon_info = m_weapon->info( );
        else
            m_weapon_info = nullptr;
    }

    hacks::g_move->handle( cmd );

    {
        simulate_prediction( cmd );
        
        /* funcs which need prediction */

        finish_prediction( );
    }

    cmd.sanitize( );

    hacks::g_move->rotate( cmd, old_angles );

    vfyd_cmd.m_cmd = cmd;
    vfyd_cmd.m_checksum = cmd.checksum( );

    {
        if ( !( ( *valve::g_game_rules )->valve_ds( ) ) )
            hacks::g_networking->emplace_cmd( send_packet, cmd.m_number );

        hacks::g_networking->m_simulate_choke = !send_packet;
    }
}