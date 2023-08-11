#pragma once

using ui_clr_t = ImColor;

class c_menu {
private:
    using cfg_ui_clr_t = sdk::cfg_var_t< ui_clr_t >;

    struct {
        struct {
            sdk::cfg_var_t< bool > m_bhop{ HASH( "move.bhop" ), false };
        } m_move{};

        struct {
            sdk::cfg_var_t< bool >  m_player{ HASH( "models.player" ), false },
                                    m_player_occluded{ HASH( "models.player_occluded" ), false };

            cfg_ui_clr_t            m_player_clr{ HASH( "models.player.clr" ), { 1.f, 1.f, 1.f, 1.f } },
                                    m_player_occluded_clr{ HASH( "models.player_occluded.clr" ), { 1.f, 1.f, 1.f, 1.f } };
        } m_models{};

        bool m_hidden{};
    } m_main{};
public:
    void render( );

    ALWAYS_INLINE auto& main( );
};

inline const auto g_menu = std::make_unique< c_menu >( );

#include "impl/menu.inl"