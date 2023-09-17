#pragma once

namespace hacks {
	class c_auto_wall {
	private:
		struct projectile_t {
			sdk::vec3_t		m_pos{};
			sdk::vec3_t		m_dir{};

			valve::trace_t	m_trace{};

			float			m_dmg{};
			int				m_pen_count{};
		};
	private:
		void scale_dmg( valve::e_hitgroup hit_group, valve::cs_player_t* player,
			float wpn_armor_ratio, float wpn_headshot_multiplier, float& dmg
		) const;

		void clip_trace_to_players( const sdk::vec3_t& start, const sdk::vec3_t& end, valve::e_mask mask,
			valve::trace_filter_simple_t* filter, valve::trace_t* trace, float min_range, float max_range
		) const;

		bool trace_to_exit( valve::trace_t& enter_trace, valve::trace_t& exit_trace,
			sdk::vec3_t& pos, sdk::vec3_t& dir, valve::base_entity_t* clip_player
		) const;

		bool handle_bullet_penetration( valve::cs_player_t* player, valve::weapon_info_t* wpn_info,
			valve::surface_data_t* enter_surface_data, projectile_t& projectile_data
		) const;

		bool simulate_projectile( valve::cs_player_t* player, valve::base_weapon_t* weapon,
			projectile_t& projectile_data
		) const;
	public:
		float get_dmg( valve::cs_player_t* player, sdk::vec3_t& point,
			projectile_t* projectile_data = nullptr
		);
	};

	inline const auto g_auto_wall = std::make_unique< c_auto_wall >( );
}