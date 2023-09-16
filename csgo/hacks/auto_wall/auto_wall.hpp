#pragma once

namespace hacks {
	class c_auto_wall {
	public:
		struct projectile_t {
			sdk::vec3_t		m_start{};
			sdk::vec3_t		m_end{};

			valve::trace_t	m_trace{};

			float			m_dmg{};
			int				m_pen_count{};
		};
	private:
		void scale_dmg( valve::e_hitgroup hit_group, valve::cs_player_t* player, 
			float wpn_armor_ratio, float wpn_headshot_multiplier, float& dmg
		) const;

		void clip_trace_to_players( const sdk::vec3_t& abs_start, const sdk::vec3_t& abs_end,
			const valve::e_mask mask, valve::trace_filter_simple_t* filter, valve::trace_t* trace,
			const float min_range, const float max_range
		) const;

		bool trace_to_exit( const valve::trace_t& enter_trace, valve::trace_t& exit_trace,
			const sdk::vec3_t& pos, const sdk::vec3_t& dir, const valve::base_entity_t* clip_player
		) const;

		bool handle_bullet_penetration( const valve::weapon_info_t* wpn_info,
			const valve::surface_data_t* enter_surface_data, projectile_t& projectile
		) const;

		bool simulate_projectile( valve::cs_player_t* player, valve::base_weapon_t* weapon, projectile_t& data );
	public:
		float get_dmg( valve::cs_player_t* player, const sdk::vec3_t& point, projectile_t* projectile = nullptr );
	};
}