#include "../../../csgo.hpp"

namespace hacks {
	void c_lag_comp::handle( ) {
		auto& players = g_players->get( );

		for ( auto& it : players ) {
			auto& entry = it.second;

			const auto player = entry.m_player;
			if ( !player || !player->alive( )
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

			if ( records.size( ) > valve::k_max_player_records )
				records.pop_back( );

			g_anim_system->handle( player, entry );

			entry.m_in_dormancy = true;
		}
	}
	
	void c_lag_comp::update_player( valve::cs_player_t* player, valve::lag_record_t record ) {
		player->sim_time( ) = record->m_sim_time;

		player->origin( ) = record->m_origin;
		player->eye_angles( ) = record->m_eye_angles;

		player->set_abs_origin( record->m_origin );
		player->set_abs_angles( record->m_abs_angles );

		player->flags( ) = record->m_flags;

		player->anim_layers( ) = record->m_layers;
		player->pose_params( ) = record->m_pose_params;

		{
			const auto cur_time = valve::g_global_vars->m_cur_time;
			const auto real_time = valve::g_global_vars->m_real_time;

			{
				valve::g_global_vars->m_cur_time = record->m_sim_time;
				valve::g_global_vars->m_real_time = record->m_sim_time;

				player->set_collision_bounds( record->m_mins, record->m_maxs );
			}

			valve::g_global_vars->m_cur_time = cur_time;
			valve::g_global_vars->m_real_time = real_time;
		}

		auto& bones = player->bones( );
		for ( auto i = 0; i < bones.size( ); i++ )
			bones.at( i ) = record->m_bones.at( i );

		player->invalidate_bone_cache( );
	}

	void c_lag_comp::store_players( ) {
		auto& players = g_players->get( );

		for ( auto& it : players ) {
			auto& entry = it.second;

			const auto player = entry.m_player;
			if ( !player || !player->alive( )
				|| player->friendly( g_local_player->self( ) ) )
				continue;

			if ( player->networkable( )->dormant( ) )
				continue;

			auto& backup_data = m_backup_players[ entry.m_index ];
			if ( !backup_data )
				backup_data = std::make_shared< valve::player_record_t >( );

			{
				backup_data->m_player = player;

				backup_data->m_sim_time = player->sim_time( );

				backup_data->m_origin = player->origin( );
				backup_data->m_eye_angles = player->eye_angles( );

				backup_data->m_abs_origin = player->abs_origin( );
				backup_data->m_abs_angles = player->abs_angles( );

				backup_data->m_flags = player->flags( );

				backup_data->m_layers = player->anim_layers( );
				backup_data->m_pose_params = player->pose_params( );

				backup_data->m_mins = player->obb_min( );
				backup_data->m_maxs = player->obb_max( );

				backup_data->m_collision_change_time = player->collision_change_time( );
				backup_data->m_collision_change_origin_z = player->collision_change_origin_z( );

				const auto bones = player->bones( );
				for ( auto i = 0; i < bones.size( ); i++ )
					backup_data->m_bones.at( i ) = bones.at( i );
			}

			backup_data->m_filled = true;
		}
	}

	void c_lag_comp::restore_players( ) {
		auto& players = g_players->get( );

		for ( auto& it : players ) {
			auto& entry = it.second;

			const auto player = entry.m_player;
			if ( !player || !player->alive( )
				|| player->friendly( g_local_player->self( ) ) )
				continue;

			if ( player->networkable( )->dormant( ) )
				continue;

			auto& backup_data = m_backup_players[ entry.m_index ];
			if ( !backup_data
				|| !backup_data->m_filled )
				continue;

			if ( backup_data->m_player != player )
				continue;

			{
				player->sim_time( ) = backup_data->m_sim_time;

				player->origin( ) = backup_data->m_origin;
				player->eye_angles( ) = backup_data->m_eye_angles;

				player->set_abs_origin( backup_data->m_abs_origin );
				player->set_abs_angles( backup_data->m_abs_angles );

				player->flags( ) = backup_data->m_flags;

				player->anim_layers( ) = backup_data->m_layers;
				player->pose_params( ) = backup_data->m_pose_params;

				{
					player->obb_min( ) = backup_data->m_mins;
					player->obb_max( ) = backup_data->m_maxs;

					player->collision_change_time( ) = backup_data->m_collision_change_time;
					player->collision_change_origin_z( ) = backup_data->m_collision_change_origin_z;
				}

				auto& bones = player->bones( );
				for ( auto i = 0; i < bones.size( ); i++ )
					bones.at( i ) = backup_data->m_bones.at( i );

				player->invalidate_bone_cache( );
			}

			backup_data->m_filled = false;
		}
	}

	bool c_lag_comp::is_valid( valve::lag_record_t record ) {
		if ( !record
			|| !record->m_filled )
			return false;

		if ( record->m_broke_lc 
			|| record->m_invalid )
			return false;

		auto delta_time = std::min( g_networking->m_latency + g_networking->m_lerp_amt, 0.2f );

		return std::abs( delta_time - (
			valve::g_global_vars->m_cur_time - record->m_sim_time ) 
		) < 0.2f;
	}
}