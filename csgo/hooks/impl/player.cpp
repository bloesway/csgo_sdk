#include "../../csgo.hpp"

namespace hooks {
	bool __fastcall interpolate_view_model( valve::base_entity_t* ecx, void* edx, float time ) {
		if ( !hacks::g_interpolation->m_skip )
			return o_interpolate_view_model( ecx, edx, time );

		auto owner = valve::g_entity_list->get_entity( ecx->view_model_owner( ) );
		if ( owner->networkable( )->index( ) != g_local_player->self( )->networkable( )->index( ) )
			return o_interpolate_view_model( ecx, edx, time );

		const auto interp_amt = valve::g_global_vars->m_interp_amt;

		valve::g_global_vars->m_interp_amt = 0.f;

		const auto ret = o_interpolate_view_model( ecx, edx, time );

		valve::g_global_vars->m_interp_amt = interp_amt;

		return ret;
	}
}