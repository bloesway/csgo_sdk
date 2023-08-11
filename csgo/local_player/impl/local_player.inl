#pragma once

#include "../local_player.hpp"

ALWAYS_INLINE valve::cs_player_t* c_local_player::self( ) const {
    return *g_ctx->offsets( ).m_local_player.as< valve::cs_player_t** >( );
}

ALWAYS_INLINE valve::weapon_cs_base_gun_t* c_local_player::weapon( ) const { return m_weapon; }

ALWAYS_INLINE valve::weapon_info_t* c_local_player::weapon_info( ) const { return m_weapon_info; }