#pragma once

using ui_clr_t = ImColor;

class c_menu {
private:
    using cfg_ui_clr_t = sdk::cfg_var_t< ui_clr_t >;

    struct main_t {
        struct move_t {
            bool        m_bhop{};
        };

        struct models_t {
            bool        m_player[ 3u ]{};
            ui_clr_t    m_player_clr[ 3u ]{};

            bool        m_player_occluded[ 3u ]{};
            ui_clr_t    m_player_occluded_clr[ 3u ]{};
        };

        sdk::cfg_var_t< move_t >	m_move{ HASH( "move" ), {} };
        sdk::cfg_var_t< models_t >	m_models{ HASH( "models" ), {} };

        bool m_hidden{};
    } m_main{};
public:
    void render( );

    ALWAYS_INLINE auto& main( );
};

inline const auto g_menu = std::make_unique< c_menu >( );

#include "impl/menu.inl"