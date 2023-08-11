#pragma once

class c_local_player {
private:
    void update_prediction( ) const;

    valve::weapon_cs_base_gun_t*    m_weapon{};
    valve::weapon_info_t*           m_weapon_info{};
public:
    void create_move( bool& send_packet,
        valve::user_cmd_t& cmd, valve::vfyd_user_cmd_t& vfyd_cmd
    );

    ALWAYS_INLINE valve::cs_player_t* self( ) const;

    ALWAYS_INLINE valve::weapon_cs_base_gun_t* weapon( ) const;

    ALWAYS_INLINE valve::weapon_info_t* weapon_info( ) const;
};

inline const auto g_local_player = std::make_unique< c_local_player >( );

#include "impl/local_player.inl"