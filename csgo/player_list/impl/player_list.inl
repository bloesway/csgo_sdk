#pragma once

ALWAYS_INLINE c_players::c_players( ) {
	m_hash_map.reserve( 64u );
}

ALWAYS_INLINE auto& c_players::get( ) {
	return m_hash_map;
}

ALWAYS_INLINE auto& c_players::get( int hash ) {
	return m_hash_map.at( hash );
}

ALWAYS_INLINE const auto c_players::contains( int hash ) {
	return m_hash_map.contains( hash );
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

ALWAYS_INLINE c_players::target_t::target_t( valve::cs_player_t* player ) {
	m_player = player;
}