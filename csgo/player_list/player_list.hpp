#pragma once

class c_players {
private:
	struct entry_t {
	public:
		ALWAYS_INLINE entry_t( ) = default;

		ALWAYS_INLINE entry_t( valve::base_entity_t* entity );
	public:
		valve::cs_player_t* m_player{};

		// add your stuff, example lag_records for lag_comp and etc
	};

	std::vector< entry_t > m_entries{};
public:
	ALWAYS_INLINE c_players( );

	void on_entity_add( valve::base_entity_t* entity );

	void on_entity_remove( valve::base_entity_t* entity );

	ALWAYS_INLINE const auto& get( );
};

#include "impl/player_list.inl"

inline const auto g_players = std::make_unique< c_players >( );