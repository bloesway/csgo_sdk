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

    ALWAYS_INLINE bool base_entity_t::breakable( ) {
        if ( health( ) <= 0
            || networkable( )->index( ) <= 0 )
            return true;

        if ( breakable_game( ) )
            return true;

        const auto client_class = networkable( )->client_class( );
        if ( !client_class )
            return false;

        return ( *reinterpret_cast< const std::uint32_t* >( client_class->m_network_name ) == 'erBC'
            && *reinterpret_cast< const std::uint32_t* >( client_class->m_network_name + 7 ) == 'Selb' )
            || ( *reinterpret_cast< const std::uint32_t* >( client_class->m_network_name ) == 'saBC'
            && *reinterpret_cast< const std::uint32_t* >( client_class->m_network_name + 7 ) == 'ytit' );
    }

    ALWAYS_INLINE void cs_player_t::set_collision_bounds( sdk::vec3_t& mins, sdk::vec3_t& maxs ) {
        return g_ctx->offsets( ).m_cs_player.m_set_collision_bounds.as<
            void( __thiscall* )( void*, sdk::vec3_t*, sdk::vec3_t* )
        >( )( reinterpret_cast< void* >( reinterpret_cast< sdk::ulong_t >( this ) + 0x320u ), &mins, &maxs );
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

    ALWAYS_INLINE sdk::vec3_t base_player_t::eye_pos( ) {
        return origin( ) + view_offset( );
    }

    ALWAYS_INLINE bool cs_player_t::friendly( cs_player_t* const with ) {
        if ( with == this )
            return true;

        if ( g_ctx->cvars( ).mp_teammates_are_enemies->get_int( ) )
            return false;

        return team( ) == with->team( );
    }

    ALWAYS_INLINE int cs_player_t::seq_activity( const int seq ) {
        const auto model = renderable( )->model( );
        if ( !model )
            return -1;

        const auto studio_model = g_model_info->studio_model( model );
        if ( !studio_model )
            return -1;

        return g_ctx->offsets( ).m_cs_player.m_seq_activity.as< int( __fastcall* )( decltype( this ), studio_hdr_t*, int ) >( )(
            this, studio_model, seq
        );
    }

    ALWAYS_INLINE void cs_player_t::invalidate_bone_cache( ) {
        auto        most_recent_model_cnt_addr = g_ctx->offsets( ).m_cs_player.m_most_recent_model_cnt;
        const auto  most_recent_model_cnt = **most_recent_model_cnt_addr.self_offset( 0xA ).as< unsigned long** >( );

        last_bones_time( ) = std::numeric_limits< float >::lowest( );
        mdl_bone_cnt( ) = most_recent_model_cnt - 1ul;
    }

    ALWAYS_INLINE bool cs_player_t::setup_bones( sdk::mat3x4_t* matrix, int bones_count, e_bone_flags flags, float time ) {
        invalidate_bone_cache( );

        return renderable( )->setup_bones( matrix, bones_count, -flags, time );
    }

    ALWAYS_INLINE bool cs_player_t::armored( const e_hitgroup hit_group ) {
        auto ret = false;

        if ( armor_value( ) > 0 ) {
            switch ( hit_group ) {
                case e_hitgroup::generic:
                case e_hitgroup::chest:
                case e_hitgroup::stomach:
                case e_hitgroup::left_arm:
                case e_hitgroup::right_arm:
                case e_hitgroup::neck:
                    ret = true;
                    break;
                case e_hitgroup::head:
                    if ( helmet( ) )
                        ret = true;
                    break;
                case e_hitgroup::left_leg:
                case e_hitgroup::right_leg:
                    if ( heavy_armor( ) )
                        ret = true;
                    break;

                default:
                    break;
            }
        }

        return ret;
    }

    ALWAYS_INLINE player_record_t::player_record_t( ) {
        m_filled = false;
    }

    ALWAYS_INLINE player_record_t::player_record_t( valve::cs_player_t* player ) {
        m_player = player;

        m_side = valve::e_rotation_side::none;

        m_sim_time = player->sim_time( );
        m_old_sim_time = player->old_sim_time( );

        m_duck_amt = player->duck_amt( );
        m_lby = player->lby( );

        m_eye_angles = player->eye_angles( );
        m_abs_angles = player->abs_angles( );

        m_velocity = player->velocity( );
        m_anim_velocity = player->velocity( );
        m_origin = player->origin( );
        m_abs_origin = player->abs_origin( );

        m_mins = player->obb_min( );
        m_maxs = player->obb_max( );

        m_flags = player->flags( );
        m_sim_ticks = 1;

        m_walking = player->walking( );
        m_broke_lc = false;

        m_seq_tick = -1;
        m_seq_type = e_seq_type::none;

        m_time_in_air = 0.f;

        const auto anim_state = player->anim_state( );
        if ( anim_state ) {
            m_eye_yaw = anim_state->m_eye_yaw;

            {
                auto aim_matrix_width_range = sdk::lerp( 1.f, 
                    sdk::lerp( 0.8f, 0.5f, anim_state->m_walk_to_run_transition ),
                    std::clamp( anim_state->m_speed_as_portion_of_walk_speed, 0.f, 1.f )
                );

                if ( anim_state->m_duck_amount > 0.f )
                    aim_matrix_width_range = sdk::lerp( aim_matrix_width_range, 0.5f, anim_state->m_duck_amount *
                        std::clamp( anim_state->m_speed_as_portion_of_crouch_speed, 0.f, 1.f )
                    );

                m_desync_delta = anim_state->m_aim_yaw_max * aim_matrix_width_range;
            }
        }
        else
            m_desync_delta = 58.f;

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

        m_layers = player->anim_layers( );

        /*  i think you can do not store pose_params here,
            because you need to store them in record again when the animations are fully updated */
        m_pose_params = player->pose_params( );

        m_filled = true;
    }
}