#pragma once

#include "../entities.hpp"

namespace valve {
    ALWAYS_INLINE weapon_info_t* base_weapon_t::info( ) {
        using fn_t = weapon_info_t * ( __thiscall* )( sdk::address_t, e_item_index );

        const auto weapon_system = g_ctx->offsets( ).m_weapon_system;

        return ( *weapon_system.as< fn_t** >( ) )[ 2u ]( weapon_system, item_index( ) );
    }

    ALWAYS_INLINE weapon_cs_base_gun_t* base_combat_character_t::weapon( ) {
        return static_cast< weapon_cs_base_gun_t* >( g_entity_list->get_entity( weapon_handle( ) ) );
    }

    ALWAYS_INLINE bool base_player_t::alive( ) {
        return life_state( ) == e_life_state::alive && health( ) > 0;
    }

    ALWAYS_INLINE std::optional< player_info_t > base_player_t::info( ) {
        player_info_t info{};
        if ( !g_engine->get_player_info( networkable( )->index( ), &info ) )
            return std::nullopt;

        return info;
    }

    ALWAYS_INLINE bool cs_player_t::friendly( cs_player_t* const with ) {
        if ( with == this )
            return true;

#ifndef CSGO2018
        if ( g_game_types->game_type( ) == e_game_type::ffa )
            return survival_team( ) == with->survival_team( );
#endif

        if ( g_ctx->cvars( ).mp_teammates_are_enemies->get_int( ) )
            return false;

        return team( ) == with->team( );
    }

    ALWAYS_INLINE player_record_t::player_record_t( ) {
        m_filled = false;
    }

    /*  !!! if you want to get bones from record store them here,
        !!! because we need setup bones when the animations are fully updated,
        !!! but i havent added the animation system yet, so the bones will be empty */
    ALWAYS_INLINE player_record_t::player_record_t( valve::cs_player_t* player ) {
        m_player = player;

        m_sim_time = player->sim_time( );
        m_old_sim_time = player->old_sim_time( );

        m_duck_amt = player->duck_amt( );
        m_lby = player->lby( );

        m_eye_angles = player->eye_angles( );
        m_abs_angles = player->abs_angles( );

        m_velocity = player->velocity( );
        m_origin = player->origin( );
        m_abs_origin = player->abs_origin( );

        m_mins = player->obb_min( );
        m_maxs = player->obb_max( );

        m_flags = player->flags( );
        m_sim_ticks = 1;

        m_walking = player->walking( );
        m_broke_lc = false;

        const auto anim_state = player->anim_state( );
        if ( anim_state ) {
            m_eye_yaw = anim_state->m_eye_yaw;
        }

        const auto weapon = player->weapon( );
        if ( weapon ) {
            auto weapon_info = weapon->info( );
            if ( weapon_info ) {
                m_max_speed = player->scoped( ) ? weapon_info->m_max_speed_alt : weapon_info->m_max_speed;
            }
            else
                m_max_speed = 260.f;

            if ( weapon->last_shot_time( ) <= player->sim_time( ) && weapon->last_shot_time( ) > player->old_sim_time( ) ) {
                m_did_shot = true;
                m_shot_tick = valve::to_ticks( weapon->last_shot_time( ) );
            }
        }

        if ( auto info = player->info( ); info.has_value( ) )
            m_fake_player = info->m_fake_player;

        std::memcpy( m_layers.data( ), player->anim_layers( ).data( ), sizeof( anim_layer_t ) * valve::k_max_layers );

        /*  i think you can do not store pose_params here,
            because you need to store them in record again when the animations are fully updated  */
        std::memcpy( m_pose_params.data( ), player->pose_params( ).data( ), sizeof( float ) * valve::k_max_poses );

        m_filled = true;
    }
}