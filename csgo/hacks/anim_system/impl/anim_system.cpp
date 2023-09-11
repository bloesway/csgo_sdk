#include "../../../csgo.hpp"

namespace hacks {
	void c_anim_system::handle( valve::cs_player_t* player, c_players::entry_t& entry ) {
		auto& records = entry.m_records;
		if ( records.empty( ) )
			return;

		const auto anim_state = player->anim_state( );
		if ( !anim_state )
			return;

		const auto record = entry.m_records.front( );
		const auto prev_record = entry.m_prev_record;

		auto has_prev_record = false;
		if ( prev_record && prev_record->m_filled )
			has_prev_record = true;

		m_anim_backup.store( player );

		{
			update( player, entry, record.get( ), prev_record.get( ), has_prev_record );
		}

		m_anim_backup.restore( player, false, false );

		{
			record->m_pose_params = player->pose_params( );

			setup_bones( player, entry, record->m_bones.data( ),
				valve::k_max_bones, valve::e_bone_flags::used_by_anything
			);

			std::memcpy( entry.m_bones.data( ), record->m_bones.data( ), sizeof( sdk::mat3x4_t ) * valve::k_max_bones );
		}
	}

	void c_anim_system::update( valve::cs_player_t* player, c_players::entry_t& entry,
		valve::player_record_t* record, valve::player_record_t* prev_record, bool has_prev_record
	) {
		const auto cur_time = valve::g_global_vars->m_cur_time;
		const auto real_time = valve::g_global_vars->m_real_time;

		const auto frame_time = valve::g_global_vars->m_frame_time;
		const auto abs_frame_time = valve::g_global_vars->m_abs_frame_time;

		const auto frame_count = valve::g_global_vars->m_frame_count;
		const auto tick_count = valve::g_global_vars->m_tick_count;

		const auto interp_amt = valve::g_global_vars->m_interp_amt;

		{
			const auto abs_origin = player->abs_origin( );

			const auto velocity = player->velocity( );
			const auto abs_velocity = player->abs_velocity( );

			const auto ent_flags = player->eflags( );

			{
				valve::g_global_vars->m_frame_time = valve::g_global_vars->m_interval_per_tick;
				valve::g_global_vars->m_abs_frame_time = valve::g_global_vars->m_interval_per_tick;

				valve::g_global_vars->m_interp_amt = 0.f;
			}

			{
				player->update_collision_bounds( );

				record->m_mins = player->obb_min( );
				record->m_maxs = player->obb_max( );
			}

			const auto anim_state = player->anim_state( );

			if ( entry.m_first_after_dormant ) {
				/* handle first after dormant */

				entry.m_first_after_dormant = false;
			}

			player->eflags( ) &= ~valve::e_eflags::dirty_abs_velocity;

			if ( prev_record && has_prev_record && !record->m_fake_player ) {
				{
					const auto movement_strafe_layer = prev_record->m_layers.at( -valve::e_anim_layer::movement_strafe_change );
					const auto movement_move_layer = prev_record->m_layers.at( -valve::e_anim_layer::movement_move );

					anim_state->m_strafe_sequence = movement_strafe_layer.m_seq;

					anim_state->m_strafe_cycle = movement_strafe_layer.m_cycle;
					anim_state->m_strafe_weight = movement_strafe_layer.m_weight;

					anim_state->m_primary_cycle = movement_move_layer.m_cycle;
					anim_state->m_move_weight = movement_move_layer.m_weight;

					player->anim_layers( ) = prev_record->m_layers;
				}

				for ( auto i = 1; i <= record->m_sim_ticks; i++ ) {
					const auto sim_time = prev_record->m_sim_time + valve::to_time( i );
					const auto sim_tick = valve::to_ticks( sim_time );

					{
						valve::g_global_vars->m_cur_time = sim_time;
						valve::g_global_vars->m_real_time = sim_time;

						valve::g_global_vars->m_frame_count = sim_tick;
						valve::g_global_vars->m_tick_count = sim_tick;
					}

					if ( record->m_sim_time == sim_time ) {
						player->set_abs_origin( record->m_origin );

						player->velocity( ) = record->m_velocity;
						player->abs_velocity( ) = record->m_velocity;
					}
					else {
						const auto lerp_origin = sdk::lerp( prev_record->m_origin, record->m_origin,
							i, record->m_sim_ticks
						);

						const auto lerp_velocity = sdk::lerp( prev_record->m_velocity, record->m_velocity,
							i, record->m_sim_ticks
						);

						const auto lerp_duck_amt = sdk::lerp( prev_record->m_duck_amt, record->m_duck_amt,
							i, record->m_sim_ticks
						);

						player->abs_origin( ) = lerp_origin;

						player->velocity( ) = lerp_velocity;
						player->abs_velocity( ) = lerp_velocity;

						player->duck_amt( ) = lerp_duck_amt;
					}

					update_client_side_anims( player, entry );
				}
			}
			else {
				{
					const auto movement_strafe_layer = record->m_layers.at( -valve::e_anim_layer::movement_strafe_change );
					const auto movement_move_layer = record->m_layers.at( -valve::e_anim_layer::movement_move );

					anim_state->m_strafe_sequence = movement_strafe_layer.m_seq;

					anim_state->m_strafe_cycle = movement_strafe_layer.m_cycle;
					anim_state->m_strafe_weight = movement_strafe_layer.m_weight;

					anim_state->m_primary_cycle = movement_move_layer.m_cycle;
					anim_state->m_move_weight = movement_move_layer.m_weight;

					player->anim_layers( ) = record->m_layers;
				}

				player->set_abs_origin( record->m_origin );

				{
					valve::g_global_vars->m_cur_time = record->m_sim_time;
					valve::g_global_vars->m_real_time = record->m_sim_time;

					valve::g_global_vars->m_frame_count = valve::to_ticks( record->m_sim_time );
					valve::g_global_vars->m_tick_count = valve::to_ticks( record->m_sim_time );
				}

				player->velocity( ) = record->m_velocity;
				player->abs_velocity( ) = record->m_velocity;

				update_client_side_anims( player, entry );
			}

			player->set_abs_origin( abs_origin );

			player->velocity( ) = velocity;
			player->abs_velocity( ) = abs_velocity;

			player->eflags( ) = ent_flags;
		}

		valve::g_global_vars->m_cur_time = cur_time;
		valve::g_global_vars->m_real_time = real_time;

		valve::g_global_vars->m_frame_time = frame_time;
		valve::g_global_vars->m_abs_frame_time = abs_frame_time;

		valve::g_global_vars->m_frame_count = frame_count;
		valve::g_global_vars->m_tick_count = tick_count;

		valve::g_global_vars->m_interp_amt = interp_amt;
	}

	void c_anim_system::update_client_side_anims( valve::cs_player_t* player,
		c_players::entry_t& entry
	) {
		entry.m_update_anims = true;

		{
			const auto anim_state = player->anim_state( );

			if ( valve::g_global_vars->m_frame_count == anim_state->m_last_update_frame )
				anim_state->m_last_update_frame -= 1;

			if ( valve::g_global_vars->m_cur_time == anim_state->m_last_update_time )
				anim_state->m_last_update_time += valve::g_global_vars->m_interval_per_tick;

			auto& anim_layers = player->anim_layers( );
			for ( auto& layer : anim_layers )
				layer.m_owner = player;
		}

		player->client_side_anim( ) = true;
		player->update_client_side_anims( );
		player->client_side_anim( ) = false;

		entry.m_update_anims = false;
	}

	void c_anim_system::setup_bones( valve::cs_player_t* player, c_players::entry_t& entry,
		sdk::mat3x4_t* bones, const int bones_count, const valve::e_bone_flags flags
	) {
		entry.m_setup_bones = true;

		const auto cur_time = valve::g_global_vars->m_cur_time;
		const auto real_time = valve::g_global_vars->m_real_time;

		const auto frame_time = valve::g_global_vars->m_frame_time;
		const auto abs_frame_time = valve::g_global_vars->m_abs_frame_time;

		const auto frame_count = valve::g_global_vars->m_frame_count;
		const auto tick_count = valve::g_global_vars->m_tick_count;

		const auto interp_amt = valve::g_global_vars->m_interp_amt;

		{
			const auto abs_origin = player->abs_origin( );

			const auto effects = player->effects( );
			const auto client_effects = player->client_effects( );

			const auto jiggle_bones = player->jiggle_bones( );

			const auto occlusion_frame_count = player->occlusion_frame_count( );
			const auto occlusion_mask = player->occlusion_mask( );

			{
				valve::g_global_vars->m_cur_time = player->sim_time( );
				valve::g_global_vars->m_real_time = player->sim_time( );

				valve::g_global_vars->m_frame_time = valve::g_global_vars->m_interval_per_tick;
				valve::g_global_vars->m_abs_frame_time = valve::g_global_vars->m_interval_per_tick;

				valve::g_global_vars->m_frame_count = std::numeric_limits< int >::min( );
				valve::g_global_vars->m_tick_count = valve::to_ticks( player->sim_time( ) );

				valve::g_global_vars->m_interp_amt = 0.f;
			}

			{
				player->set_abs_origin( player->origin( ) );

				{
					player->effects( ) &= ~valve::e_effects::no_interp;
					player->client_effects( ) |= 2;

					player->jiggle_bones( ) = false;

					player->occlusion_frame_count( ) = -1;
					player->occlusion_mask( ) &= ~2;
				}
			}

			player->invalidate_bone_cache( );

			player->setup_bones( bones, bones_count, flags, valve::g_global_vars->m_cur_time );

			player->set_abs_origin( abs_origin );

			player->effects( ) = effects;
			player->client_effects( ) = client_effects;

			player->jiggle_bones( ) = jiggle_bones;

			player->occlusion_frame_count( ) = occlusion_frame_count;
			player->occlusion_mask( ) = occlusion_mask;
		}

		valve::g_global_vars->m_cur_time = cur_time;
		valve::g_global_vars->m_real_time = real_time;

		valve::g_global_vars->m_frame_time = frame_time;
		valve::g_global_vars->m_abs_frame_time = abs_frame_time;

		valve::g_global_vars->m_frame_count = frame_count;
		valve::g_global_vars->m_tick_count = tick_count;

		valve::g_global_vars->m_interp_amt = interp_amt;

		entry.m_setup_bones = false;
	}

	bool c_anim_system::on_setup_bones( c_players::entry_t& entry, sdk::mat3x4_t* bones, int bones_count ) {
		std::memcpy( bones, entry.m_bones.data( ), sizeof( sdk::mat3x4_t ) * bones_count );

		return true;
	}
}