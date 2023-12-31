#pragma once

class c_players {
public:
	ALWAYS_INLINE c_players( );

	struct entry_t {
	public:
		ALWAYS_INLINE entry_t( ) = default;

		ALWAYS_INLINE entry_t( valve::base_entity_t* entity, int index );
	public:
		valve::cs_player_t*								m_player{};
		int												m_index{};

		valve::lag_record_t								m_prev_record{};
		std::deque< valve::lag_record_t >				m_records{};

		bool											m_in_dormancy{}, m_first_after_dormant{},
														m_setup_bones{}, m_update_anims{};

		float											m_spawn_time{};

		std::array< sdk::mat3x4_t, valve::k_max_bones >	m_bones{};
	};

	struct target_t {
		ALWAYS_INLINE constexpr target_t( ) = default;

		ALWAYS_INLINE target_t( valve::cs_player_t* player );

		valve::cs_player_t* m_player{};

		/*	your other data, 
			as example: last_record, oldest_record */
	};
private:
	std::unordered_map< int, entry_t > m_hash_map{};
public:
	void on_entity_add( valve::base_entity_t* entity );

	void on_entity_remove( valve::base_entity_t* entity );

	std::vector< target_t > find_targets( );
public:
	ALWAYS_INLINE auto& get( );

	ALWAYS_INLINE auto& get( int hash );

	ALWAYS_INLINE const auto contains( int hash );
};

#include "impl/player_list.inl"

inline const auto g_players = std::make_unique< c_players >( );