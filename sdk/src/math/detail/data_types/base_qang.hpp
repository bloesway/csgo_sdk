#pragma once

namespace sdk::detail {
    template < typename, std::size_t _size >
        requires ( _size >= 2u )
    struct base_vec_t;

    template < typename _value_t, std::size_t _rows_count, std::size_t _columns_count >
    struct base_mat_t;

    template < typename _value_t >
    struct base_qang_t final
        : public array_wrapper_t< _value_t, 3u, base_qang_t< _value_t > > {
    private:
        using base_t = array_wrapper_t< _value_t, 3u, base_qang_t< _value_t > >;
    public:
        ALWAYS_INLINE constexpr base_qang_t( ) = default;

        ALWAYS_INLINE constexpr base_qang_t( const _value_t value )
            : base_t{ value, value, value } {}

        ALWAYS_INLINE constexpr base_qang_t(
            const _value_t x, const _value_t y, const _value_t z
        ) : base_t{ x, y, z } {}

        ALWAYS_INLINE constexpr _value_t& x( ) { return base_t::at( 0u ); }

        ALWAYS_INLINE constexpr _value_t x( ) const { return base_t::at( 0u ); }

        ALWAYS_INLINE constexpr _value_t& y( ) { return base_t::at( 1u ); }

        ALWAYS_INLINE constexpr _value_t y( ) const { return base_t::at( 1u ); }

        ALWAYS_INLINE constexpr _value_t& z( ) { return base_t::at( 2u ); }

        ALWAYS_INLINE constexpr _value_t z( ) const { return base_t::at( 2u ); }

        ALWAYS_INLINE base_vec_t< _value_t, 3u > vectors(
            base_vec_t< _value_t, 3u >* const right = nullptr,
            base_vec_t< _value_t, 3u >* const up = nullptr
        ) const {
            const auto rad_x = to_rad( x( ) ), rad_y = to_rad( y( ) );

            const auto cos_x = std::cos( rad_x ), sin_x = std::sin( rad_x );
            const auto cos_y = std::cos( rad_y ), sin_y = std::sin( rad_y );

            enough_float_t< _value_t > cos_z{}, sin_z{};
            if ( right
                || up ) {
                const auto rad_z = to_rad( z( ) );

                cos_z = std::cos( rad_z );
                sin_z = std::sin( rad_z );
            }

            if ( right )
                *right = {
                    -sin_z * sin_x * cos_y + cos_z * sin_y,
                    -sin_z * sin_x * sin_y + -cos_z * cos_y,
                    -sin_z * cos_x
                };

            if ( up )
                *up = {
                    cos_z * sin_x * cos_y + sin_z * sin_y,
                    cos_z * sin_x * sin_y + -sin_z * cos_y,
                    cos_z * cos_x
                };

            return { cos_x * cos_y, cos_x * sin_y, -sin_x };
        }

        ALWAYS_INLINE base_mat_t< _value_t, 3u, 4u > matrix( ) const {
            const auto rad_x = to_rad( x( ) ), rad_y = to_rad( y( ) ), rad_z = to_rad( z( ) );

            const auto cos_x = std::cos( rad_x ), sin_x = std::sin( rad_x );
            const auto cos_y = std::cos( rad_y ), sin_y = std::sin( rad_y );
            const auto cos_z = std::cos( rad_z ), sin_z = std::sin( rad_z );

            const auto cz_cy = cos_z * cos_y;
            const auto cz_sy = cos_z * sin_y;
            const auto sz_cy = sin_z * cos_y;
            const auto sz_sy = sin_z * sin_y;

            return {
                cos_x * cos_y, sin_x * sz_cy - cz_sy, sin_x * cz_cy + sz_sy, 0.f,
                cos_x * sin_y, sin_x * sz_sy + cz_cy, sin_x * cz_sy - sz_cy, 0.f,
                -sin_x, sin_z * cos_x, cos_z * cos_x, 0.f
            };
        }

        ALWAYS_INLINE base_qang_t< _value_t >& normalize( ) {
            for ( auto& element : base_t::m_elements )
                element = normalize_angle( element );

            return *this;
        }

        ALWAYS_INLINE base_qang_t< _value_t > normalized( ) const {
            auto ret = *this;

            return ret.normalize( );
        }
    };
}