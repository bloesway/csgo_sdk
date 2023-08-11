#include "../../../csgo.hpp"

namespace hacks {
    bool c_models::on_draw_model( valve::studio_render_t* ecx, std::uintptr_t edx, std::uintptr_t results, const valve::draw_model_info_t& info,
        sdk::mat3x4_t* bones, float* flex_weights, float* flex_delayed_weights, const sdk::vec3_t& origin, int flags
    ) {
        const auto& cfg = g_menu->main( ).m_models;

        const auto type = get_model_type( info );
        if ( type == e_model_type::invalid )
            return false;

        if ( !m_flat )
            create_materials( );

        bool blocked{};

        switch ( type ) {
        case e_model_type::player:
            if ( cfg.m_player_occluded.get( ) ) {
                valve::g_studio_render->forced_material_override( m_flat_ignorez );

                set_clr( cfg.m_player_occluded_clr.get( ) );

                hooks::o_draw_model( ecx, edx, results, info, bones, flex_weights, flex_delayed_weights, origin, flags );

                if ( !cfg.m_player.get( ) )
                    valve::g_studio_render->forced_material_override( nullptr );
            }

            if ( cfg.m_player.get( ) ) {
                valve::g_studio_render->forced_material_override( m_flat );

                set_clr( cfg.m_player_clr.get( ) );

                hooks::o_draw_model( ecx, edx, results, info, bones, flex_weights, flex_delayed_weights, origin, flags );

                valve::g_studio_render->forced_material_override( nullptr );

                blocked = true;
            }

            break;
        default: break;
        }

        set_clr( { 1.f, 1.f, 1.f, 1.f } );

        return blocked;
    }
}