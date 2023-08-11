#include "../../../csgo.hpp"

namespace hacks {
    void c_move::handle( valve::user_cmd_t& cmd ) const {
        if ( g_menu->main( ).m_move.m_bhop.get( )
            /* there must be some other checks here, but again, this is enough for the example */
            && !( g_local_player->self( )->flags( ) & valve::e_ent_flags::on_ground ) )
            cmd.m_buttons &= ~valve::e_buttons::in_jump;
    }

    void c_move::rotate( valve::user_cmd_t& cmd, const sdk::qang_t& wish_angles ) const {
        const auto move_type = g_local_player->self( )->move_type( );
        if ( move_type == valve::e_move_type::noclip
            || move_type == valve::e_move_type::ladder )
            return;

        sdk::vec3_t right{};
        auto fwd = cmd.m_view_angles.vectors( &right );

        if ( fwd.z( ) ) {
            fwd.z( ) = 0.f;
            fwd.normalize( );
        }
        
        if ( right.z( ) ) {
            right.z( ) = 0.f;
            right.normalize( );
        }

        if ( const auto div = right.y( ) * fwd.x( ) - right.x( ) * fwd.y( ) ) {
            sdk::vec3_t wish_right{};
            auto wish_fwd = wish_angles.vectors( &wish_right );

            if ( wish_fwd.z( ) ) {
                wish_fwd.z( ) = 0.f;
                wish_fwd.normalize( );
            }

            if ( wish_right.z( ) ) {
                wish_right.z( ) = 0.f;
                wish_right.normalize( );
            }

            const sdk::vec2_t wish_vel{
                wish_fwd.x( ) * cmd.m_move.x( ) + wish_right.x( ) * cmd.m_move.y( ),
                wish_fwd.y( ) * cmd.m_move.x( ) + wish_right.y( ) * cmd.m_move.y( )
            };

            cmd.m_move.y( ) = ( wish_vel.y( ) * fwd.x( ) - wish_vel.x( ) * fwd.y( ) ) / div;
            cmd.m_move.x( ) = ( wish_vel.x( ) * right.y( ) - wish_vel.y( ) * right.x( ) ) / div;
        }

        const auto& cvars = g_ctx->cvars( );

        const auto max_fwd_speed = cvars.cl_forwardspeed->get_float( );
        const auto max_back_speed = cvars.cl_backspeed->get_float( );
        const auto max_side_speed = cvars.cl_sidespeed->get_float( );

        cmd.m_buttons &= ~valve::e_buttons::in_move;

        if ( ( cmd.m_move.x( ) = std::clamp( cmd.m_move.x( ), -max_back_speed, max_fwd_speed ) ) )
            cmd.m_buttons |= cmd.m_move.x( ) > 0.f
                ? valve::e_buttons::in_fwd : valve::e_buttons::in_back;

        if ( ( cmd.m_move.y( ) = std::clamp( cmd.m_move.y( ), -max_side_speed, max_side_speed ) ) )
            cmd.m_buttons |= cmd.m_move.y( ) > 0.f
                ? valve::e_buttons::in_move_right : valve::e_buttons::in_move_left;
    }
}