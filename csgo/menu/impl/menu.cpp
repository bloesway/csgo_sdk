#include "../../csgo.hpp"

static const char* g_player_type[ ] = {
    "enemy",
    "team",
    "local"
};

void c_menu::render( ) {
	if ( m_main.m_hidden )
		return;

	ImGui::Begin( "csgo sdk" );

	if ( ImGui::CollapsingHeader( "models##main" ) ) {
		static auto current_player = 0;
		ImGui::Combo( "player type", &current_player, g_player_type, IM_ARRAYSIZE( g_player_type ) );

		ImGui::Spacing( );

		ImGui::Checkbox( "player model chams", &m_main.m_models.get( ).m_player[ current_player ] );
		ImGui::ColorEdit4( "player model chams color", &m_main.m_models.get( ).m_player_clr[ current_player ].Value.x );

		ImGui::Checkbox( "player model occluded chams", &m_main.m_models.get( ).m_player_occluded[ current_player ] );
		ImGui::ColorEdit4( "player model occluded chams color", &m_main.m_models.get( ).m_player_occluded_clr[ current_player ].Value.x );
	}

	if ( ImGui::CollapsingHeader( "movement##main" ) ) {
		ImGui::Checkbox( "bhop", &m_main.m_move.get( ).m_bhop );
	}

	if ( ImGui::CollapsingHeader( "configs##main" ) ) {
		static char name[ 64u ]{};

		static int cfg = -1;

		static std::string config{};
		static std::vector< std::string > configs{};

		if ( !configs.empty( ) && configs.size( ) > cfg )
			config = configs.at( cfg );

		if ( ImGui::BeginCombo( "cfg", config.data( ) ) ) {
			for ( int i = 0; i < configs.size( ); i++ )
				if ( ImGui::Selectable( configs[ i ].data( ), cfg == i ) ) {
					cfg = i;
				}

			ImGui::EndCombo( );
		}

		ImGui::InputText( "name##cfg", name, 64u );

		if ( ImGui::Button( "create##cfg" ) )
			sdk::g_cfg->save( name );

		if ( ImGui::Button( "save##cfg" ) && !configs.empty( ) )
			sdk::g_cfg->save( configs.at( cfg ) );

		if ( ImGui::Button( "load##cfg" ) && !configs.empty( ) )
			sdk::g_cfg->load( configs.at( cfg ) );

		if ( ImGui::Button( "delete##cfg" ) && !configs.empty( ) )
			std::remove( ( std::filesystem::path{ SDK_CFG_ID_OBJECT } /= configs.at( cfg ) ).string( ).data( ) );

		if ( ImGui::Button( "refresh##cfg" ) ) {
			cfg = 0;

			configs.clear( );

			for ( auto& file : std::filesystem::recursive_directory_iterator{ std::filesystem::path{ SDK_CFG_ID_OBJECT } } )
				configs.emplace_back( file.path( ).filename( ).string( ) );
		}
	}

	ImGui::End( );
}