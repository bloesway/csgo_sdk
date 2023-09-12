#include "../../csgo.hpp"

namespace hooks {
	bool __fastcall interpolate_view_model( valve::base_entity_t* ecx, void* edx, float time ) {
		if ( !hacks::g_interpolation->m_skip )
			return o_interpolate_view_model( ecx, edx, time );

		const auto owner = valve::g_entity_list->get_entity( ecx->view_model_owner( ) );
		if ( owner->networkable( )->index( ) != g_local_player->self( )->networkable( )->index( ) )
			return o_interpolate_view_model( ecx, edx, time );

		const auto interp_amt = valve::g_global_vars->m_interp_amt;

		valve::g_global_vars->m_interp_amt = 0.f;

		const auto ret = o_interpolate_view_model( ecx, edx, time );

		valve::g_global_vars->m_interp_amt = interp_amt;

		return ret;
	}

	void __fastcall physics_simulate( valve::cs_player_t* ecx, void* edx ) {
		if ( !ecx || !ecx->alive( )
			|| ecx != g_local_player->self( ) )
			return o_physics_simulate( ecx, edx );

		auto& ctx_cmd = ecx->ctx_cmd( );

		if ( valve::g_global_vars->m_tick_count == ecx->sim_tick( )
			|| !ctx_cmd.m_needs_processing )
			return o_physics_simulate( ecx, edx );

		if ( ctx_cmd.m_user_cmd.m_tick == std::numeric_limits< int >::max( ) ) {
			ctx_cmd.m_needs_processing = false;

			ecx->sim_tick( ) = valve::g_global_vars->m_tick_count;

			const auto data = &hacks::g_networking->netvars_data( ).at( 
				ctx_cmd.m_user_cmd.m_number % valve::k_mp_backup
			);

			return data->store_netvars( ctx_cmd.m_user_cmd.m_number );
		}

		const auto data = &hacks::g_networking->netvars_data( ).at( 
			( ctx_cmd.m_user_cmd.m_number - 1 ) % valve::k_mp_backup
		);

		if ( data && data->m_filled 
			&& data->m_cmd_number == ctx_cmd.m_user_cmd.m_number - 1 )
			data->restore_netvars( );

		o_physics_simulate( ecx, edx );

		const auto new_data = &hacks::g_networking->netvars_data( ).at( 
			ctx_cmd.m_user_cmd.m_number % valve::k_mp_backup
		);

		new_data->store_netvars( ctx_cmd.m_user_cmd.m_number );
	}

	void __fastcall update_client_animations( valve::cs_player_t* ecx, void* edx ) {
		if ( !ecx || !ecx->alive( )
			|| ecx->friendly( g_local_player->self( ) ) )
			return o_update_client_side_animations( ecx, edx );

		const auto index = ecx->networkable( )->index( );
		if ( index < 1
			|| index > valve::g_global_vars->m_max_clients )
			return o_update_client_side_animations( ecx, edx );

		if ( g_players->contains( index ) ) {
			auto& entry = g_players->get( index );
			if ( entry.m_player ) {
				if ( entry.m_update_anims )
					return o_update_client_side_animations( ecx, edx );
			}
			else
				return o_update_client_side_animations( ecx, edx );
		}
		else
			return o_update_client_side_animations( ecx, edx );
	}

	bool __fastcall setup_bones( void* ecx, void* edx, sdk::mat3x4_t* bones, int max_bones, int mask, float time ) {
		const auto player = reinterpret_cast< valve::cs_player_t* >(
			reinterpret_cast< sdk::ulong_t >( ecx ) - 0x4u );
		if ( !player || !player->alive( )
			|| player->friendly( g_local_player->self( ) ) )
			return o_setup_bones( ecx, edx, bones, max_bones, mask, time );

		const auto index = player->networkable( )->index( );
		if ( index < 1
			|| index > valve::g_global_vars->m_max_clients )
			return o_setup_bones( ecx, edx, bones, max_bones, mask, time );

		auto result = true;

		{
			if ( g_players->contains( index ) ) {
				auto& entry = g_players->get( index );
				if ( entry.m_player ) {
					if ( entry.m_setup_bones )
						result = o_setup_bones( ecx, edx, bones, max_bones, mask, time );
					else if ( bones )
						result = hacks::g_anim_system->on_setup_bones( entry, bones, max_bones );
				}
				else
					result = o_setup_bones( ecx, edx, bones, max_bones, mask, time );
			}
			else
				result = o_setup_bones( ecx, edx, bones, max_bones, mask, time );
		}

		return result;
	}

	void __fastcall do_extra_bone_processing( valve::cs_player_t* ecx, void* edx,
		valve::studio_hdr_t* hdr, sdk::vec3_t* pos, void* quat, sdk::mat3x4_t* bone_to_world, void* bone_computed, void* ik_ctx
	) {
		return;
	}
}