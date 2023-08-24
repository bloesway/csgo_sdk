#pragma once

namespace valve {
    struct renderable_t {
        VFUNC( bool( __thiscall* )( decltype( this ), sdk::mat3x4_t*, int, int, float ),
            setup_bones( sdk::mat3x4_t* bones, int max_bones, int mask, float time ), 13u, bones, max_bones, mask, time
        );
    };

    struct networkable_t {
        VFUNC( bool( __thiscall* )( decltype( this ) ), dormant( ), 9u );

        VFUNC( int( __thiscall* )( decltype( this ) ), index( ), 10u );
    };

    struct base_entity_t {
        POFFSET( renderable_t, renderable( ), sizeof( sdk::address_t ) );
        POFFSET( networkable_t, networkable( ), sizeof( sdk::address_t ) * 2u );

        OFFSET( int, health( ), g_ctx->offsets( ).m_base_entity.m_health );
        OFFSET( e_team, team( ), g_ctx->offsets( ).m_base_entity.m_team_num );

        OFFSET( float, sim_time( ), g_ctx->offsets( ).m_base_entity.m_sim_time );
        OFFSET( float, old_sim_time( ), g_ctx->offsets( ).m_base_entity.m_sim_time + sizeof( float ) );

        OFFSET( e_ent_flags, flags( ), g_ctx->offsets( ).m_base_entity.m_flags );
        OFFSET( e_eflags, eflags( ), g_ctx->offsets( ).m_base_entity.m_eflags );

        OFFSET( sdk::vec3_t, origin( ), g_ctx->offsets( ).m_base_entity.m_origin );
        OFFSET( sdk::vec3_t, velocity( ), g_ctx->offsets( ).m_base_entity.m_velocity );

        OFFSET( sdk::vec3_t, abs_velocity( ), g_ctx->offsets( ).m_base_entity.m_abs_velocity );
        OFFSET( sdk::qang_t, abs_rotation( ), g_ctx->offsets( ).m_base_entity.m_abs_rotation );

        OFFSET( e_move_type, move_type( ), g_ctx->offsets( ).m_base_entity.m_move_type );

        OFFSET( sdk::vec3_t, obb_min( ), g_ctx->offsets( ).m_base_entity.m_mins );
        OFFSET( sdk::vec3_t, obb_max( ), g_ctx->offsets( ).m_base_entity.m_maxs );

        OFFSET( bones_t, bones( ), g_ctx->offsets( ).m_renderable.m_bone_cache + sizeof( std::uintptr_t ) );
        OFFSET( sdk::ulong_t, mdl_bone_cnt( ), g_ctx->offsets( ).m_renderable.m_mdl_bone_cnt + sizeof( std::uintptr_t ) );

        OFFSET( float, last_bones_time( ), 0x2928 );

        OFFSET( bool, jiggle_bones( ), 0x2930 );

        OFFSET( std::uint32_t, occlusion_mask( ), 0xa24 );

        OFFSET( std::uint32_t, occlusion_frame_count( ), 0xa30 );

        VFUNC( sdk::vec3_t& ( __thiscall* )( decltype( this ) ), abs_origin( ), 10u );

        VFUNC( sdk::qang_t& ( __thiscall* )( decltype( this ) ), abs_angles( ), 11u );

        OFFSET_VFUNC( bool( __thiscall* )( decltype( this ), sdk::vec3_t& ),
            set_abs_origin( sdk::vec3_t& new_origin ), g_ctx->offsets( ).m_base_entity.m_set_abs_origin, new_origin
        );

        OFFSET_VFUNC( bool( __thiscall* )( decltype( this ), sdk::qang_t& ),
            set_abs_angles( sdk::qang_t& new_angles ), g_ctx->offsets( ).m_base_entity.m_set_abs_angles, new_angles
        );

        /* it can be in new struct base_view_model_t ig */
        OFFSET( ent_handle_t, view_model_owner( ), g_ctx->offsets( ).m_view_model.m_owner );
    };

    struct base_animating_t : public base_entity_t {
        OFFSET( int, sequence( ), g_ctx->offsets( ).m_base_animating.m_sequence );

        OFFSET( int, hitbox_set_index( ), g_ctx->offsets( ).m_base_animating.m_hitbox_set_index );
        OFFSET( studio_hdr_t*, studio_hdr( ), g_ctx->offsets( ).m_base_animating.m_studio_hdr );

        OFFSET( pose_params_t, pose_params( ), g_ctx->offsets( ).m_base_animating.m_pose_params );
        PPOFFSET( anim_layers_t, anim_layers( ), g_ctx->offsets( ).m_base_animating.m_anim_layers );

        OFFSET( bool, client_side_anim( ), g_ctx->offsets( ).m_base_animating.m_client_side_anim );
    };

    struct base_attributable_item_t : public base_animating_t {
        OFFSET( e_item_index, item_index( ), g_ctx->offsets( ).m_base_attributable_item.m_item_index );
    };

    struct base_grenade_t : public base_attributable_item_t {
        OFFSET( bool, pin_pulled( ), g_ctx->offsets( ).m_base_grenade.m_pin_pulled );

        OFFSET( float, throw_time( ), g_ctx->offsets( ).m_base_grenade.m_throw_time );
        OFFSET( float, throw_strength( ), g_ctx->offsets( ).m_base_grenade.m_throw_strength );
    };

    struct base_weapon_t : public base_attributable_item_t {
        ALWAYS_INLINE weapon_info_t* info( );

        OFFSET( int, clip1( ), g_ctx->offsets( ).m_base_weapon.m_clip1 );
        OFFSET( int, primary_reserve_ammo_count( ), g_ctx->offsets( ).m_base_weapon.m_primary_reserve_ammo_count );

        OFFSET( float, next_primary_attack( ), g_ctx->offsets( ).m_base_weapon.m_next_primary_attack );
        OFFSET( float, next_secondary_attack( ), g_ctx->offsets( ).m_base_weapon.m_next_secondary_attack );
    };

    struct weapon_cs_base_t : public base_weapon_t {
        OFFSET( bool, burst_mode( ), g_ctx->offsets( ).m_weapon_cs_base.m_burst_mode );
        OFFSET( float, last_shot_time( ), g_ctx->offsets( ).m_weapon_cs_base.m_last_shot_time );

        OFFSET( float, recoil_index( ), g_ctx->offsets( ).m_weapon_cs_base.m_recoil_index );

        OFFSET( float, postpone_fire_ready_time( ), g_ctx->offsets( ).m_weapon_cs_base.m_postpone_fire_ready_time );
    };

    struct weapon_cs_base_gun_t : public weapon_cs_base_t {
        OFFSET( int, zoom_lvl( ), g_ctx->offsets( ).m_weapon_cs_base_gun.m_zoom_lvl );

        OFFSET( int, burst_shots_remaining( ), g_ctx->offsets( ).m_weapon_cs_base_gun.m_burst_shots_remaining );
        OFFSET( float, next_burst_shot( ), g_ctx->offsets( ).m_weapon_cs_base_gun.m_next_burst_shot );
    };

    struct base_combat_character_t : public base_animating_t {
        ALWAYS_INLINE weapon_cs_base_gun_t* weapon( );

        OFFSET( ent_handle_t, weapon_handle( ), g_ctx->offsets( ).m_base_combat_character.m_weapon_handle );
        OFFSET( float, next_attack( ), g_ctx->offsets( ).m_base_combat_character.m_next_attack );
    };

    struct base_player_t : public base_combat_character_t {
        ALWAYS_INLINE bool alive( );

        ALWAYS_INLINE std::optional< player_info_t > info( );

        OFFSET( int, tick_base( ), g_ctx->offsets( ).m_base_player.m_tick_base );
        OFFSET( int, pred_tick( ), g_ctx->offsets( ).m_base_player.m_tick_base + sizeof( int ) );

        OFFSET( int, sim_tick( ), 0x2ac );

        OFFSET( e_life_state, life_state( ), g_ctx->offsets( ).m_base_player.m_life_state );

        OFFSET( float, duck_amt( ), g_ctx->offsets( ).m_base_player.m_duck_amt );
        OFFSET( float, duck_speed( ), g_ctx->offsets( ).m_base_player.m_duck_speed );

        OFFSET( float, spawn_time( ), g_ctx->offsets( ).m_base_player.m_spawn_time );

        OFFSET( sdk::vec3_t, view_offset( ), g_ctx->offsets( ).m_base_player.m_view_offset );

        OFFSET( sdk::qang_t, aim_punch( ), g_ctx->offsets( ).m_base_player.m_aim_punch );
        OFFSET( sdk::qang_t, view_punch( ), g_ctx->offsets( ).m_base_player.m_view_punch );

        OFFSET( sdk::vec3_t, aim_punch_vel( ), g_ctx->offsets( ).m_base_player.m_punch_vel );
        
        OFFSET( valve::cmd_context_t, ctx_cmd( ), 0x350cu );

        OFFSET( valve::user_cmd_t*, cur_cmd( ), 0x3348 );

        OFFSET( valve::user_cmd_t*, last_cmd( ), 0x3298 );
    };

    struct cs_player_t : public base_player_t {
        ALWAYS_INLINE bool friendly( cs_player_t* const with );

        ALWAYS_INLINE void update_collision_bounds( );

        ALWAYS_INLINE void invalidate_bone_cache( );

        ALWAYS_INLINE bool setup_bones( sdk::mat3x4_t* matrix, int bones_count, e_bone_flags flags, float time );

        OFFSET( float, lby( ), g_ctx->offsets( ).m_cs_player.m_lby );

        OFFSET( sdk::qang_t, eye_angles( ), g_ctx->offsets( ).m_cs_player.m_eye_angles );

#ifndef CSGO2018
        OFFSET( int, survival_team( ), g_ctx->offsets( ).m_cs_player.m_survival_team );
#endif
        OFFSET( std::uint32_t, client_effects( ), 0x68 );

        OFFSET( e_effects, effects( ), g_ctx->offsets( ).m_cs_player.m_effects );

        OFFSET( anim_state_t*, anim_state( ), g_ctx->offsets( ).m_cs_player.m_anim_state );

        OFFSET( bool, defusing( ), g_ctx->offsets( ).m_cs_player.m_defusing );

        OFFSET( bool, walking( ), g_ctx->offsets( ).m_cs_player.m_walking );
        OFFSET( bool, scoped( ), g_ctx->offsets( ).m_cs_player.m_scoped );

        VFUNC( void( __thiscall* )( void* ), update_client_side_anims( ), 224u );
    };

    struct player_record_t {
        ALWAYS_INLINE player_record_t( );

        ALWAYS_INLINE player_record_t( valve::cs_player_t* player );

        valve::cs_player_t*                             m_player{};

        float                                           m_sim_time{},
                                                        m_old_sim_time{};

        float                                           m_duck_amt{},
                                                        m_lby{};

        float                                           m_eye_yaw{},
                                                        m_max_speed{};

        sdk::qang_t                                     m_eye_angles{},
                                                        m_abs_angles{};

        sdk::vec3_t                                     m_velocity{},
                                                        m_origin{}, m_abs_origin{};

        sdk::vec3_t                                     m_mins{},
                                                        m_maxs{};

        valve::e_ent_flags			                    m_flags{};
        int								                m_sim_ticks{};

        bool							                m_did_shot{};
        int								                m_shot_tick{};

        bool                                            m_fake_player{},
                                                        m_invalid{};

        bool                                            m_walking{},
                                                        m_broke_lc{};

        valve::anim_layers_t		                    m_layers{};
        valve::pose_params_t			                m_pose_params{};

        std::array< sdk::mat3x4_t, valve::k_max_bones > m_bones{};

        bool                                            m_filled{};
    };
}

#include "impl/entities.inl"