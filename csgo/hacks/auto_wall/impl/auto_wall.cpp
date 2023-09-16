#include "../../../csgo.hpp"

namespace hacks {
	float c_auto_wall::get_dmg( valve::cs_player_t* player, const sdk::vec3_t& point, projectile_t* projectile ) {
		const auto pos = player->eye_pos( );

		projectile_t projectile_data{};

		projectile_data.m_start = pos;
		projectile_data.m_end = ( point - pos ).normalized( );

		if ( auto weapon = player->weapon( ); !weapon 
			|| !simulate_projectile( player, weapon, projectile_data ) )
			return -1.f;

		if ( projectile != nullptr )
			*projectile = projectile_data;

		return projectile_data.m_dmg;
	}

	bool c_auto_wall::simulate_projectile( valve::cs_player_t* player, valve::base_weapon_t* weapon, projectile_t& data ) {
		const auto wpn_info = weapon->info( );
		if ( !wpn_info )
			return false;

		auto max_range = wpn_info->m_range;

		data.m_pen_count = 4;
		data.m_dmg = static_cast< float >( wpn_info->m_dmg );

		auto trace_length = 0.f;
		valve::trace_filter_simple_t filter( player );

		while ( data.m_pen_count > 0 && data.m_dmg >= 1.f ) {
			max_range -= trace_length;

			const auto end = data.m_start + data.m_end * max_range;

			valve::ray_t ray( data.m_start, end );
			valve::g_engine_trace->trace_ray(
				ray, static_cast< valve::e_mask >( -valve::e_mask::shot_hull | ( 1 << 30 ) ), &filter, &data.m_trace
			);

			clip_trace_to_players(
				data.m_start, end + data.m_end * 40.f,
				static_cast< valve::e_mask >( -valve::e_mask::shot_hull | ( 1 << 30 ) ),
				&filter, &data.m_trace, -60.f, 60.f
			);

			const auto enter_surface_data = valve::g_phys_props->get_surface_data( data.m_trace.m_surface.m_surface_props );
			const auto enter_pen_modifier = enter_surface_data->m_game.m_pen_modifier;

			if ( data.m_trace.m_frac == 1.f )
				break;

			trace_length += data.m_trace.m_frac * max_range;
			data.m_dmg *= std::powf( wpn_info->m_range_modifier, trace_length / 500.f );

			if ( trace_length > 3000.f || enter_pen_modifier < 0.1f )
				break;

			if ( data.m_trace.m_hitgroup != valve::e_hitgroup::generic
				&& data.m_trace.m_hitgroup != valve::e_hitgroup::gear
				&& !player->friendly( reinterpret_cast< valve::cs_player_t* >( data.m_trace.m_entity ) ) ) {
				scale_dmg( data.m_trace.m_hitgroup, reinterpret_cast< valve::cs_player_t* >(
					data.m_trace.m_entity
				), wpn_info->m_armor_ratio, wpn_info->m_headshot_multiplier, data.m_dmg );
				return true;
			}

			if ( handle_bullet_penetration( wpn_info, enter_surface_data, data ) )
				break;
		}

		return false;
	}

	void c_auto_wall::scale_dmg( valve::e_hitgroup hit_group, valve::cs_player_t* player,
		float wpn_armor_ratio, float wpn_headshot_multiplier, float& dmg
	) const {
		if ( hit_group != valve::e_hitgroup::head && g_ctx->cvars( ).mp_damage_headshot_only->get_int( ) ) {
			dmg = 0.f;

			return;
		}

		auto head_dmg_scale = ( player->team( ) == valve::e_team::ct ? g_ctx->cvars().mp_damage_scale_ct_head->get_float( ) 
			: g_ctx->cvars( ).mp_damage_scale_t_head->get_float( ) );

		const auto body_dmg_scale = ( player->team( ) == valve::e_team::ct ? g_ctx->cvars( ).mp_damage_scale_ct_body->get_float( ) 
			: g_ctx->cvars( ).mp_damage_scale_t_body->get_float( ) );

		const auto heavy_armor = player->heavy_armor( );
		if ( heavy_armor )
			head_dmg_scale *= 0.5f;

		switch ( hit_group ) {
			case valve::e_hitgroup::head:
				dmg *= wpn_headshot_multiplier * head_dmg_scale;
				break;

			case valve::e_hitgroup::chest:
			case valve::e_hitgroup::left_arm:
			case valve::e_hitgroup::right_arm:
			case valve::e_hitgroup::neck:
				dmg *= body_dmg_scale;
				break;

			case valve::e_hitgroup::stomach:
				dmg *= 1.25f * body_dmg_scale;
				break;

			case valve::e_hitgroup::left_leg:
			case valve::e_hitgroup::right_leg:
				dmg *= 0.75f * body_dmg_scale;
				break;

			default:
				break;
		}

		if ( player->armored( hit_group ) ) {
			const auto	armor = player->armor_value( );
			auto		heavy_armor_bonus = 1.f, 
						armor_bonus = 0.5f, 
						armor_ratio = wpn_armor_ratio * 0.5f;

			if ( heavy_armor ) {
				heavy_armor_bonus = 0.25f;
				armor_bonus = 0.33f;
				armor_ratio *= 0.20f;
			}

			auto dmg_to_health = dmg * armor_ratio;
			if ( const auto dmg_to_armor = ( dmg - dmg_to_health ) * ( heavy_armor_bonus * armor_bonus );
				dmg_to_armor > static_cast< float >( armor ) )
				dmg_to_health = dmg - static_cast< float >( armor ) / armor_bonus;

			dmg = dmg_to_health;
		}
	}

	void c_auto_wall::clip_trace_to_players( const sdk::vec3_t& abs_start, const sdk::vec3_t& abs_end, 
		const valve::e_mask mask, valve::trace_filter_simple_t* filter, valve::trace_t* trace, 
		const float min_range, const float max_range 
	) const {
		valve::trace_t l_trace{};

		auto smallest_frac = trace->m_frac;

		const valve::ray_t ray( abs_start, abs_end );

		for ( auto it : g_players->get( ) ) {
			auto entry = it.second;
			if ( !entry.m_player
				|| !entry.m_player->alive( ) || entry.m_player->networkable( )->dormant( ) )
				continue;

			if ( filter && !filter->should_hit_entity( entry.m_player, mask ) )
				continue;

			const auto pos = entry.m_player->world_space_center( );

			const auto to = pos - abs_start;

			auto dir = abs_end - abs_start;

			const auto length = dir.normalize_in_place( );
			const auto range_along = dir.dot( to );

			auto range = 0.f;
			if ( range_along < 0.f )
				range = -to.length( );
			else if ( range_along > length )
				range = -( pos - abs_end ).length( );
			else
				range = ( pos - ( dir * range_along + abs_start ) ).length( );

			if ( range < min_range || range > max_range )
				continue;

			valve::g_engine_trace->clip_ray_to_entity( 
				ray, static_cast< valve::e_mask >( -mask | ( 1 << 30 ) ), entry.m_player, &l_trace 
			);

			if ( l_trace.m_frac < smallest_frac ) {
				*trace = l_trace;
				smallest_frac = l_trace.m_frac;
			}
		}
	}

	bool c_auto_wall::trace_to_exit( const valve::trace_t& enter_trace, valve::trace_t& exit_trace,
		const sdk::vec3_t& pos, const sdk::vec3_t& dir, const valve::base_entity_t* clip_player
	) const {
		auto distance = 0.f;
		auto start_contents = valve::e_mask( );

		while ( distance <= 90.f ) {
			distance += 4.f;

			auto end = pos + dir * distance;

			if ( !( -start_contents ) )
				start_contents = valve::g_engine_trace->get_point_contents( end,
					static_cast< valve::e_mask >( -valve::e_mask::shot_hull | ( 1 << 30 ) ), nullptr
				);

			if ( const auto current_points = valve::g_engine_trace->get_point_contents( 
					end, static_cast< valve::e_mask >( -valve::e_mask::shot_hull | ( 1 << 30 ) ), nullptr 
				); !( current_points & valve::e_mask::shot_hull ) 
				|| ( ( current_points & ( 1 << 30 ) ) 
				&& current_points != start_contents ) ) {
				const auto start = end - ( dir * 4.f );

				valve::ray_t ray_world( end, start );
				valve::g_engine_trace->trace_ray( 
					ray_world, static_cast< valve::e_mask >( -valve::e_mask::shot_hull | ( 1 << 30 ) ), nullptr, &exit_trace 
				);

				if ( g_ctx->cvars().sv_clip_penetration_traces_to_players->get_int( ) ) {
					valve::trace_filter_simple_t filter( clip_player );

					clip_trace_to_players(
						start, end, static_cast< valve::e_mask >( -valve::e_mask::shot_hull | ( 1 << 30 ) ),
						&filter, &exit_trace, -60.f, 60.f
					);
				}

				if ( exit_trace.m_start_solid && ( exit_trace.m_surface.m_flags & 32768 ) ) {
					valve::ray_t ray( end, pos );
					valve::trace_filter_skip_two_entities_t filter( exit_trace.m_entity, clip_player );

					valve::g_engine_trace->trace_ray( ray, valve::e_mask::shot_hull, &filter, &exit_trace );

					if ( exit_trace.hit( ) && !exit_trace.m_start_solid ) {
						end = exit_trace.m_end;

						return true;
					}
				}
				else if ( !exit_trace.hit( ) || exit_trace.m_start_solid ) {
					if ( enter_trace.m_entity != nullptr && enter_trace.m_entity->networkable( )->index( ) != 0 &&
						enter_trace.m_entity->breakable( ) ) {
						exit_trace = enter_trace;
						exit_trace.m_end = end + dir;

						return true;
					}
				}
				else {
					if ( enter_trace.m_entity->breakable( ) && exit_trace.m_entity->breakable( ) )
						return true;

					if ( ( enter_trace.m_surface.m_flags & 128 ) 
						|| ( !( exit_trace.m_surface.m_flags & 128 ) 
						&& exit_trace.m_plane.m_normal.dot( dir ) <= 1.f ) ) {
						end -= dir * ( exit_trace.m_frac * 4.f );

						return true;
					}
				}
			}
		}

		return false;
	}

	bool c_auto_wall::handle_bullet_penetration( const valve::weapon_info_t* wpn_info,
		const valve::surface_data_t* enter_surface_data, projectile_t& projectile
	) const {
		const auto enter_material = enter_surface_data->m_game.m_material;
		if ( !projectile.m_pen_count
			&& enter_material != 'G' && enter_material != 'Y' && !( projectile.m_trace.m_surface.m_flags & 128 ) )
			return true;

		if ( wpn_info->m_penetration <= 0.f || projectile.m_pen_count <= 0 )
			return true;

		valve::trace_t exit_trace{};
		if ( !trace_to_exit( projectile.m_trace, exit_trace, 
				projectile.m_trace.m_end, projectile.m_end, g_local_player->self() 
			) && !( valve::g_engine_trace->get_point_contents( 
				projectile.m_trace.m_end, valve::e_mask::shot_hull, nullptr
			) & valve::e_mask::shot_hull ) )
			return true;

		const auto exit_surface_data = valve::g_phys_props->get_surface_data( exit_trace.m_surface.m_surface_props );
		const auto exit_material = exit_surface_data->m_game.m_material;

		const auto enter_pen_modifier = enter_surface_data->m_game.m_pen_modifier;
		const auto exit_pen_modifier = exit_surface_data->m_game.m_pen_modifier;

		auto dmg_lost_modifier = 0.16f;
		auto pen_modifier = 0.f;

		if ( enter_material == 'G' || enter_material == 'Y' ) {
			dmg_lost_modifier = 0.05f;
			pen_modifier = 3.f;
		}
		else if ( ( ( projectile.m_trace.m_contents >> 3 ) & 1 ) || ( ( projectile.m_trace.m_surface.m_flags >> 7 ) & 1 ) )
			pen_modifier = 1.f;
		else if ( enter_material == 'F'
			&& g_ctx->cvars( ).ff_damage_reduction_bullets->get_float( ) == 0.f
			&& projectile.m_trace.m_entity != nullptr && projectile.m_trace.m_entity->is_player( )
			&& !g_ctx->cvars( ).mp_teammates_are_enemies->get_int( )
			&& g_local_player->self( )->team( ) == projectile.m_trace.m_entity->team( ) ) {
			const auto pen_dmg = g_ctx->cvars( ).ff_damage_bullet_penetration->get_float( );
			if ( pen_dmg == 0.f )
				return true;

			pen_modifier = pen_dmg;
		}
		else
			pen_modifier = ( enter_pen_modifier + exit_pen_modifier ) * 0.5f;

		if ( enter_material == exit_material ) {
			if ( exit_material == 'U' || exit_material == 'W' )
				pen_modifier = 3.f;
			else if ( exit_material == 'L' )
				pen_modifier = 2.f;
		}

		const auto trace_dist = ( exit_trace.m_end - projectile.m_trace.m_end ).length_sqr( );
		const auto modifier = ( pen_modifier > 0.f ? 1.f / pen_modifier : 0.f );

		const auto lost_dmg = ( projectile.m_dmg * dmg_lost_modifier
			+ ( wpn_info->m_penetration > 0.f ? 3.75f / wpn_info->m_penetration : 0.f ) * ( modifier * 3.f ) )
			+ ( ( modifier * trace_dist ) / 24.f );

		projectile.m_dmg -= std::max( lost_dmg, 0.f );

		if ( projectile.m_dmg < 1.f )
			return true;

		projectile.m_start = exit_trace.m_end;
		projectile.m_pen_count -= 1;

		return false;
	}
}