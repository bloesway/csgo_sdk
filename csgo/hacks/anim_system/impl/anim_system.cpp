#include "../../../csgo.hpp"

namespace hacks {
	void c_anim_system::handle( valve::cs_player_t* player, c_players::entry_t& entry ) {
		const auto anim_state = player->anim_state( );
		if ( !anim_state )
			return;

		auto& records = entry.m_records;
		if ( records.empty( ) )
			return;

		const auto record = entry.m_records.front( );
		const auto prev_record = entry.m_prev_record;

		auto has_prev_record = false;
		if ( prev_record && prev_record->m_filled )
			has_prev_record = true;

		{
			m_anim_backup.store( player );

			{
				/* pre update things */
				{

				}

				update( player, entry, record.get( ), prev_record.get( ), has_prev_record );
			}

			m_anim_backup.restore( player, false, false );
		}

		/* set correct values to player after update */
		{
			record->m_pose_params = player->pose_params( );
		}

		/* here setup bones and other u needed things */
		{
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
		const auto frame_time = valve::g_global_vars->m_frame_time;

		const auto abs_origin = player->abs_origin( );

		const auto velocity = player->velocity( );
		const auto abs_velocity = player->abs_velocity( );

		const auto eflags = player->eflags( );

		valve::g_global_vars->m_frame_time = valve::g_global_vars->m_interval_per_tick;

		{
			player->update_collision_bounds( );

			record->m_mins = player->obb_min( );
			record->m_maxs = player->obb_max( );
		}

		if ( entry.m_first_after_dormant ) {
			/* handle first after dormant */

			entry.m_first_after_dormant = false;
		}

		{
			player->eflags( ) &= ~valve::e_eflags::dirty_abs_velocity;

			if ( prev_record && has_prev_record && !record->m_fake_player ) {
				player->anim_layers( ) = prev_record->m_layers;

				{
					for ( auto i = 1; i <= record->m_sim_ticks; i++ ) {
						const auto sim_time = prev_record->m_sim_time + valve::to_time( i );
						const auto sim_tick = valve::to_ticks( sim_time );

						valve::g_global_vars->m_cur_time = sim_time;

						if ( record->m_sim_time != sim_time ) {
							player->set_abs_origin( sdk::lerp(
								prev_record->m_origin, record->m_origin, i, record->m_sim_ticks
							) );

							const auto lerp_velocity = sdk::lerp( prev_record->m_velocity,
								record->m_velocity, i, record->m_sim_ticks
							);

							player->velocity( ) = lerp_velocity;
							player->abs_velocity( ) = lerp_velocity;
						}
						else {
							player->set_abs_origin( record->m_origin );

							player->velocity( ) = record->m_velocity;
							player->abs_velocity( ) = record->m_velocity;
						}

						update_client_side_anims( player, entry );
					}
				}
			}
			else {
				player->anim_layers( ) = record->m_layers;

				player->set_abs_origin( record->m_origin );

				{
					valve::g_global_vars->m_cur_time = record->m_sim_time;

					{
						player->velocity( ) = record->m_velocity;
						player->abs_velocity( ) = record->m_velocity;

						update_client_side_anims( player, entry );
					}
				}
			}
		}

		player->set_abs_origin( abs_origin );

		player->velocity( ) = velocity;
		player->abs_velocity( ) = abs_velocity;

		player->eflags( ) = eflags;

		valve::g_global_vars->m_cur_time = cur_time;
		valve::g_global_vars->m_frame_time = frame_time;
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
		const auto frame_time = valve::g_global_vars->m_frame_time;

		const auto frame_count = valve::g_global_vars->m_frame_count;

		const auto abs_origin = player->abs_origin( );

		const auto effects = player->effects( );
		const auto client_effects = player->client_effects( );

		const auto jiggle_bones = player->jiggle_bones( );

		const auto occlusion_frame_count = player->occlusion_frame_count( );
		const auto occlusion_mask = player->occlusion_mask( );

		valve::g_global_vars->m_cur_time = player->sim_time( );
		valve::g_global_vars->m_frame_time = valve::g_global_vars->m_interval_per_tick;

		valve::g_global_vars->m_frame_count = std::numeric_limits< int >::min( );

		{
			player->set_abs_origin( player->origin( ) );

			player->invalidate_bone_cache( );

			{
				player->effects( ) &= ~valve::e_effects::no_interp;
				player->client_effects( ) |= 2;

				player->jiggle_bones( ) = false;

				player->occlusion_frame_count( ) = -1;
				player->occlusion_mask( ) &= ~2;
			}

			player->setup_bones( bones, bones_count, flags, player->sim_time( ) );
		}

		player->set_abs_origin( abs_origin );

		player->effects( ) = effects;
		player->client_effects( ) = client_effects;

		player->jiggle_bones( ) = jiggle_bones;

		player->occlusion_frame_count( ) = occlusion_frame_count;
		player->occlusion_mask( ) = occlusion_mask;

		valve::g_global_vars->m_cur_time = cur_time;
		valve::g_global_vars->m_frame_time = frame_time;

		valve::g_global_vars->m_frame_count = frame_count;

		entry.m_setup_bones = false;
	}

	bool c_anim_system::on_setup_bones( c_players::entry_t& entry, sdk::mat3x4_t* bones, int bones_count ) {
		std::memcpy( bones, entry.m_bones.data( ), sizeof( sdk::mat3x4_t ) * bones_count );

		return true;
	}
}