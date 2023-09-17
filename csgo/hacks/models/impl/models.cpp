#include "../../../csgo.hpp"

namespace hacks {
    bool c_models::on_draw_model( valve::studio_render_t* ecx, std::uintptr_t edx, std::uintptr_t results, const valve::draw_model_info_t& info,
        sdk::mat3x4_t* bones, float* flex_weights, float* flex_delayed_weights, const sdk::vec3_t& origin, int flags
    ) {
        const auto type = get_model_type( info );
        if ( type == e_model_type::invalid )
            return false;

        if ( !m_flat )
            create_materials( );

        bool blocked{};

        switch ( type ) {
            case e_model_type::player:
                {
                    const auto player = reinterpret_cast< valve::cs_player_t* >(
                        reinterpret_cast< std::uintptr_t >( info.m_renderable ) - 0x4u
                    );

                    if ( player && player->alive( ) ) {
                        auto player_type = 0u;

                        if ( player->friendly( g_local_player->self( ) ) ) {
                            player_type = 1u;

                            if ( player->networkable( )->index( ) == g_local_player->self( )->networkable( )->index( ) )
                                player_type = 2u;
                        }

                        if ( player_type == 0u ) {
                            if ( g_menu->main( ).m_models.get( ).m_player_occluded[ player_type ] ) {
                                valve::g_studio_render->forced_material_override( m_flat_ignorez );

                                set_clr( g_menu->main( ).m_models.get( ).m_player_occluded_clr[ player_type ] );

                                hooks::o_draw_model( ecx, edx, results, info, bones, flex_weights, flex_delayed_weights, origin, flags );

                                if ( !g_menu->main( ).m_models.get( ).m_player[ player_type ] )
                                    valve::g_studio_render->forced_material_override( nullptr );
                            }

                            if ( g_menu->main( ).m_models.get( ).m_player[ player_type ] ) {
                                valve::g_studio_render->forced_material_override( m_flat );

                                set_clr( g_menu->main( ).m_models.get( ).m_player_clr[ player_type ] );

                                hooks::o_draw_model( ecx, edx, results, info, bones, flex_weights, flex_delayed_weights, origin, flags );

                                valve::g_studio_render->forced_material_override( nullptr );

                                blocked = true;
                            }
                        }
                        else if ( player_type == 1u ) {
                            if ( g_menu->main( ).m_models.get( ).m_player_occluded[ player_type ] ) {
                                valve::g_studio_render->forced_material_override( m_flat_ignorez );

                                set_clr( g_menu->main( ).m_models.get( ).m_player_occluded_clr[ player_type ] );

                                hooks::o_draw_model( ecx, edx, results, info, bones, flex_weights, flex_delayed_weights, origin, flags );

                                if ( !g_menu->main( ).m_models.get( ).m_player[ player_type ] )
                                    valve::g_studio_render->forced_material_override( nullptr );
                            }

                            if ( g_menu->main( ).m_models.get( ).m_player[ player_type ] ) {
                                valve::g_studio_render->forced_material_override( m_flat );

                                set_clr( g_menu->main( ).m_models.get( ).m_player_clr[ player_type ] );

                                hooks::o_draw_model( ecx, edx, results, info, bones, flex_weights, flex_delayed_weights, origin, flags );

                                valve::g_studio_render->forced_material_override( nullptr );

                                blocked = true;
                            }
                        }
                        else if ( player_type == 2u ) {
                            if ( g_menu->main( ).m_models.get( ).m_player_occluded[ player_type ] ) {
                                valve::g_studio_render->forced_material_override( m_flat_ignorez );

                                set_clr( g_menu->main( ).m_models.get( ).m_player_occluded_clr[ player_type ] );

                                hooks::o_draw_model( ecx, edx, results, info, bones, flex_weights, flex_delayed_weights, origin, flags );

                                if ( !g_menu->main( ).m_models.get( ).m_player[ player_type ] )
                                    valve::g_studio_render->forced_material_override( nullptr );
                            }

                            if ( g_menu->main( ).m_models.get( ).m_player[ player_type ] ) {
                                valve::g_studio_render->forced_material_override( m_flat );

                                set_clr( g_menu->main( ).m_models.get( ).m_player_clr[ player_type ] );

                                hooks::o_draw_model( ecx, edx, results, info, bones, flex_weights, flex_delayed_weights, origin, flags );

                                valve::g_studio_render->forced_material_override( nullptr );

                                blocked = true;
                            }
                        }
                    }
                } break;

            default:
                break;
        }

        set_clr( { 1.f, 1.f, 1.f, 1.f } );

        return blocked;
    }
}