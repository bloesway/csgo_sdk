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

		auto& cmd_context = ecx->context_cmd( );

		if ( valve::g_global_vars->m_tick_count == ecx->sim_tick( )
			|| !cmd_context.m_needs_processing )
			return o_physics_simulate( ecx, edx );

		if ( cmd_context.m_user_cmd.m_tick == std::numeric_limits< int >::max( ) ) {
			cmd_context.m_needs_processing = false;

			ecx->sim_tick( ) = valve::g_global_vars->m_tick_count;

			const auto data = &hacks::g_networking->netvars_data( ).at( cmd_context.m_user_cmd.m_number % valve::k_mp_backup );

			return data->store_netvars( cmd_context.m_user_cmd.m_number );
		}

		const auto data = &hacks::g_networking->netvars_data( ).at( ( cmd_context.m_user_cmd.m_number - 1 ) % valve::k_mp_backup );

		if ( data && data->m_filled && data->m_cmd_number == cmd_context.m_user_cmd.m_number - 1 )
			data->restore_netvars( cmd_context.m_user_cmd.m_number - 1 );

		o_physics_simulate( ecx, edx );

		const auto new_data = &hacks::g_networking->netvars_data( ).at( cmd_context.m_user_cmd.m_number % valve::k_mp_backup );

		new_data->store_netvars( cmd_context.m_user_cmd.m_number );
	}
}