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

		auto net_channel = valve::g_client_state->m_net_chan;
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

	void c_networking::reset( ) {
		m_process_cmds.clear( );
		m_latency = 0.f;

		m_seq = -1;

		m_local_tick = -1;
		m_server_tick = -1;

		m_tick_rate = -1;

		m_simulate_choke = false;
	}

	void c_networking::netvars_data_t::store_netvars( int cmd_number ) {
		m_tick_base = g_local_player->self( )->tick_base( );

		m_cmd_number = cmd_number;

		m_view_punch = g_local_player->self( )->view_punch( );
		m_punch = g_local_player->self( )->aim_punch( );

		m_punch_vel = g_local_player->self( )->aim_punch_vel( );
		m_velocity = g_local_player->self( )->velocity( );
		m_origin = g_local_player->self( )->origin( );
		m_view_offset = g_local_player->self( )->view_offset( );

		m_duck_amt = g_local_player->self( )->duck_amt( );
		m_duck_speed = g_local_player->self( )->duck_speed( );

		m_filled = true;
	}

	void c_networking::netvars_data_t::restore_netvars( ) {
		if ( !m_filled )
			return;

		auto handle_pred_error_int = [ & ]( int& networked, const int predicted, const int max_diff, const std::string_view& name ) -> bool {
			if ( std::abs( networked - predicted ) > max_diff ) {
#ifdef _DEBUG
				valve::g_cvar->console_print( { 255, 255, 255, 255 },
					"[ pred error ] %s out of max_diff ( max_diff: %s )( diff: %s )\n",
					name.data( ), std::to_string( max_diff ).c_str( ), std::to_string( networked - predicted ).c_str( )
				);
#endif
				return true;
			}

			networked = predicted;

			return false;
		};

		auto handle_pred_error_float = [ & ]( float& networked, const float predicted, const float max_diff, const std::string_view& name ) -> bool {
			if ( std::abs( networked - predicted ) > max_diff ) {
#ifdef _DEBUG
				valve::g_cvar->console_print( { 255, 255, 255, 255 },
					"[ pred error ] %s out of max_diff ( max_diff: %s )( diff: %s )\n",
					name.data( ), std::to_string( max_diff ).c_str( ), std::to_string( networked - predicted ).c_str( )
				);
#endif
				return true;
			}

			networked = predicted;

			return false;
		};

		auto handle_pred_error_vec = [ & ]( sdk::vec3_t& networked, const sdk::vec3_t predicted, const float max_diff, const std::string_view& name ) -> bool {
			for ( auto i = 0; i < 3u; i++ ) {
				if ( std::abs( networked.at( i ) - predicted.at( i ) ) <= max_diff )
					continue;

#ifdef _DEBUG
				valve::g_cvar->console_print( { 255, 255, 255, 255 },
					"[ pred error ] %s out of max_diff ( max_diff: %s )( diff: %s )\n",
					name.data( ), std::to_string( max_diff ).c_str( ), std::to_string( networked.at( i ) - predicted.at( i ) ).c_str( )
				);
#endif
				return true;
			}

			networked = predicted;

			return false;
		};

		auto handle_pred_error_qang = [ & ]( sdk::qang_t& networked, const sdk::qang_t predicted, const float max_diff, const std::string_view& name ) -> bool {
			for ( auto i = 0; i < 3u; i++ ) {
				if ( std::abs( networked.at( i ) - predicted.at( i ) ) <= max_diff )
					continue;

#ifdef _DEBUG
				valve::g_cvar->console_print( { 255, 255, 255, 255 },
					"[ pred error ] %s out of max_diff ( max_diff: %s )( diff: %s )\n",
					name.data( ), std::to_string( max_diff ).c_str( ), std::to_string( networked.at( i ) - predicted.at( i ) ).c_str( )
				);
#endif
				return true;
			}

			networked = predicted;

			return false;
		};

		auto had_pred_errors = false;

		if ( handle_pred_error_vec( g_local_player->self( )->velocity( ), m_velocity, 0.03125f, "Velocity" ) )
			had_pred_errors = true;

		if ( handle_pred_error_vec( g_local_player->self( )->origin( ), m_origin, 0.03125f, "Origin" ) )
			had_pred_errors = true;

		if ( handle_pred_error_vec( g_local_player->self( )->aim_punch_vel( ), m_punch_vel, 0.03125f, "Punch vel" ) )
			had_pred_errors = true;

		if ( handle_pred_error_float( g_local_player->self( )->view_offset( ).z( ), m_view_offset.z( ), 0.03125f, "View offset" ) )
			had_pred_errors = true;

		if ( handle_pred_error_float( g_local_player->self( )->duck_amt( ), m_duck_amt, 0.03125f, "Duck amount" ) )
			had_pred_errors = true;

		if ( handle_pred_error_float( g_local_player->self( )->duck_speed( ), m_duck_speed, 0.03125f, "Duck speed" ) )
			had_pred_errors = true;

		if ( handle_pred_error_qang( g_local_player->self( )->aim_punch( ), m_punch, 0.03125f, "Aim punch" ) )
			had_pred_errors = true;

		if ( handle_pred_error_qang( g_local_player->self( )->view_punch( ), m_view_punch, 0.03125f, "View punch" ) )
			had_pred_errors = true;

		if ( had_pred_errors ) {
			valve::g_prediction->m_prev_start_frame = -1;
			valve::g_prediction->m_cmds_predicted = 0;

			valve::g_prediction->m_prev_ack_had_errors = true;
		}
	}
}