#pragma once

#include "../networking.hpp"

namespace hacks {
	ALWAYS_INLINE void c_networking::emplace_cmd( const bool outgoing, const int cmd_num, 
		const int prev_cmd_num, const bool handled 
	) {
		auto& cmd = m_process_cmds.emplace_back( );

		cmd.m_outgoing = outgoing;
		cmd.m_handled = handled;

		cmd.m_cmd_num = cmd_num;
		cmd.m_prev_cmd_num = prev_cmd_num;
	}

	ALWAYS_INLINE auto& c_networking::netvars_data( ) {
		return m_netvars_data;
	}
}