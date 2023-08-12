#include "../../csgo.hpp"

namespace hooks {
	void __cdecl cl_move( float frame_time, bool is_final_tick ) {
		hacks::g_networking->start( );

		o_cl_move( frame_time, is_final_tick );

		hacks::g_networking->end( );
	}
}