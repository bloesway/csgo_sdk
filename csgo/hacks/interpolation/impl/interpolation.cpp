#include "../../../csgo.hpp"

namespace hacks {
	void c_interpolation::handle( bool post ) {
		if ( post ) {
			valve::g_global_vars->m_interp_amt = m_amount;

			return;
		}

		m_amount = valve::g_global_vars->m_interp_amt;

		if ( m_skip )
			valve::g_global_vars->m_interp_amt = 0.f;

		g_local_player->self( )->pred_tick( ) = g_local_player->self( )->tick_base( );
	}

	void c_interpolation::skip( ) {
		m_skip = true;
	}

	void c_interpolation::process( ) {
		m_skip = false;
	}
}