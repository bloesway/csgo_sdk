#pragma once

namespace hacks {
	class c_lag_comp {
	private:
		valve::lag_record_t m_backup_players[ 64u ]{};
	public:
		void handle( );

		void update_player( valve::cs_player_t* player, valve::lag_record_t record );

		void store_players( );

		void restore_players( );
	};

	inline const auto g_lag_comp = std::make_unique< c_lag_comp >( );
}