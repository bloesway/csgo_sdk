#include "../../csgo.hpp"

namespace hooks {
	void __fastcall packet_start( void* ecx, void* edx, int incoming, int outgoing ) {
		if ( !g_local_player->self( )
			|| !g_local_player->self( )->alive( ) )
			return o_packet_start( ecx, edx, incoming, outgoing );

		if ( hacks::g_networking->m_process_cmds.empty( ) )
			return o_packet_start( ecx, edx, incoming, outgoing );

		if ( ( *valve::g_game_rules )->valve_ds( ) )
			return o_packet_start( ecx, edx, incoming, outgoing );

		for ( auto it = hacks::g_networking->m_process_cmds.rbegin( ); it != hacks::g_networking->m_process_cmds.rend( ); ++it ) {
			if ( !it->m_outgoing )
				continue;

			if ( ( it->m_cmd_num == outgoing )
				|| ( ( outgoing > it->m_cmd_num ) && ( !it->m_handled || ( it->m_prev_cmd_num == outgoing ) ) ) ) {
				it->m_prev_cmd_num = outgoing;
				it->m_handled = true;

				o_packet_start( ecx, edx, incoming, outgoing );

				break;
			}
		}

		auto result = false;

		for ( auto it = hacks::g_networking->m_process_cmds.begin( ); it != hacks::g_networking->m_process_cmds.end( );) {
			if ( outgoing == it->m_cmd_num
				|| outgoing == it->m_prev_cmd_num )
				result = true;

			if ( outgoing > it->m_cmd_num
				&& outgoing > it->m_prev_cmd_num )
				it = hacks::g_networking->m_process_cmds.erase( it );
			else
				++it;
		}

		if ( !result )
			o_packet_start( ecx, edx, incoming, outgoing );
	}
}