#pragma once

#include "../local_player.hpp"

ALWAYS_INLINE valve::cs_player_t* c_local_player::self( ) const {
    return *g_ctx->offsets( ).m_local_player.as< valve::cs_player_t** >( );
}

ALWAYS_INLINE valve::weapon_cs_base_gun_t* c_local_player::weapon( ) const { return m_weapon; }

ALWAYS_INLINE valve::weapon_info_t* c_local_player::weapon_info( ) const { return m_weapon_info; }

ALWAYS_INLINE void c_local_player::prediction_t::store( ) {
    m_tick_count = valve::g_global_vars->m_tick_count;

    m_in_prediction = valve::g_prediction->m_in_prediction;
    m_first_time_predicted = valve::g_prediction->m_first_time_predicted;

    m_cur_time = valve::g_global_vars->m_cur_time;
    m_frame_time = valve::g_global_vars->m_frame_time;
}

ALWAYS_INLINE void c_local_player::prediction_t::restore( ) {
    valve::g_global_vars->m_tick_count = m_tick_count;

    valve::g_prediction->m_in_prediction = m_in_prediction;
    valve::g_prediction->m_first_time_predicted = m_first_time_predicted;

    valve::g_global_vars->m_cur_time = m_cur_time;
    valve::g_global_vars->m_frame_time = m_frame_time;
}