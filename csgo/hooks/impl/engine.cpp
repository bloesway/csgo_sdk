#include "../../csgo.hpp"

namespace hooks {
	void __cdecl cl_move( float frame_time, bool is_final_tick ) {
		if ( !valve::g_engine->in_game( ) )
			return o_cl_move( frame_time, is_final_tick );

		hacks::g_networking->start( );

		o_cl_move( frame_time, is_final_tick );

		hacks::g_networking->end( );
	}

	bool __fastcall is_paused( void* ecx, void* edx ) {
		const auto ret = sdk::stack_frame_t( ).ret_addr( );
		if ( ret == g_ctx->offsets( ).m_is_extrapolated )
			return true;

		return o_is_paused( ecx, edx );
	}

	bool __fastcall is_hltv( void* ecx, void* edx ) {
		const auto ret = sdk::stack_frame_t( ).ret_addr( );
		if ( ret == g_ctx->offsets( ).m_setup_velocity
			|| ret == g_ctx->offsets( ).m_accumulate_layers )
			return true;

		if ( g_players->contains( hacks::g_anim_system->last_anim_player( ) ) ) {
			const auto& entry = g_players->get( hacks::g_anim_system->last_anim_player( ) );
			if ( entry.m_setup_bones 
				|| entry.m_update_anims )
				return true;
		}

		return o_is_hltv( ecx, edx );
	}
}