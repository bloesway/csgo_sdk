#pragma once

namespace hacks {
	class c_anim_system {
	private:
		void handle_velocity( valve::cs_player_t* player, 
			valve::lag_record_t record, valve::lag_record_t prev_record, bool has_prev_record 
		);

		struct anim_backup_t {
		private:
			valve::anim_layers_t m_layers{};
			valve::pose_params_t m_poses{};
			valve::anim_state_t  m_anim_state{};
		public:
			ALWAYS_INLINE void store( valve::cs_player_t* player ) {
				m_layers = player->anim_layers( );
				m_poses = player->pose_params( );

				std::memcpy( &m_anim_state, player->anim_state( ), sizeof( valve::anim_state_t ) );
			}

			ALWAYS_INLINE void restore( valve::cs_player_t* player, bool layers, bool poses, bool anim_state ) {
				if ( layers )
					player->anim_layers( ) = m_layers;

				if ( poses )
					player->pose_params( ) = m_poses;

				if ( anim_state )
					std::memcpy( player->anim_state( ), &m_anim_state, sizeof( valve::anim_state_t ) );
			}
		} m_anim_backup{};

		int m_last_anim_player{ -1 };
	public:
		void handle( valve::cs_player_t* player, c_players::entry_t& entry );

		void update( valve::cs_player_t* player, c_players::entry_t& entry,
			valve::lag_record_t record, valve::lag_record_t prev_record, bool has_prev_record, 
			valve::e_rotation_side side = valve::e_rotation_side::none
		);

		void update_client_side_anims( valve::cs_player_t* player, c_players::entry_t& entry );

		void setup_bones( valve::cs_player_t* player, c_players::entry_t& entry,
			sdk::mat3x4_t* bones, const int bones_count, const valve::e_bone_flags flags 
		);

		bool on_setup_bones( c_players::entry_t& entry, sdk::mat3x4_t* bones, int bones_count );
	public:
		ALWAYS_INLINE const auto last_anim_player( ) const;
	};

	inline const auto g_anim_system = std::make_unique< c_anim_system >( );
}

#include "impl/anim_system.inl"