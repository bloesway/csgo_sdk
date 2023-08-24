#include "../../../csgo.hpp"

namespace hacks {
	void c_lag_comp::handle( ) {
		auto& players = g_players->get( );

		for ( auto& entry : players ) {
			const auto player = entry.m_player;

			if ( !player->alive( )
				|| player->friendly( g_local_player->self( ) ) )
				continue;

			auto& records = entry.m_records;

			const auto spawn_time = player->spawn_time( );
			if ( spawn_time != entry.m_spawn_time ) {
				records.clear( );

				const auto anim_state = player->anim_state( );
				if ( anim_state )
					anim_state->reset( );

				entry.m_spawn_time = spawn_time;
			}

			if ( player->networkable( )->dormant( ) ) {
				entry.m_in_dormancy = false;

				continue;
			}

			if ( player->sim_time( ) <= player->old_sim_time( ) )
				continue;

			if ( !entry.m_in_dormancy ) {
				records.clear( );

				entry.m_first_after_dormant = true;
			}

			auto prev_record = std::make_shared< valve::player_record_t >( );

			if ( !records.empty( ) ) {
				entry.m_prev_record = records.front( );

				prev_record = entry.m_prev_record;
				prev_record->m_filled = true;
			}

			const auto record = std::make_shared< valve::player_record_t >( player );

			if ( prev_record->m_filled ) {
				if ( prev_record->m_layers.at( -valve::e_anim_layer::alive_loop ).m_cycle 
					== record->m_layers.at( -valve::e_anim_layer::alive_loop ).m_cycle ) {
					player->sim_time( ) = prev_record->m_sim_time;

					continue;
				}

				if ( ( record->m_origin - prev_record->m_origin ).length_sqr( 2u ) > valve::k_lc_tp_dist ) {
					record->m_broke_lc = true;
					record->m_invalid = true;

					records.clear( );
				}

				record->m_sim_ticks = valve::to_ticks( record->m_sim_time - prev_record->m_sim_time );
			}
			else
				record->m_sim_ticks = valve::to_ticks( record->m_sim_time - record->m_old_sim_time );

			if ( record->m_fake_player )
				record->m_sim_ticks = 1;
		
			record->m_sim_ticks = std::clamp( record->m_sim_ticks, 1, 15 );

			records.emplace_front( record );

			while ( records.size( ) > valve::k_max_player_records )
				records.pop_back( );

			{
				/* here animations stuff, setup bones and etc. */
			}

			entry.m_in_dormancy = true;
		}
	}
}