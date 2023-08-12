#include "../../../csgo.hpp"

namespace hacks {
	void c_networking::start( ) {
		m_tick_rate = static_cast< int >( 1.f / valve::g_global_vars->m_interval_per_tick );
		m_simulate_choke = false;

		if ( auto net_channel_info = valve::g_engine->net_info( ); net_channel_info ) {
			m_latency = net_channel_info->latency( valve::e_net_flow::in ) 
				+ net_channel_info->latency( valve::e_net_flow::out );

			if ( valve::g_client_state->m_net_chan )
				m_seq = valve::g_client_state->m_net_chan->m_out_seq;
		}

		m_local_tick = m_server_tick = valve::g_global_vars->m_tick_count + valve::to_ticks( m_latency );
	}

	void c_networking::end( ) {
		if ( m_process_cmds.size( ) > m_tick_rate )
			m_process_cmds.pop_front( );

		{
			const auto client_state = valve::g_client_state;
			if ( !client_state )
				return;

			if ( !g_local_player->self( ) || !g_local_player->self( )->alive( )
				|| g_local_player->self( )->flags( ) & valve::e_ent_flags::frozen )
				return;

			auto net_channel = client_state->m_net_chan;
			if ( !net_channel )
				return;

			if ( net_channel->m_choked_packets % 4 || !m_simulate_choke )
				return;

			const auto seq_number = net_channel->m_out_seq;
			const auto choked_cmds = net_channel->m_choked_packets;

			{
				net_channel->m_choked_packets = 0;
				net_channel->m_out_seq = m_seq;

				net_channel->send_datagram( );
			}

			net_channel->m_out_seq = seq_number;
			net_channel->m_choked_packets = choked_cmds;
		}
	}

	void c_networking::reset( ) {
		m_process_cmds.clear( );
		m_latency = 0.f;

		m_seq = -1;

		m_local_tick = -1;
		m_server_tick = -1;

		m_tick_rate = -1;

		m_simulate_choke = false;
	}
}