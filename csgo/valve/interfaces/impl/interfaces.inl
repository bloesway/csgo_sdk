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
}