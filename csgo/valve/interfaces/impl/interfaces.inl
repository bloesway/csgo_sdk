#pragma once

#include "../interfaces.hpp"

namespace valve {
    ALWAYS_INLINE bool studio_render_t::is_forced_material_override( ) {
        if ( m_override_material )
            if ( const auto name = m_override_material->name( ) )
                return *reinterpret_cast< const std::uint32_t* >( name + 4 ) == 'wolg';

        return m_override_type == e_override_type::depth_write
            || m_override_type == e_override_type::ssao_depth_write;
    }

    /* it can be anywhere else */
    ALWAYS_INLINE int to_ticks( const float time ) {
        return static_cast< int >( time / g_global_vars->m_interval_per_tick + 0.5f );
    }

    /* it can be anywhere else */
    ALWAYS_INLINE float to_time( const int ticks ) {
        return ticks * g_global_vars->m_interval_per_tick;
    }

    /* it can be anywhere else */
    ALWAYS_INLINE bool to_screen( const sdk::vec3_t& world, sdk::vec2_t& screen ) {
        const auto& matrix = *reinterpret_cast< sdk::mat4x4_t* >( 
            *g_ctx->offsets().m_view_matrix.as< uintptr_t* >( ) + 0xb0u
        );

        screen.x( ) = matrix.at( 0u, 0u ) * world.x( ) + matrix.at( 0u, 1u ) * world.y( ) 
            + matrix.at( 0u, 2u ) * world.z( ) + matrix.at( 0u, 3u );

        screen.y( ) = matrix.at( 1u, 0u ) * world.x( ) + matrix.at( 1u, 1u ) * world.y( )
            + matrix.at( 1u, 2u ) * world.z( ) + matrix.at( 1u, 3u );

        const auto width = matrix.at( 3u, 0u ) * world.x( ) + matrix.at( 3u, 1u ) * world.y( ) 
            + matrix.at( 3u, 2u ) * world.z( ) + matrix.at( 3u, 3u );

        if ( width < 0.00001f ) {
            screen *= 1000.f;

            return false;
        }

        screen /= width;

        screen.x( ) = g_render->m_screen_size.x( ) * 0.5f
            + ( screen.x( ) * g_render->m_screen_size.x( ) ) * 0.5f;

        screen.y( ) = g_render->m_screen_size.y( ) * 0.5f
            - ( screen.y( ) * g_render->m_screen_size.y( ) ) * 0.5f;

        return true;
    }
}