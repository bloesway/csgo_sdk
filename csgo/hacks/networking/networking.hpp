#pragma once

namespace hacks {
	class c_networking {
	private:
		struct netvars_data_t {
			sdk::qang_t		m_view_punch{}, m_punch{};

			sdk::vec3_t		m_punch_vel{}, m_velocity{},
							m_origin{}, m_view_offset{};

			float			m_duck_amt{}, m_duck_speed{};

			int				m_tick_base{}, m_cmd_number{};

			bool			m_filled{};

			void store_netvars( valve::cs_player_t* player, int cmd_number );

			void restore_netvars( valve::cs_player_t* player );
		};

		struct process_cmd_t {
			bool	m_outgoing{}, m_handled{};

			int		m_prev_cmd_num{}, m_cmd_num{};
		};
	public:
		float												m_latency{}, m_lerp_amt{};

		bool												m_simulate_choke{};

		int													m_tick_rate{}, m_seq{},
															m_server_tick{}, m_local_tick{};

		std::deque< process_cmd_t >							m_process_cmds{};
		std::array< netvars_data_t, valve::k_mp_backup >	m_netvars_data{};

		ALWAYS_INLINE void emplace_cmd( const bool outgoing, const int cmd_num,
			const int prev_cmd_num = 0, const bool handled = false 
		);

		ALWAYS_INLINE auto& netvars_data( );
	public:
		void start( );

		void end( );

		void reset( );
	};

	inline const auto g_networking = std::make_unique< c_networking >( );
}

#include "impl/networking.inl"