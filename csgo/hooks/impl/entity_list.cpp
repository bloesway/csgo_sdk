#include "../../csgo.hpp"

namespace hooks {
	void __fastcall on_entity_add( void* ecx, void* edx, valve::base_entity_t* entity, valve::ent_handle_t handle ) {
		if ( entity )
			g_players->on_entity_add( entity );

		o_on_entity_add( ecx, edx, entity, handle );
	}

	void __fastcall on_entity_remove( void* ecx, void* edx, valve::base_entity_t* entity, valve::ent_handle_t handle ) {
		if ( entity )
			g_players->on_entity_remove( entity );

		o_on_entity_remove( ecx, edx, entity, handle );
	}
}