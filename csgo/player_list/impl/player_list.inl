#pragma once

ALWAYS_INLINE constexpr c_players::c_players( ) {
	m_entries.reserve( 64u );
}

ALWAYS_INLINE auto& c_players::get( ) {
	return m_entries;
}

ALWAYS_INLINE c_players::entry_t::entry_t( valve::base_entity_t* entity, int index ) {
	m_player = reinterpret_cast< valve::cs_player_t* >( entity );
	m_index = index;

	m_records.clear( );
	m_prev_record = nullptr;

	m_in_dormancy = true;
	m_first_after_dormant = false;

	m_bones.fill( sdk::mat3x4_t{} );
}