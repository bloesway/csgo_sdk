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
}