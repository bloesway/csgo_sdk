#include "../../../csgo.hpp"

namespace hacks {
	void c_anim_system::handle( valve::cs_player_t* player, c_players::entry_t& entry ) {	
		auto& records = entry.m_records;
		if ( records.empty( ) )
			return;

		const auto anim_state = player->anim_state( );
		if ( !anim_state )
			return;

		m_last_anim_player = entry.m_index;

		const auto record = entry.m_records.front( );
		const auto prev_record = entry.m_prev_record;

		auto has_prev_record = false;
		if ( prev_record && prev_record->m_filled )
			has_prev_record = true;

		m_anim_backup.store( player );

		{
			{
				handle_velocity( player, record, prev_record, has_prev_record );

				/*	dont forget to add this check wherever you use fake matrices, 
					as bots just wont have them  */
				if ( !record->m_fake_player ) {
					for ( auto i = -valve::e_rotation_side::left; i <= -valve::e_rotation_side::right; i++ ) {
						update( player, entry, record, prev_record, has_prev_record,
							static_cast< valve::e_rotation_side >( i )
						);

						record->m_fake_layers[ i ] = player->anim_layers( );

						m_anim_backup.restore( player, true, false, false );

						setup_bones( player, entry, record->m_fake_bones[ i ].data( ),
							valve::k_max_bones, valve::e_bone_flags::used_by_hitbox
						);

						m_anim_backup.restore( player, false, true, true );
					}
				}
			}

			if ( !record->m_did_shot ) {
				if ( !record->m_fake_player ) {
					if ( record->m_sim_ticks > 1 ) {
						/* resolver logic */

						update( player, entry, record, prev_record, has_prev_record, record->m_side );
					}
					else
						update( player, entry, record, prev_record, has_prev_record );
				}
				else
					update( player, entry, record, prev_record, has_prev_record );
			}
			else
				update( player, entry, record, prev_record, has_prev_record );
		}

		m_anim_backup.restore( player, true, false, false );

		{
			{
				record->m_pose_params = player->pose_params( );

				record->m_abs_angles = { 0.f, anim_state->m_foot_yaw, 0.f };
			}

			setup_bones( player, entry, record->m_bones.data( ),
				valve::k_max_bones, valve::e_bone_flags::used_by_anything
			);

			std::memcpy( entry.m_bones.data( ), record->m_bones.data( ), sizeof( sdk::mat3x4_t ) * valve::k_max_bones );
		}
	}

	void c_anim_system::update( valve::cs_player_t* player, c_players::entry_t& entry,
		valve::lag_record_t record, valve::lag_record_t prev_record, bool has_prev_record, valve::e_rotation_side side
	) {
		const auto cur_time = valve::g_global_vars->m_cur_time;
		const auto real_time = valve::g_global_vars->m_real_time;

		const auto frame_time = valve::g_global_vars->m_frame_time;
		const auto abs_frame_time = valve::g_global_vars->m_abs_frame_time;

		const auto frame_count = valve::g_global_vars->m_frame_count;
		const auto tick_count = valve::g_global_vars->m_tick_count;

		const auto interp_amt = valve::g_global_vars->m_interp_amt;

		{
			const auto origin = player->origin( );
			const auto abs_origin = player->abs_origin( );

			const auto velocity = player->velocity( );
			const auto abs_velocity = player->abs_velocity( );

			const auto lby = player->lby( );
			const auto duck_amt = player->duck_amt( );

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
				entry.m_first_after_dormant = false;

				const auto land_layer = record->m_layers.at( -valve::e_anim_layer::land_or_climb );
				const auto jump_layer = record->m_layers.at( -valve::e_anim_layer::jump_or_fall );

				auto update_time = record->m_sim_time - valve::g_global_vars->m_interval_per_tick;

				if ( record->m_flags & valve::e_ent_flags::on_ground ) {
					const auto land_seq = player->seq_activity( land_layer.m_seq );
					if ( land_seq == 988
						|| land_seq == 989 ) {
						auto dur = land_layer.m_cycle / land_layer.m_playback_rate;

						auto land_time = record->m_sim_time - dur;
						if ( land_time == update_time ) {
							anim_state->m_on_ground = true;
							anim_state->m_landing = true;
						}
						else if ( land_time - valve::g_global_vars->m_interval_per_tick == update_time ) {
							anim_state->m_on_ground = false;
							anim_state->m_landing = false;
						}

						auto time_in_air = jump_layer.m_cycle - land_layer.m_cycle;
						if ( time_in_air < 0.f )
							time_in_air += 1.f;

						anim_state->m_time_since_in_air = time_in_air;

						if ( land_time < update_time && land_time > anim_state->m_last_update_time )
							update_time = land_time;
					}
				}
				else {
					const auto jump_seq = player->seq_activity( jump_layer.m_seq );
					if ( jump_seq == 985 ) {
						auto time_in_air = jump_layer.m_cycle / jump_layer.m_playback_rate;

						auto jump_time = record->m_sim_time - time_in_air;
						if ( jump_time <= update_time )
							anim_state->m_on_ground = false;
						else
							anim_state->m_on_ground = true;

						if ( jump_time < update_time && jump_time > anim_state->m_last_update_time )
							update_time = jump_time;

						anim_state->m_time_since_in_air = time_in_air - valve::g_global_vars->m_interval_per_tick;
						anim_state->m_landing = false;
					}
				}

				anim_state->m_last_update_time = update_time;
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

				{
					const auto land_layer = record->m_layers.at( -valve::e_anim_layer::land_or_climb );
					const auto jump_layer = record->m_layers.at( -valve::e_anim_layer::jump_or_fall );

					const auto prev_land_layer = prev_record->m_layers.at( -valve::e_anim_layer::land_or_climb );
					const auto prev_jump_layer = prev_record->m_layers.at( -valve::e_anim_layer::jump_or_fall );

					const auto land_seq = player->seq_activity( land_layer.m_seq );
					const auto jump_seq = player->seq_activity( jump_layer.m_seq );

					if ( land_seq == 988
						|| land_seq == 989 ) {
						if ( land_layer.m_cycle > prev_land_layer.m_cycle ) {
							auto dur = land_layer.m_cycle / land_layer.m_playback_rate;
							if ( dur ) {
								record->m_seq_type = valve::e_seq_type::on_land;
								record->m_seq_tick = valve::to_ticks( record->m_sim_time - dur ) + 1;

								auto dur_in_air = jump_layer.m_cycle - land_layer.m_cycle;
								if ( dur_in_air < 0.f )
									dur_in_air += 1.f;

								record->m_time_in_air = dur_in_air / jump_layer.m_playback_rate;
							}
						}
					}

					if ( jump_seq == 985 ) {
						if ( jump_layer.m_weight > 0.f
							&& prev_jump_layer.m_weight > 0.f ) {
							if ( jump_layer.m_cycle != prev_jump_layer.m_cycle ) {
								record->m_time_in_air = jump_layer.m_cycle / jump_layer.m_playback_rate;

								if ( record->m_time_in_air ) {
									record->m_seq_type = valve::e_seq_type::on_jump;
									record->m_seq_tick = valve::to_ticks( record->m_sim_time - record->m_time_in_air ) + 1;
								}
							}
						}
					}
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

					player->lby( ) = prev_record->m_lby;

					if ( record->m_sim_time == sim_time ) {
						player->origin( ) = record->m_origin;
						player->set_abs_origin( record->m_origin );

						player->velocity( ) = record->m_anim_velocity;
						player->abs_velocity( ) = record->m_anim_velocity;
					}
					else {
						if ( record->m_seq_type != valve::e_seq_type::none ) {
							if ( record->m_seq_tick == sim_tick ) {
								auto seq_layer = valve::e_anim_layer::land_or_climb;
								if ( record->m_seq_type == valve::e_seq_type::on_jump )
									seq_layer = valve::e_anim_layer::jump_or_fall;

								auto& layer = player->anim_layers( ).at( -seq_layer );

								layer.m_cycle = 0.f;
								layer.m_weight = 0.f;

								layer.m_playback_rate = player->layer_seq_cycle_rate( &record->m_layers.at( -seq_layer ),
									record->m_layers.at( -seq_layer ).m_seq
								);
							}
							else if ( record->m_seq_tick > sim_tick ) {
								if ( record->m_seq_type == valve::e_seq_type::on_jump )
									player->flags( ) &= ~valve::e_ent_flags::on_ground;
								else if ( record->m_seq_type == valve::e_seq_type::on_land )
									player->flags( ) |= valve::e_ent_flags::on_ground;
							}
						}

						const auto lerp_origin = sdk::lerp( prev_record->m_origin, record->m_origin,
							i, record->m_sim_ticks
						);

						const auto lerp_velocity = sdk::lerp( prev_record->m_velocity, record->m_velocity,
							i, record->m_sim_ticks
						);

						const auto lerp_duck_amt = sdk::lerp( prev_record->m_duck_amt, record->m_duck_amt,
							i, record->m_sim_ticks
						);

						player->origin( ) = lerp_origin;
						player->set_abs_origin( lerp_origin );

						player->velocity( ) = lerp_velocity;
						player->abs_velocity( ) = lerp_velocity;

						player->duck_amt( ) = lerp_duck_amt;
					}

					switch ( side ) {
						case valve::e_rotation_side::left:
							{
								anim_state->m_foot_yaw = sdk::normalize_yaw( 
									record->m_eye_angles.y( ) - record->m_desync_delta 
								);
							} break;

						case valve::e_rotation_side::right:
							{
								anim_state->m_foot_yaw = sdk::normalize_yaw( 
									record->m_eye_angles.y( ) + record->m_desync_delta 
								);
							} break;

						default:
							break;
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

				player->velocity( ) = record->m_anim_velocity;
				player->abs_velocity( ) = record->m_anim_velocity;

				switch ( side ) {
					case valve::e_rotation_side::left:
						{
							anim_state->m_foot_yaw = sdk::normalize_yaw( 
								record->m_eye_angles.y( ) - record->m_desync_delta 
							);
						} break;

					case valve::e_rotation_side::right:
						{
							anim_state->m_foot_yaw = sdk::normalize_yaw(
								record->m_eye_angles.y( ) + record->m_desync_delta 
							);
						} break;

					default:
						break;
				}

				update_client_side_anims( player, entry );
			}

			player->origin( ) = origin;
			player->set_abs_origin( abs_origin );

			player->velocity( ) = velocity;
			player->abs_velocity( ) = abs_velocity;

			player->lby( ) = lby;
			player->duck_amt( ) = duck_amt;

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

	void c_anim_system::handle_velocity( valve::cs_player_t* player,
		valve::lag_record_t record, valve::lag_record_t prev_record, bool has_prev_record
	) {
		const auto move_layer = record->m_layers.at( -valve::e_anim_layer::movement_move );
		if ( move_layer.m_weight <= 0.f
			|| move_layer.m_playback_rate < 0.00001f ) {
			record->m_anim_velocity = {};

			return;
		}

		if ( !prev_record 
			|| !has_prev_record )
			return;
	
		auto anim_speed = 0.f;

		const auto time_diff = std::max( valve::g_global_vars->m_interval_per_tick, valve::to_time( record->m_sim_ticks ) );
		if ( time_diff > 0.f ) {
			const auto origin_diff = ( record->m_origin - prev_record->m_origin ) * ( 1.f / time_diff );
			if ( origin_diff > 0.f )
				anim_speed = origin_diff.length( 2u );

			const auto alive_loop_layer = record->m_layers.at( -valve::e_anim_layer::alive_loop );
			const auto prev_alive_loop_layer = prev_record->m_layers.at( -valve::e_anim_layer::alive_loop );

			if ( record->m_flags & valve::e_ent_flags::on_ground
				&& prev_record->m_flags & valve::e_ent_flags::on_ground ) {
				if ( alive_loop_layer.m_cycle > prev_alive_loop_layer.m_cycle ) {
					const auto alive_loop_weight = alive_loop_layer.m_weight;
					if ( alive_loop_weight > 0.f && alive_loop_weight < 0.95f ) {
						const auto vel_modifier = 0.35f * ( 1.f - alive_loop_weight );
						if ( vel_modifier > 0.f && vel_modifier < 1.f ) {
							auto vel_bound = 1.f;

							if ( record->m_walking )
								vel_bound = 0.52f;
							else if ( record->m_flags & valve::e_ent_flags::ducking )
								vel_bound = 0.34f;

							anim_speed = ( record->m_max_speed * vel_bound ) * ( vel_modifier + 0.55f );
						}
					}
				}

				record->m_anim_velocity.z( ) = 0.f;
			}
			else {
				const auto sv_gravity = g_ctx->cvars( ).sv_gravity;
				const auto air_time = std::clamp( time_diff, valve::g_global_vars->m_interval_per_tick, 1.f );

				record->m_anim_velocity.z( ) -= sv_gravity->get_float( ) * air_time * 0.5f;
			}
		}

		if ( anim_speed > 0.f ) {
			anim_speed /= record->m_anim_velocity.length( 2u );

			record->m_anim_velocity.x( ) *= anim_speed;
			record->m_anim_velocity.y( ) *= anim_speed;
		}
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