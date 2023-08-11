#pragma once

namespace hacks {
    class c_models {
    private:
        enum struct e_model_type : std::uint8_t {
            invalid,
            player
        };

        ALWAYS_INLINE valve::material_t* create_material( const char* name, const char* shader, const char* settings ) const;

        ALWAYS_INLINE void create_materials( );

        ALWAYS_INLINE void set_clr( const ui_clr_t& clr ) const;

        ALWAYS_INLINE e_model_type get_model_type( const valve::draw_model_info_t& info ) const;

        valve::material_t* m_flat{}, *m_flat_ignorez{};
    public:
        bool on_draw_model( valve::studio_render_t* ecx, std::uintptr_t edx, std::uintptr_t results, const valve::draw_model_info_t& info,
            sdk::mat3x4_t* bones, float* flex_weights, float* flex_delayed_weights, const sdk::vec3_t& origin, int flags
        );
    };

    inline const auto g_models = std::make_unique< c_models >( );
}

#include "impl/models.inl"