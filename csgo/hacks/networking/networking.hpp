#pragma once

namespace hacks {
	class c_networking {
	private:
		struct process_cmd_t {
			bool	m_outgoing{};
			bool	m_handled{};
			int		m_prev_cmd_num{};
			int		m_cmd_num{};
		};
	public:
		float						m_latency{};
		bool						m_simulate_choke{};

		int							m_tick_rate{};
		int							m_seq{};

		int							m_server_tick{};
		int							m_local_tick{};

		std::deque< process_cmd_t >	m_process_cmds{};

		ALWAYS_INLINE void emplace_cmd( const bool outgoing, const int cmd_num,
			const int prev_cmd_num = 0, const bool handled = false 
		);
	public:
		void start( );
		void end( );

		void reset( );
	};

	inline const auto g_networking = std::make_unique< c_networking >( );
}

#include "impl/networking.inl"