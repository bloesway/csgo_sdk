#include "../../csgo.hpp"

void c_menu::render( ) {
    if ( m_main.m_hidden )
        return;

    ImGui::Begin( "wok sdk" );

    ImGui::Checkbox( "bhop", &m_main.m_move.m_bhop );

    ImGui::Checkbox( "player model chams", &m_main.m_models.m_player );
    ImGui::ColorEdit4( "player model chams color", &m_main.m_models.m_player_clr.get( ).Value.x );

    ImGui::Checkbox( "player model occluded chams", &m_main.m_models.m_player_occluded );
    ImGui::ColorEdit4( "player model occluded chams color", &m_main.m_models.m_player_occluded_clr.get( ).Value.x );

    if ( ImGui::Button( "load" ) )
        sdk::g_cfg->load( "example" );

    if ( ImGui::Button( "save" ) )
        sdk::g_cfg->save( "example" );

    ImGui::End( );
}