#include "../../csgo.hpp"

void c_renderer::process( ) {
	m_draw_list->_ResetForNewFrame( );
	m_draw_list->PushClipRectFullScreen( );

	m_screen_size = *reinterpret_cast< sdk::vec2_t* >( &ImGui::GetIO( ).DisplaySize );

	text( "hello world!", { m_screen_size.x( ) - 100.f, 15.f }, { 255, 255, 255, 255 }, m_fonts.m_verdana12, e_font_flags::outline );

	{
		const auto lock = std::unique_lock< std::mutex >( m_mutex );

		*m_data_draw_list = *m_draw_list;
	}
}

void c_renderer::replace_draw_list( ) {
	const auto lock = std::unique_lock< std::mutex >( m_mutex, std::try_to_lock );
	if ( lock.owns_lock( ) )
		*m_replace_draw_list = *m_data_draw_list;

	*ImGui::GetBackgroundDrawList( ) = *m_replace_draw_list;
}

sdk::vec2_t c_renderer::calc_text_size( ImFont* font, std::string_view txt ) {
	if ( !font
		|| txt.empty( )
		|| !font->IsLoaded( ) )
		return sdk::vec2_t( );

	const auto size = font->CalcTextSizeA( font->FontSize, std::numeric_limits< float >::max( ), 0.f, txt.data( ) );

	return sdk::vec2_t( ( float )( int )( size.x + 0.95f ), size.y ); // (float)(int) - IM_FLOOR
}

void c_renderer::text( std::string_view txt, sdk::vec2_t pos, const sdk::argb_t& clr, ImFont* font, e_font_flags flags ) {
	if ( !font
		|| txt.empty( )
		|| clr.a( ) <= 0
		|| !font->IsLoaded( ) )
		return;

	const auto centered_x = flags & e_font_flags::centered_x;
	const auto centered_y = flags & e_font_flags::centered_y;

	if ( centered_x
		|| centered_y ) {
		const auto text_size = calc_text_size( font, txt );

		if ( centered_x )
			pos.x( ) -= text_size.x( ) / 2.f;

		if ( centered_y )
			pos.y( ) -= text_size.y( ) / 2.f;
	}

	m_draw_list->PushTextureID( font->ContainerAtlas->TexID );

	if ( flags & e_font_flags::shadow )
		m_draw_list->AddTextSoftShadow( font, font->FontSize, 
			*reinterpret_cast< ImVec2* >( &pos ), clr.hex( ), txt.data( ) 
		);
	else if ( flags & e_font_flags::outline )
		m_draw_list->AddTextOutline( font, font->FontSize, 
			*reinterpret_cast< ImVec2* >( &pos ), clr.hex( ), txt.data( ) 
		);
	else
		m_draw_list->AddText( font, font->FontSize, 
			*reinterpret_cast< ImVec2* >( &pos ), clr.hex( ), txt.data( ) 
		);

	m_draw_list->PopTextureID( );
}

void c_renderer::line( const sdk::vec2_t& from, const sdk::vec2_t& to, const sdk::argb_t& clr ) {
	m_draw_list->AddLine( *reinterpret_cast< const ImVec2* >( &from ), 
		*reinterpret_cast< const ImVec2* >( &to ), clr.hex( ) 
	);
}

void c_renderer::rect( const sdk::vec2_t& pos, const sdk::vec2_t& size, const sdk::argb_t& clr, float rounding ) {
	m_draw_list->AddRect( *reinterpret_cast< const ImVec2* >( &pos ),
		ImVec2( pos.x( ) + size.x( ), pos.y( ) + size.y( ) ), clr.hex( ), rounding 
	);
}

void c_renderer::rect_filled( const sdk::vec2_t& pos, const sdk::vec2_t& size, const sdk::argb_t& clr, float rounding ) {
	m_draw_list->AddRectFilled( *reinterpret_cast< const ImVec2* >( &pos ), 
		ImVec2( pos.x( ) + size.x( ), pos.y( ) + size.y( ) ), clr.hex( ), rounding 
	);
}