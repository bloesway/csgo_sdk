#pragma once

ALWAYS_INLINE c_players::c_players( ) {
	m_entries.reserve( 64u );
}

ALWAYS_INLINE const auto& c_players::get( ) {
	return m_entries;
}

ALWAYS_INLINE c_players::entry_t::entry_t( valve::base_entity_t* entity ) {
	m_player = reinterpret_cast< valve::cs_player_t* >( entity );
}