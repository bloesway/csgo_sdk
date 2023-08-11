#pragma once

enum struct e_font_flags : std::uint8_t {
	none,
	centered_x = 1 << 0,
	centered_y = 1 << 1,
	centered = centered_x | centered_y,
	shadow = 1 << 2,
	outline = 1 << 4
};
ENUM_BIT_OPERATORS( e_font_flags, true );

class c_renderer {
public:
	std::shared_ptr< ImDrawList > m_draw_list{};
	std::shared_ptr< ImDrawList > m_data_draw_list{};
	std::shared_ptr< ImDrawList > m_replace_draw_list{};

	std::mutex					m_mutex{};
	sdk::vec2_t					m_screen_size{};
public:
	void process( );

	void replace_draw_list( );
public:
	sdk::vec2_t calc_text_size( ImFont* font, std::string_view txt );

	void text( std::string_view txt, sdk::vec2_t pos, const sdk::argb_t& clr, ImFont* font, e_font_flags flags = e_font_flags::none );

	void line( const sdk::vec2_t& from, const sdk::vec2_t& to, const sdk::argb_t& clr );

	void rect( const sdk::vec2_t& pos, const sdk::vec2_t& size, const sdk::argb_t& clr, float rounding = 0.f );

	void rect_filled( const sdk::vec2_t& pos, const sdk::vec2_t& size, const sdk::argb_t& clr, float rounding = 0.f );

	struct {
		ImFont* m_verdana12{};
	} m_fonts{};
};

inline const auto g_render = std::make_unique< c_renderer >( );