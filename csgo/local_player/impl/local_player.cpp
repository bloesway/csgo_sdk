#include "../../csgo.hpp"

void c_local_player::update_prediction( ) const {
    if ( valve::g_client_state->m_delta_tick <= 0 )
        return;

    valve::g_prediction->update( valve::g_client_state->m_delta_tick, true,
        valve::g_client_state->m_last_cmd_ack, valve::g_client_state->m_last_cmd_out + valve::g_client_state->m_choked_cmds
    );
}

void c_local_player::create_move( bool& send_packet,
    valve::user_cmd_t& cmd, valve::vfyd_user_cmd_t& vfyd_cmd
) {
    const auto old_angles = cmd.m_view_angles;

    update_prediction( );

    hacks::g_move->handle( cmd );

    {
        if ( ( m_weapon = self( )->weapon( ) ) )
            m_weapon_info = m_weapon->info( );
        else
            m_weapon_info = nullptr;
    }

    cmd.sanitize( );

    hacks::g_move->rotate( cmd, old_angles );

    vfyd_cmd.m_cmd = cmd;
    vfyd_cmd.m_checksum = cmd.checksum( );
}